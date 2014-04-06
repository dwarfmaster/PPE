
#include "calib.hpp"
#include "cv.h"
#include "cxmisc.h"
#include "highgui.h"
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

namespace libcv
{
    CalibCam::CalibCam()
        : m_display(true), m_mx1(NULL), m_mx2(NULL), m_my1(NULL), m_my2(NULL)
    {
    }

    CalibCam::~CalibCam()
    {
        if(m_mx1 != NULL)
            cvReleaseMat(&m_mx1);
        if(m_mx2 != NULL)
            cvReleaseMat(&m_mx2);
        if(m_my1 != NULL)
            cvReleaseMat(&m_my1);
        if(m_my2 != NULL)
            cvReleaseMat(&m_my2);
    }

    bool CalibCam::load(const std::string& path)
    {
        std::string used;
        used = path + "/mx1.xml";
        m_mx1 = (CvMat*)cvLoad(used.c_str());
        used = path + "/mx2.xml";
        m_mx2 = (CvMat*)cvLoad(used.c_str());
        used = path + "/my1.xml";
        m_my1 = (CvMat*)cvLoad(used.c_str());
        used = path + "/my2.xml";
        m_my2 = (CvMat*)cvLoad(used.c_str());

        used = path + "/size";
        std::ifstream ifs(used.c_str());
        if(!ifs) {
            std::cout << "Couldn't load !" << std::endl;
            return false;
        }
        ifs >> m_imgSize.width >> m_imgSize.height;
        std::cout << "Size loaded : " << m_imgSize.width << "x" << m_imgSize.height << std::endl;
        return true;
    }

    bool CalibCam::save(const std::string& path) const
    {
        std::string used = path;
        used += "/mx1.xml";
        cvSave(used.c_str(), m_mx1, "mx1");
        used = path + "/mx2.xml";
        cvSave(used.c_str(), m_mx2, "mx2");
        used = path + "/my1.xml";
        cvSave(used.c_str(), m_my1, "my1");
        used = path + "/my2.xml";
        cvSave(used.c_str(), m_my2, "my2");
        used = path + "/size";
        std::ofstream ofs(used.c_str());
        if(!ofs) {
            std::cout << "Couldn't save !" << std::endl;
            return false;
        }
        ofs << m_imgSize.width << " " << m_imgSize.height << std::endl;
        std::cout << "Size saved : " << m_imgSize.width << "x" << m_imgSize.height << std::endl;

        return true;
    }

    void CalibCam::start(int nx, int ny, float size, CvSize img)
    {
        m_nx = nx;
        m_ny = ny;
        m_size = size;
        m_imgSize = img;
        m_frames = 0;

        if(m_mx1 != NULL)
            cvReleaseMat(&m_mx1);
        if(m_mx2 != NULL)
            cvReleaseMat(&m_mx2);
        if(m_my1 != NULL)
            cvReleaseMat(&m_my1);
        if(m_my2 != NULL)
            cvReleaseMat(&m_my2);
        m_mx1 = m_mx2 = m_my1 = m_my2 = NULL;
    }

    void CalibCam::end()
    {
        size_t nframes = m_frames;
        if(nframes == 0)
            return;
        int n = m_nx * m_ny;
        std::vector<CvPoint3D32f> objectPoints(nframes * m_nx * m_ny);

        /* Store normal coordinates of corners into the objec tPoints list */
        for(int i = 0; i < m_ny; ++i) {
            for(int j = 0; j < m_nx; ++j)
                objectPoints[i*m_nx + j] = cvPoint3D32f(i * m_size, j * m_size, 0);
        }
        for(size_t i = 1; i < nframes; ++i)
            std::copy(objectPoints.begin(), objectPoints.begin() + n, objectPoints.begin() + i*n);

        /* Prepare some matrix */
        std::vector<int> npoints(nframes, n);
        int N = nframes * n;
        CvMat _objPoints  = cvMat(1, N,       CV_32FC3, &objectPoints[0]);
        CvMat _imgPoints1 = cvMat(1, N,       CV_32FC2, &m_corners[0][0]);
        CvMat _imgPoints2 = cvMat(1, N,       CV_32FC2, &m_corners[1][0]);
        CvMat _npoints    = cvMat(1, nframes, CV_32S, &npoints[0]);

        double M1[3][3], M2[3][3], D1[5],   D2[5];
        double R[3][2],  T[3],     E[3][3], F[3][3];
        double Q[4][4];
        CvMat _M1 = cvMat(3, 3, CV_64F, M1);
        CvMat _M2 = cvMat(3, 3, CV_64F, M2);
        CvMat _D1 = cvMat(1, 5, CV_64F, D1);
        CvMat _D2 = cvMat(1, 5, CV_64F, D2);
        CvMat _R  = cvMat(3, 3, CV_64F, R);
        CvMat _T  = cvMat(3, 1, CV_64F, T);
        CvMat _E  = cvMat(3, 3, CV_64F, E);
        CvMat _F  = cvMat(3, 3, CV_64F, F);
        CvMat _Q  = cvMat(4, 4, CV_64F, Q);
        cvSetIdentity(&_M1);
        cvSetIdentity(&_M2);
        cvZero(&_D1);
        cvZero(&_D2);

        /* Calibrate the stereo cameras */
        std::cout << "Calibrating ..." << std::endl;
        cvStereoCalibrate(&_objPoints, &_imgPoints1,
                &_imgPoints2, &_npoints,
                &_M1, &_D1, &_M2, &_D2,
                m_imgSize, &_R, &_T, &_E, &_F,
                cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS,
                    100, 1e-5),
                CV_CALIB_FIX_ASPECT_RATIO  +
                CV_CALIB_ZERO_TANGENT_DIST +
                CV_CALIB_SAME_FOCAL_LENGTH);
        std::cout << "Done." << std::endl;

        /* Compute m_m{x,y}{1,2} */
        m_mx1 = cvCreateMat(m_imgSize.height, m_imgSize.width, CV_32F);
        m_my1 = cvCreateMat(m_imgSize.height, m_imgSize.width, CV_32F);
        m_mx2 = cvCreateMat(m_imgSize.height, m_imgSize.width, CV_32F);
        m_my2 = cvCreateMat(m_imgSize.height, m_imgSize.width, CV_32F);
        double R1[3][3], R2[3][3], P1[3][4], P2[3][4];
        CvMat _R1 = cvMat(3, 3, CV_64F, R1);
        CvMat _R2 = cvMat(3, 3, CV_64F, R2);
        CvMat _P1 = cvMat(3, 4, CV_64F, P1);
        CvMat _P2 = cvMat(3, 4, CV_64F, P2);
        cvStereoRectify(&_M1, &_M2, &_D1, &_D2, m_imgSize,
                &_R, &_T,
                &_R1, &_R2, &_P1, &_P2, &_Q,
                0);
        cvInitUndistortRectifyMap(&_M1, &_D1, &_R1, &_P1, m_mx1, m_my1);
        cvInitUndistortRectifyMap(&_M2, &_D2, &_R2, &_P2, m_mx2, m_my2);
    }

    bool CalibCam::compute(IplImage* i1, IplImage* i2)
    {
        std::vector<CvPoint2D32f> ret1 = computeOne(i1, "Left");
        std::vector<CvPoint2D32f> ret2 = computeOne(i2, "Right");
        if(ret1.empty() || ret2.empty())
            return false;

        size_t N = m_corners[0].size();
        m_corners[0].resize(N + m_nx * m_ny, cvPoint2D32f(0,0));
        std::copy(ret1.begin(), ret1.end(), m_corners[0].begin() + N);
        N = m_corners[1].size();
        m_corners[1].resize(N + m_nx * m_ny, cvPoint2D32f(0,0));
        std::copy(ret2.begin(), ret2.end(), m_corners[1].begin() + N);
        ++m_frames;
        return true;
    }

    std::vector<CvPoint2D32f> CalibCam::computeOne(IplImage* img, const std::string& win)
    {
        /* Find the chessboard corners */
        std::vector<CvPoint2D32f> temp(m_nx * m_ny);
        int count = 0;
        int result = cvFindChessboardCorners(img, cvSize(m_nx, m_ny),
                &temp[0], &count,
                CV_CALIB_CB_ADAPTIVE_THRESH |
                CV_CALIB_CB_NORMALIZE_IMAGE);
        if(!result || count != m_nx*m_ny) {
            temp.clear();
            return temp;
        }

        cvFindCornerSubPix(img, &temp[0], count,
                cvSize(11, 11), cvSize(-1, -1),
                cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS,
                    30, 0.01));

        if(m_display) {
            cvDrawChessboardCorners(img, cvSize(m_nx, m_ny), &temp[0],
                    count, result);
            cvShowImage(win.c_str(), img);
        }

        return temp;
    }

}



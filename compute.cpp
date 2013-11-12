
#include "compute.hpp"
#include "calib.hpp"
#include "cv.h"
#include "cxmisc.h"
#include "highgui.h"

namespace libcv
{
    Compute::Compute(CalibCam* cc)
        : m_x1(NULL), m_x2(NULL), m_y1(NULL), m_y2(NULL), m_pair(NULL)
    {
        m_size = cc->m_imgSize;
        m_x1   = cc->m_mx1;
        m_x2   = cc->m_mx2;
        m_y1   = cc->m_my1;
        m_y2   = cc->m_my2;
        m_pair = cvCreateMat(m_size.height, m_size.width * 2, CV_8UC3);
    }

    Compute::~Compute()
    {
        if(m_pair != NULL)
            cvReleaseMat(&m_pair);
    }

    void Compute::process(IplImage* left, IplImage* right)
    {
        static CvMat* vdisp = NULL;
        if(vdisp)
            cvReleaseMat(&vdisp);

        /* Creating the necessary matrixs */
        CvMat* img1  = cvCreateMat(m_size.height, m_size.width, CV_8U);
        CvMat* img2  = cvCreateMat(m_size.height, m_size.width, CV_8U);
        CvMat* img1r = cvCreateMat(m_size.height, m_size.width, CV_8U);
        CvMat* img2r = cvCreateMat(m_size.height, m_size.width, CV_8U);
        CvMat* disp  = cvCreateMat(m_size.height, m_size.width, CV_16S);
        vdisp = cvCreateMat(m_size.height, m_size.width, CV_8U);

        /* Create the bm state for finding stereo correspondances */
        CvStereoBMState* BMState = cvCreateStereoBMState();
        BMState->preFilterSize       = 5;
        BMState->preFilterCap        = 63;
        BMState->SADWindowSize       = 21;
        BMState->minDisparity        = 0;
        BMState->numberOfDisparities = 64;
        BMState->textureThreshold    = 0;
        BMState->uniquenessRatio     = 0;

        /* Stereo correspondance */
        cvCvtColor(left,  img1, CV_BGR2GRAY);
        cvCvtColor(right, img2, CV_BGR2GRAY);
        cvRemap(img1, img1r, m_x1, m_y1);
        cvRemap(img2, img2r, m_x2, m_y2);
        cvFindStereoCorrespondenceBM(img1r, img2r, disp,
                BMState);
        cvNormalize(disp, vdisp, 0, 256, CV_MINMAX);
        /**********************/
        cvNamedWindow("Disp");
        cvShowImage("Disp", vdisp);
        /**********************/

        /* Releasing */
        cvReleaseStereoBMState(&BMState);
        cvReleaseMat(&img1r);
        cvReleaseMat(&img2r);
        cvReleaseMat(&disp);
    }

}



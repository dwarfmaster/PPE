
#ifndef DEF_LIBCV_CALIB
#define DEF_LIBCV_CALIB

#include <vector>
#include <string>
#include <opencv/cv.h>

namespace libcv
{
    class CalibCam
    {
        public:
            CalibCam();
            CalibCam(const CalibCam&) = delete;
            ~CalibCam();

            /* Load the calibration datas from a file */
            bool load(const std::string& path);
            /* Save the calibration data to a file */
            bool save(const std::string& path) const;

            /* Calibrate the cameras with a list of pictures from the two cameras
             * cam1 and cam2 must have the same size
             */
            void start(int nx, int ny, float size, CvSize img);
            void end();
            bool compute(IplImage* i1, IplImage* i2);

        private:
            int m_nx;
            int m_ny;
            float m_size;
            CvSize m_imgSize;
            bool m_display;

            std::vector<CvPoint2D32f> m_corners[2];
            size_t m_frames;
            CvMat* m_mx1;
            CvMat* m_mx2;
            CvMat* m_my1;
            CvMat* m_my2;

            /* Internal functions */
            std::vector<CvPoint2D32f> computeOne(IplImage* img, const std::string& win = "");
    };
}

#endif


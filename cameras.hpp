
#ifndef DEF_LIBCV_CAMERAS
#define DEF_LIBCV_CAMERAS

#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include <exception>

namespace libcv
{
    class Cameras
    {
        public:
            Cameras(int l, bool delay = true);
            Cameras() = delete;
            Cameras(const Cameras&) = delete;
            ~Cameras();
            bool loaded() const;

            /* To call once in each iteration of main loop */
            bool queryFrames();
            /* Get the pictures (return mustn't be free'd */
            cv::Mat get() const;
            cv::Size size() const;

        private:
            cv::VideoCapture m_cpt;
            cv::Mat m_img;
            CvSize m_size;
            int m_loops;
    };
}

#endif


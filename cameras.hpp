
#ifndef DEF_LIBCV_CAMERAS
#define DEF_LIBCV_CAMERAS

#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include <exception>

namespace libcv
{
    class CamException : public std::exception
    {
        public:
            CamException() = delete;
            explicit CamException(const char* msg) noexcept;
            virtual const char* what() const noexcept;

        private:
            const char* m_msg;
    };

    class Cameras
    {
        public:
            Cameras(int l, int r, bool delay = true);
            Cameras() = delete;
            Cameras(const Cameras&) = delete;
            ~Cameras();

            /* To call once in each iteration of main loop */
            bool queryFrames();
            /* Get the pictures (return mustn't be free'd */
            IplImage* left() const;
            IplImage* right() const;
            CvSize size() const;

        private:
            CvCapture* m_left;
            CvCapture* m_right;
            IplImage* m_gleft;
            IplImage* m_gright;
            CvSize m_size;
            int m_loops;
    };
}

#endif


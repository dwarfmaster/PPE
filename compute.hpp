
#ifndef DEF_COMPUTE
#define DEF_COMPUTE

#include "calib.hpp"
#include <opencv/cv.h>

namespace libcv
{
    class Compute
    {
        public:
            Compute(CalibCam* cc);
            ~Compute();

            /* TODO improve interface */
            void process(IplImage* left, IplImage* right);

        private:
            CvMat* m_x1;
            CvMat* m_x2;
            CvMat* m_y1;
            CvMat* m_y2;
            CvMat* m_pair;
            CvSize m_size;
    };
}

#endif


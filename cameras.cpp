
#include "cameras.hpp"

namespace libcv
{
    CamException::CamException(const char* msg) noexcept
        : m_msg(msg)
    {}

    const char* CamException::what() const noexcept
    {
        return m_msg;
    }

    Cameras::Cameras(int l, int r, bool delay)
        : m_left(NULL), m_right(NULL), m_gleft(NULL), m_gright(NULL), m_loops(0)
    {
        m_left  = cvCaptureFromCAM(l);
        m_right = cvCaptureFromCAM(r);
        if(!m_left)
            throw CamException("Couldn't open left camera.");;
        if(!m_right)
            throw CamException("Couldn't open right camera.");

        m_size.width  = cvGetCaptureProperty(m_left, CV_CAP_PROP_FRAME_WIDTH);
        m_size.height = cvGetCaptureProperty(m_left, CV_CAP_PROP_FRAME_HEIGHT);
        if(m_size.width != cvGetCaptureProperty(m_right, CV_CAP_PROP_FRAME_WIDTH)
                    || m_size.height != cvGetCaptureProperty(m_right, CV_CAP_PROP_FRAME_HEIGHT))
            throw CamException("The left and right cameras doesn't have the same size.");

        m_gleft  = cvCreateImage(m_size, IPL_DEPTH_8U, 1);
        m_gright = cvCreateImage(m_size, IPL_DEPTH_8U, 1);
        if(!m_gleft || !m_gright)
            throw CamException("Couldn't load internal image buffers in Cameras.");

        if(delay)
            m_loops = 10;
        else
            m_loops = 1;
    }

    Cameras::~Cameras()
    {
        if(m_left)
            cvReleaseCapture(&m_left);
        if(m_right)
            cvReleaseCapture(&m_right);
        if(m_gleft)
            cvReleaseImage(&m_gleft);
        if(m_gright)
            cvReleaseImage(&m_gright);
    }

    bool Cameras::queryFrames()
    {
        /* Clear the buffered images and store the last one */
        for(int i = 0; i < m_loops; ++i) {
            if(!cvGrabFrame(m_left))
                return false;
            if(!cvGrabFrame(m_right))
                return false;
        }

        IplImage* tmp = cvRetrieveFrame(m_left);
        cvCvtColor(tmp, m_gleft, CV_BGR2GRAY);
        tmp = cvRetrieveFrame(m_right);
        cvCvtColor(tmp, m_gright, CV_BGR2GRAY);
        return true;
    }

    IplImage* Cameras::left() const
    {
        return m_gleft;
    }

    IplImage* Cameras::right() const
    {
        return m_gright;
    }

    CvSize Cameras::size() const
    {
        return m_size;
    }

}



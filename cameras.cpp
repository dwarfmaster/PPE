
#include "cameras.hpp"

namespace libcv
{
    Cameras::Cameras(int l, bool delay)
        : m_cpt(NULL), m_img(NULL), m_loops(0)
    {
        m_cpt = cvCaptureFromCAM(l);
        if(!m_cpt)
            return;

        m_size.width  = cvGetCaptureProperty(m_cpt, CV_CAP_PROP_FRAME_WIDTH);
        m_size.height = cvGetCaptureProperty(m_cpt, CV_CAP_PROP_FRAME_HEIGHT);

        if(delay)
            m_loops = 10;
        else
            m_loops = 1;
    }

    Cameras::~Cameras()
    {
        if(m_cpt)
            cvReleaseCapture(&m_cpt);
    }
            
    bool Cameras::loaded() const
    {
        return m_cpt != NULL;
    }

    bool Cameras::queryFrames()
    {
        /* Clear the buffered images and store the last one */
        for(int i = 0; i < m_loops; ++i) {
            if(!cvGrabFrame(m_cpt))
                return false;
        }

        m_img = cvRetrieveFrame(m_cpt);
        return true;
    }

    IplImage* Cameras::get() const
    {
        return m_img;
    }

    CvSize Cameras::size() const
    {
        return m_size;
    }

}



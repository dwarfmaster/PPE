
#include "cameras.hpp"

namespace libcv
{
    Cameras::Cameras(int l, bool delay)
    {
        m_cpt.open(l);
        if(!m_cpt.isOpened())
            return;

        m_size.width  = m_cpt.get(CV_CAP_PROP_FRAME_WIDTH);
        m_size.height = m_cpt.get(CV_CAP_PROP_FRAME_HEIGHT);

        if(delay)
            m_loops = 10;
        else
            m_loops = 1;
    }

    Cameras::~Cameras()
    {}
            
    bool Cameras::loaded() const
    {
        return m_cpt.isOpened();
    }

    bool Cameras::queryFrames()
    {
        /* Clear the buffered images and store the last one */
        for(int i = 0; i < m_loops; ++i) {
            if(!m_cpt.grab())
                return false;
        }

        if(!m_cpt.retrieve(m_img))
            return false;
        return true;
    }

    cv::Mat Cameras::get() const
    {
        return m_img;
    }

    cv::Size Cameras::size() const
    {
        return m_size;
    }

}



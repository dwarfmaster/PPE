
#ifndef DEF_FOLLOWER
#define DEF_FOLLOWER

#include <opencv2/opencv.hpp>

class Follower
{
    public:
        Follower();
        ~Follower();

        void process(const cv::Mat& img);
        const char* status() const;
        enum Status{Left, Right, Center, Unknown};
        Status pos() const;

    private:
        Status m_st;
};

#endif



#ifndef DEF_ABOD
#define DEF_ABOD

#include <opencv2/opencv.hpp>
#include <string>

class Abod
{
    public:
        Abod();
        ~Abod();

        void addGround(const cv::Mat& pict);
        bool save(const std::string& path);
        bool load(const std::string& path);

        void compute(const cv::Mat& pict, bool save = false);
        /* Get the distance of the obstacle in column in range |[-2;2]| */
        float operator[](int i) const;

    private:
        cv::MatND m_shist;
        cv::MatND m_hhist;
        float m_sthresh;
        float m_hthresh;
        float m_dists[5];

        void computeDists(const cv::Mat& bw);
};

#endif



#include "abod.hpp"
#include <iostream>
#include <sstream>
using namespace cv;

Abod::Abod()
{
    for(unsigned int i = 0; i < 5; ++i)
        m_dists[i] = 0.0f;
}

Abod::~Abod()
{
}

void Abod::addGround(const cv::Mat& pict)
{
    /* Preparing picture */
    const int plotHeight = pict.size().height;
    const int plotWidth  = pict.size().width;
    cv::Mat used;
    GaussianBlur(pict, used, Size(5,5), 1.8);
    cvtColor(used, used, CV_BGR2HSV);

    /* Create mask */
    Mat mask(pict.size(), CV_8U);
    mask = Scalar(0);
    Point poly[] = { Point(20,plotHeight), Point(60,plotHeight/2), Point(plotWidth - 60,plotHeight/2), Point(plotWidth - 20,plotHeight) };
    const Point* polys[] = { poly };
    int npts[] = { 4 };
    fillPoly(mask, polys, npts, 1, Scalar(255, 255, 255));

    /* Compute hue histogram */
    int histSize[] = { 180 };
    float hranges[] = { 0, 180 };
    const float* ranges[] = { hranges };
    Mat hhist;
    int channels[] = { 0 };
    calcHist(&used, 1, channels, mask, hhist, 1, histSize, ranges, true, false);

    /* Compute saturation histogram */
    histSize[0] = 255;
    hranges[2] = 255;
    Mat shist;
    channels[0] = 1;
    calcHist(&used, 1, channels, mask, shist, 1, histSize, ranges, true, false);

    /* Debug plot */
    Mat splitted[3];
    split(used, splitted);

    Point hpt(0,0);
    Point vpt(0,0);
    const float scale = .05f;
    for(int i = 0; i < 255; ++i) {
        Point npt(i*3, plotHeight - hhist.at<float>(i) * scale);
        if(i < 180) {
            line(used, hpt, npt, Scalar(255,0,0), 3);
            hpt = npt;
        }
        npt.y = plotHeight - shist.at<float>(i) * scale;
        line(used, vpt, npt, Scalar(0,255,0), 3);
        vpt = npt;
    }

    /* Store them. */
    if(m_shist.empty())
        m_shist = shist;
    else {
        for(int i = 0; i < 255; ++i)
            m_shist.at<float>(i) = m_shist.at<float>(i) + shist.at<float>(i);
    }

    if(m_hhist.empty())
        m_hhist = hhist;
    else {
        for(int i = 0; i < 180; ++i)
            m_hhist.at<float>(i) = m_hhist.at<float>(i) + hhist.at<float>(i);
    }
}

bool Abod::save(const std::string& path)
{
    Mat ssorted;
    Mat hsorted;

    /* Blur */
    Mat temp = m_shist.clone();
    m_shist.at<float>(0)   = floor(0.5f * m_shist.at<float>(0) + 0.25f * m_shist.at<float>(1));
    m_shist.at<float>(254) = floor(0.5f * m_shist.at<float>(254) + 0.25f * m_shist.at<float>(253));
    for(unsigned int i = 1; i < 254; ++i) {
        m_shist.at<float>(i) = 0.25f * temp.at<float>(i-1)
            + 0.25f * temp.at<float>(i+1)
            + 0.5f  * temp.at<float>(i);
        m_shist.at<float>(i) = floor(m_shist.at<float>(i));
    }

    temp = m_hhist.clone();
    m_hhist.at<float>(0)   = floor(0.5f * m_hhist.at<float>(0) + 0.25f * m_hhist.at<float>(1));
    m_hhist.at<float>(179) = floor(0.5f * m_hhist.at<float>(179) + 0.25f * m_hhist.at<float>(178));
    for(unsigned int i = 1; i < 179; ++i) {
        m_hhist.at<float>(i) = 0.25f * temp.at<float>(i-1)
            + 0.25f * temp.at<float>(i+1)
            + 0.5f  * temp.at<float>(i);
        m_hhist.at<float>(i) = floor(m_hhist.at<float>(i));
    }

    /* Median */
    sort(m_shist, ssorted, CV_SORT_EVERY_COLUMN | CV_SORT_ASCENDING);
    sort(m_hhist, hsorted, CV_SORT_EVERY_COLUMN | CV_SORT_ASCENDING);

    m_sthresh = m_hthresh = 0.0f;
    int i = 255/2;
    while(i < 255 && m_sthresh == 0) {
        m_sthresh = ssorted.at<float>(i);
        ++i;
    }
    i = 90;
    while(i < 180 && m_hthresh == 0) {
        m_hthresh = hsorted.at<float>(i);
        ++i;
    }

    /* Save */
    FileStorage fs(path, FileStorage::WRITE);
    fs << "sat" << m_shist;
    fs << "hue" << m_hhist;
    fs << "sth" << m_sthresh;
    fs << "hth" << m_hthresh;
    fs.release();
    return true;
}

bool Abod::load(const std::string& path)
{
    FileStorage fs(path, FileStorage::READ);
    if(!fs.isOpened())
        return false;
    if(fs["sat"].isNone() || fs["hue"].isNone()
            || fs["sth"].isNone() || fs["hth"].isNone())
        return false;

    fs["sat"] >> m_shist;
    fs["hue"] >> m_hhist;
    fs["sth"] >> m_sthresh;
    fs["hth"] >> m_hthresh;
    fs.release();
    return true;
}

void Abod::compute(const cv::Mat& pict, bool sav)
{
    cv::Mat hsv;
    GaussianBlur(pict, hsv, Size(5,5), 1.8);
    cvtColor(hsv, hsv, CV_BGR2HSV);
    cv::Mat result;
    cvtColor(pict, result, CV_BGR2GRAY);

    /* Computing saturation histogram. */
    int histSize[] = { 180 };
    float hranges[] = { 0, 180 };
    const float* ranges[] = { hranges };
    Mat hhist;
    int channels[] = { 0 };
    calcHist(&hsv, 1, channels, Mat(), hhist, 1, histSize, ranges, true, false);

    /* Compute saturation histogram */
    histSize[0] = 255;
    hranges[2] = 255;
    Mat shist;
    channels[0] = 2;
    calcHist(&hsv, 1, channels, Mat(), shist, 1, histSize, ranges, true, false);

    for(int i = 0; i < pict.rows; ++i) {
        for(int j = 0; j < pict.cols; ++j) {
            auto vec = hsv.at<Vec<unsigned char,3>>(i,j);
            int hue = (float)vec[0];
            int sat = (float)vec[1];

            if(m_shist.at<float>(sat) < m_sthresh
                    || m_hhist.at<float>(hue) < m_hthresh)
                result.at<unsigned char>(i,j) = 0;
            else
                result.at<unsigned char>(i,j) = 255;
        }
    }
    computeDists(result);

    if(sav) {
        static unsigned int nb = 0;
        std::ostringstream path;
        path << "results/pict" << nb << ".png";
        imwrite(path.str(), result);
        ++nb;
    }
}

float Abod::operator[](int i) const
{
    if(i >= -2 && i <= 2)
        return m_dists[i+2];
    else
        return 0.0f;
}

void Abod::computeDists(const cv::Mat& bw)
{
    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;
    cv::Mat mask(bw.size(), CV_8UC1);
    cv::Mat used;
    int off = bw.size().width / 5;
    int hgh = bw.size().height;

    cv::Mat drawn;
    cvtColor(bw, drawn, CV_GRAY2BGR);
    rectangle(drawn, Point(0,0), Point(bw.size().width, hgh), Scalar(0, 0, 0),   -1);

    for(int i = 0; i < 5; ++i) {
        /* Create mask */
        rectangle(mask, Point(0,0), Point(bw.size().width, hgh), Scalar(0, 0, 0),   -1);
        rectangle(mask, Point(i*off,0), Point((i+1)*off, hgh), Scalar(255,255,255), -1);
        bw.copyTo(used, mask);
        rectangle(used, Point(0,0), Point(i*off, hgh), Scalar(255,255,255),   -1);
        rectangle(used, Point((i+1)*off,0), Point(bw.size().width, hgh), Scalar(255,255,255),   -1);
        bitwise_not(used, used);

        /* Compute contours */
        findContours(used, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
        float min = hgh;
        for(unsigned int j = 0; j < contours.size(); ++j) {
            /* Ignoring too little areas. */
            if(contourArea(contours[j]) > 100) {
                Rect r = boundingRect(contours[j]);
                min = std::min(min, float(hgh - (r.y + r.height)));
                drawContours(drawn, contours, j, Scalar(255,0,0), 2, 8, hierarchy, 0, Point());
            }
            else
                drawContours(drawn, contours, j, Scalar(0,255,0), 2, 8, hierarchy, 0, Point());
        }
        contours.clear();
        m_dists[i] = min;
    }
}


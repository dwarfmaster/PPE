#include "follower.hpp"
using namespace cv;

const unsigned char minHue = 0;
const unsigned char maxHue = 255;
const unsigned char minSat = 0;
const unsigned char maxSat = 255;
const unsigned char minVal = 0;
const unsigned char maxVal = 255;

Follower::Follower()
    : m_st(Unknown)
{}

Follower::~Follower()
{}

void Follower::process(const cv::Mat& img)
{
    Mat binary(img.size(), CV_8UC1);
    /* Creating the binary picture. */
    for(int i = 0; i < img.rows; ++i) {
        for(int j = 0; j < img.cols; ++j) {
            auto vec = img.at<Vec<unsigned char,3>>(i,j);
            if(vec[0] >= minHue && vec[0] <= maxHue
                    && vec[1] >= minSat && vec[1] <= maxSat
                    && vec[2] >= minVal && vec[2] <= minVal)
                binary.at<unsigned char>(i,j) = 255;
            else
                binary.at<unsigned char>(i,j) = 0;
        }
    }
    imshow("Binary", binary);

    /* Getting contours. */
    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;
    findContours(binary, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point(0,0));

    /* Find center of biggest. */
    double maxArea = 0.0f;
    Point center;
    for(std::vector<Point> ctr : contours) {
        double area = contourArea(ctr);
        if(area > maxArea) {
            maxArea = area;
            Rect r = boundingRect(ctr);
            center.x = r.x + r.width / 2;
            center.y = r.y + r.height / 2;
        }
    }

    /* Set status depending on center. */
    if(maxArea < 100)
        m_st = Unknown;
    else if(center.x < img.size().width / 3)
        m_st = Left;
    else if(center.x < 2*img.size().width / 3)
        m_st = Center;
    else
        m_st = Right;
}

const char* Follower::status() const
{
    switch(m_st) {
        case Left:    return "left";
        case Right:   return "right";
        case Center:  return "center";
        case Unknown: return "unknown";
    }
    return "error";
}

Follower::Status Follower::pos() const
{
    return m_st;
}


#include "touchRecognizer.hpp"

#include <algorithm>
#include <vector>

void TouchRecognizer::calibrate(const cv::Mat& background)
{
    m_background = background.clone();
}

std::vector<cv::RotatedRect> TouchRecognizer::recognize(const cv::Mat& depthFrame) const
{
    std::vector<cv::RotatedRect> positions = std::vector<cv::RotatedRect>();

    cv::Mat touch = (m_background - depthFrame);
    // threshold to zero
    cv::threshold(touch, touch, 10, 255, cv::THRESH_TOZERO);
    cv::threshold(touch, touch, 60, 255, cv::THRESH_TOZERO_INV);

    // blur to reduce noise
    cv::blur(touch, touch, cv::Size(21, 21));

    // threshold to binary
    cv::threshold(touch, touch, 10, 1, cv::THRESH_BINARY);
    touch.convertTo(touch, CV_8U);

    // find contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(touch, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
 
    // find bounding boxes
    for (const auto& contour : contours)
    {
        positions.push_back(cv::minAreaRect(contour));
    }

	cv::imshow("debug",  touch * 100);

    return positions;
}

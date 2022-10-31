#include "touchVisualizer.hpp"

void TouchVisualizer::draw(cv::Mat & frame, const cv::RotatedRect & position, cv::Mat & output)
{
    output = frame.clone();
    cv::circle(output, position.center, 9, cv::Scalar(0, 0, 255), 2);
}

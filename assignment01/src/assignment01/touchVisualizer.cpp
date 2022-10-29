#include "touchVisualizer.hpp"

void TouchVisualizer::draw(cv::Mat & frame, const cv::RotatedRect & position, cv::Mat & output)
{
    output = frame.clone();

    cv::Point2f vertices[4];
    position.points(vertices);
    for (int i = 0; i < 4; i++)
    {
        cv::line(output, vertices[i], vertices[(i + 1) % 4], cv::Scalar(0, 255, 0), 2);
    }

    cv::circle(output, position.center, 5, cv::Scalar(0, 0, 255), -1);
}

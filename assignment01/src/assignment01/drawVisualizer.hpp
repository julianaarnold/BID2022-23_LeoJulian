#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

class DrawVisualizer
{
    public:
        static void draw(cv::Mat & frame, std::vector<cv::RotatedRect> & positions, std::vector<int> & frameIndices, cv::Mat & output);
};
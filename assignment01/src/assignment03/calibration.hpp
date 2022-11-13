#pragma once

#include "fullscreenWindow.hpp"

#include <vector>
#include <string>

#include <opencv2/opencv.hpp>

class HomograpyCalibrator : public FullscreenWindow
{
public:
    HomograpyCalibrator();
    void toggleFullscreen() override;
    void draw(const cv::Mat & colorFrame);
    bool calibrationComplete() const;
    void computeHomography();
    cv::Point2f cameraToGame(const cv::Point2f& point) const;
    void gameToProjector(const cv::Mat& input, cv::Mat& output) const;
private:
    std::string windowName() override;
    void setMouseCallback(cv::MouseCallback callback);
    static void mouseCallback(int event, int x, int y, int flags, void *userdata);
    std::vector<cv::Point2f> projectorCorners();
    std::vector<cv::Point2f> cameraCorners();

    const std::string c_windowName = "calibration";
    const std::string c_colors[4] = {"black", "blue", "green", "red"};
    const std::string c_spaces[2] = {"physical", "projected"};

    std::vector<cv::Point2i> m_corners;
    bool m_cornersComplete = false;

    cv::Mat m_cameraToGame;
    cv::Mat m_gameToProjector;
    cv::Mat m_testImage;
};

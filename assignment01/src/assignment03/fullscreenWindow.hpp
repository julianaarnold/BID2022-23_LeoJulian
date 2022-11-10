#pragma once

#include <string>
#include <opencv2/opencv.hpp>

class FullscreenWindow
{
public:
    virtual ~FullscreenWindow() = default;
    void initialize();
    virtual void toggleFullscreen();
    void drawText(int x, int y, const std::string& text);
protected:
    virtual std::string windowName() = 0;
    const static int c_lineHeight = 20;
    const std::string c_windowName;
    const cv::Size2i c_initialSize = cv::Size2i(400, 100);
    cv::Mat m_canvas;
    bool m_isFullscreen = false;
};

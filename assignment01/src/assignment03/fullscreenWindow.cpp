#include "fullscreenWindow.hpp"

void FullscreenWindow::initialize()
{
    cv::namedWindow(windowName(), CV_WINDOW_AUTOSIZE);
    m_canvas = cv::Mat(c_initialSize, CV_8UC3);
}

void FullscreenWindow::toggleFullscreen()
{
    m_isFullscreen = !m_isFullscreen;

    cv::destroyWindow(windowName());
    cv::namedWindow(windowName(), m_isFullscreen ? CV_WINDOW_NORMAL : CV_WINDOW_AUTOSIZE);

    auto size = c_initialSize;
    if(m_isFullscreen)
    {
        cv::setWindowProperty(windowName(), CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
        size = cv::getWindowImageRect(windowName()).size();
    }

    m_canvas = cv::Mat(size, CV_8UC3);
}

void FullscreenWindow::drawText(const int x, const int y, const std::string& text)
{
    cv::putText(m_canvas, text, cv::Point2i(x, y), cv::HersheyFonts::FONT_HERSHEY_SIMPLEX, cv::getFontScaleFromHeight(cv::HersheyFonts::FONT_HERSHEY_SIMPLEX, c_lineHeight, 2),  cv::Scalar::all(255), 2);
}

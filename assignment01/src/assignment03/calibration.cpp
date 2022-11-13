#include "calibration.hpp"
#include "game.hpp"
#include "../framework/kinect.hpp"

HomograpyCalibrator::HomograpyCalibrator()
{
    initialize();
}

void HomograpyCalibrator::toggleFullscreen()
{
    FullscreenWindow::toggleFullscreen();
    cv::setMouseCallback(c_windowName, m_isFullscreen ? mouseCallback : nullptr, this);
}

void HomograpyCalibrator::draw(const cv::Mat& colorFrame)
{
    m_canvas = cv::Mat::zeros(m_canvas.rows, m_canvas.cols, m_canvas.type());

    if (m_isFullscreen && m_corners.size() > 3)
    {
        cv::resize(colorFrame, m_canvas, m_canvas.size());
    }

    const auto count = m_corners.size();

    if(m_isFullscreen)
    {
        drawText(2, 2 + c_lineHeight, "Click the " + c_colors[count % 4] + " " + c_spaces[count / 4] + " circle.");
    } 
    else
	{
	    drawText(2, 2 + c_lineHeight, "Move to correct screen,");
	    drawText(2, 2 + 2 * c_lineHeight + 5, "press F for fullscreen.");
	}

    cv::imshow(c_windowName, m_canvas);
}

bool HomograpyCalibrator::calibrationComplete() const
{
    return m_cornersComplete;
}

void HomograpyCalibrator::computeHomography()
{
    // scaling???
    float scaleFactor = 1 / (m_canvas.rows / m_canvas.cols);
    std::vector<cv::Point2f> scaledCameraCorners();
    for (auto &corner: cameraCorners()) {
        scaledCameraCorners.push_back(scaleFactor * corner);
    }

    std::vector<cv::Point2f> scaledProjectorCorners();
    for (auto &corner: projectorCorners()) {
        scaledProjectorCorners.push_back(scaleFactor * corner);
    }

    m_cameraToGame = cv::getPerspectiveTransform(scaledCameraCorners(), Game::gameCorners());
    m_gameToProjector = cv::getPerspectiveTransform(gameCorners(), scaledProjectorCorners());
    // You should assign something to m_cameraToGame and m_gameToProjector.
    // For this, you can use cameraCorners(), projectorCorners() and Game::gameCorners().
    // Tip: The camera and projector corners are given in window resolution (probably something like 800x600, check this using m_canvas.cols/rows),
    // which is different from the usual camera coordinates. Maybe you should apply some scaling before building the matrices...
}

cv::Point2f HomograpyCalibrator::cameraToGame(const cv::Point2f& point) const
{
    cv::Mat <float> pointMatrix(3,1);
    pointMatrix(0,0) = point.x; 
    pointMatrix(1,0) = point.y; 
    pointMatrix(2,0) = 1.0;
    // inverse???
    cv::Point2f transformedPoint = m_cameraToGame * pointMatrix;

    return transformedPoint;
    // Use m_cameraToGame to transform the point.
    // Tip: You can only multiply matrices with each other in OpenCV, so you'll have to build one (or build a cv::Vec3 and convert that).
    // Also, matrix multiplication isn't commutative.
}

void HomograpyCalibrator::gameToProjector(const cv::Mat& input, cv::Mat& output) const
{
    cv::warpPerspective (InputArray src, OutputArray dst, InputArray M, Size dsize, int flags=INTER_LINEAR, int borderMode=BORDER_CONSTANT, const Scalar &borderValue=Scalar())

    // Use m_gameToProjector to transform the image.
}

std::string HomograpyCalibrator::windowName()
{
    return c_windowName;
}

void HomograpyCalibrator::setMouseCallback(const cv::MouseCallback callback)
{
    cv::setMouseCallback(c_windowName, callback, this);
}

void HomograpyCalibrator::mouseCallback(const int event, int x, int y, const int flags, void* userdata)
{
    auto calibrator = static_cast<HomograpyCalibrator*>(userdata);

    if (event == cv::EVENT_LBUTTONDOWN)
    {
        calibrator->m_corners.emplace_back(x, y);
        if (calibrator->m_corners.size() >= 8)
        {
            calibrator->m_cornersComplete = true;
            cv::destroyWindow(calibrator->c_windowName);
            calibrator->computeHomography();
        }
    }
    else if (event == cv::EVENT_RBUTTONDOWN)
    {
        if (!calibrator->m_corners.empty())
        {
            calibrator->m_corners.pop_back();
        }
    }
}

std::vector<cv::Point2f> HomograpyCalibrator::projectorCorners()
{
    return std::vector<cv::Point2f>(m_corners.begin(), m_corners.begin() + 4);
}

std::vector<cv::Point2f> HomograpyCalibrator::cameraCorners()
{
    return std::vector<cv::Point2f>(m_corners.begin() + 4, m_corners.begin() + 8);
}

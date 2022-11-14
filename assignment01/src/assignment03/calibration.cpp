#include "calibration.hpp"
#include "game.hpp"
#include "../framework/kinect.hpp"


#define TEST_IMAGE_PATH "D:\\Testcard_G_PM5544_KSL.JPG"

HomograpyCalibrator::HomograpyCalibrator()
{
    initialize();
    m_testImage = cv::imread(TEST_IMAGE_PATH);
}

void HomograpyCalibrator::toggleFullscreen()
{
    FullscreenWindow::toggleFullscreen();
    cv::setMouseCallback(c_windowName, m_isFullscreen ? mouseCallback : nullptr, this);
}

void HomograpyCalibrator::draw(const cv::Mat& colorFrame)
{
    m_canvas = cv::Mat::zeros(m_canvas.rows, m_canvas.cols, m_canvas.type());
    m_cameraHeight = colorFrame.rows;
    m_cameraWidth = colorFrame.cols;


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
    float gameHeight = (float) Game::gameCanvasSize().height;
    float gameWidth = (float) Game::gameCanvasSize().width;

    std::vector<cv::Point2f> scaledCameraCorners = std::vector<cv::Point2f>();
    for (auto &corner: cameraCorners()) {
        scaledCameraCorners.push_back(
            cv::Point2f(corner.x * m_cameraWidth / m_canvas.cols, corner.y * m_cameraHeight / m_canvas.rows)
        );
    }

    std::vector<cv::Point2f> scaledProjectorCorners = std::vector<cv::Point2f>();
    for (auto &corner: projectorCorners()) {
        scaledProjectorCorners.push_back(
            cv::Point2f(corner.x * gameWidth / m_canvas.cols, corner.y * gameHeight / m_canvas.rows)
        );
    }

    m_cameraToGame = cv::getPerspectiveTransform(scaledCameraCorners, Game::gameCorners());
    m_gameToProjector = cv::getPerspectiveTransform(Game::gameCorners(), scaledProjectorCorners);
    
    // You should assign something to m_cameraToGame and m_gameToProjector.
    // For this, you can use cameraCorners(), projectorCorners() and Game::gameCorners().
    // Tip: The camera and projector corners are given in window resolution (probably something like 800x600, check this using m_canvas.cols/rows),
    // which is different from the usual camera coordinates. Maybe you should apply some scaling before building the matrices...
}

cv::Point2f HomograpyCalibrator::cameraToGame(const cv::Point2f& point) const
{
    cv::Mat_<double>pointMatrix(3,1);
    pointMatrix(0,0) = point.x; 
    pointMatrix(1,0) = point.y; 
    pointMatrix(2,0) = 1.0;
    // inverse???
    cv::Mat_<double> transformedMatrix = m_cameraToGame * pointMatrix;
    
    return cv::Point2f(transformedMatrix(0,0), transformedMatrix(1,0));
    // Use m_cameraToGame to transform the point.
    // Tip: You can only multiply matrices with each other in OpenCV, so you'll have to build one (or build a cv::Vec3 and convert that).
    // Also, matrix multiplication isn't commutative.
}

void HomograpyCalibrator::gameToProjector(const cv::Mat& input, cv::Mat& output) const
{
    //cv::Mat newImage;
    //cv::resize(m_testImage, newImage, input.size(), cv::INTER_LINEAR);
    cv::warpPerspective(input, output, m_gameToProjector, input.size());

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

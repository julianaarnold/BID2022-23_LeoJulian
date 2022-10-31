#include "../framework/kinect.hpp"
#include "touchRecognizer.hpp"
#include "touchVisualizer.hpp"
#include "drawVisualizer.hpp" // only for bonus assignment

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

int main()
{
    // initialize framework
    Kinect::initialize();
    Kinect::listDevices();

    // set up device
    Kinect kinect;
    if(!kinect.ready())
    {
        std::cout << "Could not set up Kinect. Exiting." << std::endl;
        return -1;
    }

    // prepare touch pad
    TouchRecognizer touchRecognizer = TouchRecognizer();

    // prepare variables
    cv::Mat depthFrame;
    cv::Mat colorFrame;
    cv::Mat touchFrame;
    cv::Mat drawFrame;
    auto running = true;

    kinect.getDepthFrame(depthFrame);
    kinect.getColorFrame(colorFrame);

    touchRecognizer.calibrate(depthFrame);

    // prepare windows - this isn't necessary, but it allows to assign useful positions
    cv::namedWindow("color");
    cv::namedWindow("depth");
    cv::namedWindow("touch");
    cv::namedWindow("debug");
    cv::namedWindow("draw");

    // move windows
    const auto xOffset = colorFrame.cols;
    const auto yOffset = colorFrame.rows;
    cv::moveWindow("color", 0 * xOffset, 0 * yOffset);
    cv::moveWindow("depth", 1 * xOffset, 0 * yOffset);
    cv::moveWindow("touch", 0 * xOffset, 1 * yOffset);
    cv::moveWindow("draw", 1 * xOffset, 1 * yOffset);
    cv::moveWindow("debug", 2 * xOffset, 1 * yOffset);
    /*~~~~~~~~~~~~~~~*
     * DEBUG WINDOWS *
     * COULD GO HERE *
     *~~~~~~~~~~~~~~~*/

    std::vector<cv::RotatedRect> drawnPositions = std::vector<cv::RotatedRect>();
    std::vector<int> positionsFrameIndex = std::vector<int>();

    int currentFrameIndex = 0;
    while(running)
    {
        // update frames
        kinect.getDepthFrame(depthFrame);
        kinect.getColorFrame(colorFrame);
        colorFrame.copyTo(touchFrame);
        colorFrame.copyTo(drawFrame);

        // run touch recognizer
        
        std::vector<cv::RotatedRect> currentPositions = touchRecognizer.recognize(depthFrame);
        
        for (const auto& position : currentPositions)
        {
            drawnPositions.push_back(position);
            positionsFrameIndex.push_back(currentFrameIndex);
        }

        /*
        for (const auto& position : drawnPositions)
        {
            printf("x: %f, y: %f, w: %f, h: %f, a: %f", position.center.x, position.center.y, position.size.width, position.size.height, position.angle);
        }*/

        //printf("size: %d\n", drawnPositions.size());
        

        // run visualizer - there may be no position
        for (const auto& position : drawnPositions)
        {
            TouchVisualizer::draw(touchFrame, position, touchFrame);
        }

        DrawVisualizer::draw(drawFrame, drawnPositions, positionsFrameIndex, drawFrame);

        // show frames
        auto depthFrameUnscaled = depthFrame.clone();
        depthFrame = 20 * depthFrame;
        cv::imshow("depth", depthFrame);
        cv::imshow("color", colorFrame);
		cv::imshow("touch", touchFrame);
		cv::imshow("draw", drawFrame);

        currentFrameIndex++;

        // check for keypresses
        const auto key = cv::waitKey(10);
        switch (key)
        {
		case 0x1b: // esc - exit
            running = false;
            break;
		case 0x0d: // enter - calibrate again
            kinect.getDepthFrame(depthFrame);
            touchRecognizer.calibrate(depthFrame);
            drawnPositions.clear();
            positionsFrameIndex.clear();
            break;
        default:
            break;
        }
    }
}

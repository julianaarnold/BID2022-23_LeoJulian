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
    auto running = true;

    kinect.getDepthFrame(depthFrame);
    kinect.getColorFrame(colorFrame);

    touchRecognizer.calibrate(depthFrame);

    // prepare windows - this isn't necessary, but it allows to assign useful positions
    cv::namedWindow("color");
    cv::namedWindow("depth");

    
    cv::namedWindow("debug");
    /*~~~~~~~~~~~~~~~*
     * DEBUG WINDOWS *
     * COULD GO HERE *
     *~~~~~~~~~~~~~~~*/

    // move windows
    const auto xOffset = colorFrame.cols;
    const auto yOffset = colorFrame.rows;
    cv::moveWindow("color", 0 * xOffset, 0 * yOffset);
    cv::moveWindow("depth", 1 * xOffset, 0 * yOffset);
    /*~~~~~~~~~~~~~~~*
     * DEBUG WINDOWS *
     * COULD GO HERE *
     *~~~~~~~~~~~~~~~*/

    while(running)
    {
        // update frames
        kinect.getDepthFrame(depthFrame);
        kinect.getColorFrame(colorFrame);
        colorFrame.copyTo(touchFrame);

        // run touch recognizer
        
        std::vector<cv::RotatedRect> positions = touchRecognizer.recognize(depthFrame);

        // run visualizer - there may be no position
        
        
        for (const auto& position : positions)
        {
            TouchVisualizer::draw(touchFrame, position, touchFrame);
        }

        // show frames
        auto depthFrameUnscaled = depthFrame.clone();
        depthFrame = 20 * depthFrame;
        cv::imshow("depth", depthFrame);
        cv::imshow("color", colorFrame);
		cv::imshow("touch", touchFrame);

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
            break;
        default:
            break;
        }
    }
}

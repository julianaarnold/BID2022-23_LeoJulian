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
    TouchRecognizer touchRecognizer;

    // prepare variables
    cv::Mat depthFrame;
    cv::Mat colorFrame;
    cv::Mat touchFrame;
    auto running = true;

    // calibrate
    
    /*~~~~~~~~~~~*
     * YOUR CODE *
     * GOES HERE *
     *~~~~~~~~~~~*/

    // prepare windows - this isn't necessary, but it allows to assign useful positions
    cv::namedWindow("color");
    cv::namedWindow("depth");
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
        
        /*~~~~~~~~~~~*
         * YOUR CODE *
         * GOES HERE *
         *~~~~~~~~~~~*/

        // run visualizer - there may be no position
        
        /*~~~~~~~~~~~*
         * YOUR CODE *
         * GOES HERE *
         *~~~~~~~~~~~*/

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
            /*~~~~~~~~~~~*
             * YOUR CODE *
             * GOES HERE *
             *~~~~~~~~~~~*/
            break;
        default:
            break;
        }
    }
}

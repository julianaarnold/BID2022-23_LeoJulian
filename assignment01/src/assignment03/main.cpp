#include "../framework/kinect.hpp"
#include "../assignment01/touchRecognizer.hpp"
#include "calibration.hpp"
#include "game.hpp"

#include <opencv2/opencv.hpp>
#include "multitouchGameControl.hpp"

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
    cv::Mat outputFrame;
    auto running = true;

    // homography stuff
    HomograpyCalibrator calibrator;
    Game game(calibrator);
    MultitouchGameControl gameControl(game, calibrator);

    // calibrate
    kinect.getDepthFrame(depthFrame);
    touchRecognizer.calibrate(depthFrame);

    // prepare windows - this isn't necessary, but it allows to assign useful positions
    cv::namedWindow("color");
    cv::namedWindow("depth");

    // move windows
    kinect.getColorFrame(colorFrame);
    const auto xOffset = colorFrame.cols;
    const auto yOffset = colorFrame.rows;
    cv::moveWindow("color", 0 * xOffset, 0 * yOffset);
    cv::moveWindow("depth", 1 * xOffset, 0 * yOffset);

    while(running)
    {
        // update frames
        kinect.getDepthFrame(depthFrame);
        kinect.getColorFrame(colorFrame);

        // run touch recognizer
        auto positions = touchRecognizer.recognize(depthFrame);

        // calibrate homography matrix
        if(!calibrator.calibrationComplete())
        {
            calibrator.draw(colorFrame);
        }
        // update and draw game
		else
		{
		    if(!game.initialized())
		    {
		        game.initialize();
		    }

            if(!positions.empty())
            {
                gameControl.handleInput(positions);
            }

            game.update();
            game.draw(colorFrame);
            game.drawTouches(positions, 3);
            game.show();
		}

        // show frames
        auto depthFrameUnscaled = depthFrame.clone();
        depthFrame = 20 * depthFrame;
        cv::imshow("depth", depthFrame);
        cv::imshow("color", colorFrame);

        // check for keypresses
        const auto key = cv::waitKey(1);
        switch (key)
        {
		case 0x1b: // esc
            running = false;
            break;
		case 0x0d: // enter
            touchRecognizer.calibrate(depthFrameUnscaled);
            break;
		case 0x20: // space
            game.start();
            break;
		case 0x72: // r
            game.reset();
            break;
		case 0x66: // f
            if(!calibrator.calibrationComplete())
            {
                calibrator.toggleFullscreen();
            }
			else
			{
			    game.toggleFullscreen();
			}
            break;
        default:
            break;
        }
    }
}

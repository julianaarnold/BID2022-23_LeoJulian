#include "../framework/kinect.hpp"
#include "../assignment01/touchRecognizer.hpp"
#include "classifier.hpp"
#include "digitVisualizer.hpp"

#include <opencv2/opencv.hpp>
#include "data.hpp"

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

    // load data set
    const auto data = DataSet::read("pendigits.tra");

    // prepare variables
    cv::Mat depthFrame;
    cv::Mat colorFrame;
    cv::Mat outputFrame;
    auto running = true;
    std::vector<cv::Point2f> path;

    // calibrate
    kinect.getDepthFrame(depthFrame);
    touchRecognizer.calibrate(depthFrame);

    // prepare classifier and viewer
    Classifier classifier(data, 25);
    DigitVisualizer digitVisualizer;

    // prepare windows - this isn't necessary, but it allows to assign useful positions
    cv::namedWindow("color");
    cv::namedWindow("depth");
    
    cv::namedWindow("normalized");
    cv::namedWindow("output");

    // move windows
    kinect.getColorFrame(colorFrame);
    const auto xOffset = colorFrame.cols;
    const auto yOffset = colorFrame.rows;
    cv::moveWindow("color", 0 * xOffset, 0 * yOffset);
    cv::moveWindow("depth", 1 * xOffset, 0 * yOffset);
    cv::moveWindow("output", 0 * xOffset, 1 * yOffset);
    cv::moveWindow("normalized", 1 * xOffset, 1 * yOffset);

    while(running)
    {
        // update frames
        kinect.getDepthFrame(depthFrame);
        kinect.getColorFrame(colorFrame);

        // run touch recognizer
        auto positions = touchRecognizer.recognize(depthFrame);
        if(!positions.empty())
		{
            // track user's drawing using the path variable
            path.push_back(positions[0].center);
            digitVisualizer.draw(colorFrame, path, outputFrame);
		} 
        else
		{
            // done drawing -> classify

            classifier.classify(path);
		    digitVisualizer.setResult(classifier.getResult());
            digitVisualizer.setSimplifiedPath(classifier.getSimplifiedPath());

            // visualize results
            digitVisualizer.draw(colorFrame, std::vector<cv::Point2f>(), outputFrame);

            // after classifying, you should update the visualizer like this:
		    // digitVisualizer.setSimplifiedPath(classifier.getSimplifiedPath());
		    // digitVisualizer.setResult(classifier.getResult());
		}

        // show frames
        auto depthFrameUnscaled = depthFrame.clone();
        depthFrame = 20 * depthFrame;
        cv::imshow("depth", depthFrame);
        cv::imshow("color", colorFrame);
        cv::imshow("output", outputFrame);

        // check for keypresses
        const auto key = cv::waitKey(10);
        switch (key)
        {
		case 0x1b: // esc
            running = false;
            break;
		case 0x0d: // enter
            touchRecognizer.calibrate(depthFrameUnscaled);
            path.clear();
            break;
		case 0x30: // number keys - you can use this to take a look at the data set
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
		case 0x38:
		case 0x39: 
            DataVisualizer::showNumber(data, key - 0x30);
            break;
        default:
            break;
        }
    }
}

#include "drawVisualizer.hpp"

void DrawVisualizer::draw(cv::Mat & frame, std::vector<cv::RotatedRect> & positions, std::vector<int> & frameIndices, cv::Mat & output) {
    output = frame.clone();

    if (positions.size() == 0) return;

    cv::RotatedRect lastPosition = positions[0];
    int lastFrameIndex = frameIndices[0];
    for (int i = 1; i < positions.size(); i++) {
        cv::RotatedRect currentPosition = positions[i];
        int currentFrameIndex = frameIndices[i];

        if (currentFrameIndex - lastFrameIndex < 3) {
            cv::line(output, lastPosition.center, currentPosition.center, cv::Scalar(0, 0, 255), 2);
        }

        lastPosition = currentPosition;
        lastFrameIndex = currentFrameIndex;
    }
}
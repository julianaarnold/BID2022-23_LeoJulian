#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include "game.hpp"

class MultitouchGameControl
{
public:
    MultitouchGameControl(Game& game, HomograpyCalibrator& calibrator);
    void handleInput(std::vector<cv::RotatedRect> positions);
protected:
    Game& m_game;
    HomograpyCalibrator& m_calibrator;
    /*~~~~~~~~~~~~~~~~*
     * YOU MAY DEFINE *
     * VARIABLES HERE *
     *~~~~~~~~~~~~~~~~*/
};

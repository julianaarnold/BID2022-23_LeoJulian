#include "multitouchGameControl.hpp"

MultitouchGameControl::MultitouchGameControl(Game& game, HomograpyCalibrator& calibrator) : m_game(game), m_calibrator(calibrator)
{
}

void MultitouchGameControl::handleInput(std::vector<cv::RotatedRect> positions)
{
    /*~~~~~~~~~~~*
     * YOUR CODE *
     * GOES HERE *
     *~~~~~~~~~~~*/

    // Use m_calibrator to access and modify the m_game's player's pedals.
    // How you do it is your call, but you should probably use active(), setActive(), position() and setTargetPosition().
}

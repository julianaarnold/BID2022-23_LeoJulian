#include "multitouchGameControl.hpp"
#include "game.hpp"

MultitouchGameControl::MultitouchGameControl(Game& game, HomograpyCalibrator& calibrator) : m_game(game), m_calibrator(calibrator)
{
}

void MultitouchGameControl::handleInput(std::vector<cv::RotatedRect> positions)
{
    // iterate over players
   

    for (auto &player : m_game.players()) {
        auto paddles = player.paddles();
        for (int i = 0; i < 2; i++) {
            auto * paddle = &paddles[i];

            std::vector<cv::Point2f> input = std::vector<cv::Point2f>();
            for (auto &position: positions) {
                cv::Point2f center = position.center;
                center = m_calibrator.cameraToGame(center);

                float diffX = center.x - (m_game.gameCanvasSize().width / 2);
                float diffY = center.y - (m_game.gameCanvasSize().height / 2);
                float playerDirection = diffX * paddle->direction();

                if ((playerDirection < 0) && (diffY * (2 * i - 1) < 0)) {
                    input.push_back(center);
                }
            }

            if (!input.empty()) {
                printf("set paddle %d to %f \n", i, input[0].y);
                paddle->setActive(true);
                printf("paddle is active %d \n", paddle->active());
                paddle->setTargetPosition(input[0].y);
            } else {
                //paddle->setActive(false);
            }
        }
    }
    /*~~~~~~~~~~~*
     * YOUR CODE *
     * GOES HERE *
     *~~~~~~~~~~~*/

    // Use m_calibrator to access and modify the m_game's player's pedals.
    // How you do it is your call, but you should probably use active(), setActive(), position() and setTargetPosition().
}

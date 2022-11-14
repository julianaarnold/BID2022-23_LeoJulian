#include "multitouchGameControl.hpp"
#include "game.hpp"

MultitouchGameControl::MultitouchGameControl(Game& game, HomograpyCalibrator& calibrator) : m_game(game), m_calibrator(calibrator)
{
}

void MultitouchGameControl::handleInput(std::vector<cv::RotatedRect> positions)
{
    for (auto i = 0; i < positions.size(); i++) {
        // decide which player this belongs to
        auto position = m_calibrator.cameraToGame(positions[i].center);
        auto playerIndex = static_cast<int>(position.x > Game::gameCanvasSize().width / 2.0);
        auto& player = m_game.players()[playerIndex];

        auto paddleIndex = static_cast<int>(position.y > Game::gameCanvasSize().height / 2.0);
        player.paddles()[paddleIndex].setTargetPosition(position.y);
    }
}

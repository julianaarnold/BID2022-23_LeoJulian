#include "game.hpp"
#include "calibration.hpp"

const cv::Rect2i Game::c_gameCanvasSize = cv::Rect2i(0, 0, 800, 600);

Game::Game(HomograpyCalibrator& calibrator) : m_calibrator(calibrator)
{
    reset();
}

bool Game::initialized() const
{
    return m_isFullscreen;
}

std::vector<cv::Point2f> Game::gameCorners()
{
    return std::vector<cv::Point2f>
    {
        cv::Point2f(
			c_gameCanvasSize.x, 
			c_gameCanvasSize.y),
        cv::Point2f(
			c_gameCanvasSize.x + c_gameCanvasSize.width, 
			c_gameCanvasSize.y),
        cv::Point2f(
			c_gameCanvasSize.x + c_gameCanvasSize.width, 
			c_gameCanvasSize.y + c_gameCanvasSize.height),
        cv::Point2f(
			c_gameCanvasSize.x, 
			c_gameCanvasSize.y + c_gameCanvasSize.height)
    };
}

cv::Rect2i Game::gameCanvasSize()
{
    return c_gameCanvasSize;
}

void Game::reset()
{
    m_players.clear();

	m_players.emplace_back(1);
	m_players.emplace_back(-1);

    m_balls.clear();

    m_balls.emplace_back(cv::Point2d(c_gameCanvasSize.size() / 2), cv::Point2d(1, 0) * Ball::ballMovementSpeed());
    m_balls.emplace_back(cv::Point2d(c_gameCanvasSize.size() / 2), cv::Point2d(-1, 0) * Ball::ballMovementSpeed());
}

void Game::start()
{
    if(m_running)
    {
        return;
    }

    m_running = true;
    m_lastUpdate = std::chrono::steady_clock::now();
}

void Game::update()
{
    if(!m_running)
    {
        return;
    }

    const auto now = std::chrono::steady_clock::now();
    const auto delta = now - m_lastUpdate;
    const auto deltaSeconds = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(delta).count()) / 1000.0;

    for (auto& player : m_players)
    {
        player.update(deltaSeconds);
    }

    std::vector<Paddle> paddles;
    for (auto& player : m_players)
    {
        auto playerPaddles = player.paddles();
        paddles.insert(paddles.end(), playerPaddles.begin(), playerPaddles.end());
    }

    for (auto& ball : m_balls)
    {
        ball.update(deltaSeconds, paddles);
        if(ball.position().x > c_gameCanvasSize.width)
        {
            m_players[0].touchdown();
            ball = Ball(cv::Point2d(c_gameCanvasSize.size() / 2), cv::Point2d(-1, 0) * Ball::ballMovementSpeed());
        }
		else if(ball.position().x < 0)
		{
		    m_players[1].touchdown();
            ball = Ball(cv::Point2d(c_gameCanvasSize.size() / 2), cv::Point2d(1, 0) * Ball::ballMovementSpeed());
		}
    }

    m_lastUpdate = now;
}

void Game::draw(const cv::Mat& colorFrame)
{
    m_gameCanvas = cv::Mat(c_gameCanvasSize.size(), CV_32FC3, cv::Scalar::all(255));

    const auto thickness = 10;
    const auto fontScale = cv::getFontScaleFromHeight(cv::HersheyFonts::FONT_HERSHEY_SIMPLEX, 100, thickness);
    const auto textSize = cv::getTextSize("0", cv::HersheyFonts::FONT_HERSHEY_SIMPLEX, fontScale, thickness, nullptr);
    cv::putText(m_gameCanvas, std::to_string(m_players[0].score()), cv::Point2i(100, 100 + textSize.height), cv::HersheyFonts::FONT_HERSHEY_SIMPLEX, fontScale, cv::Scalar(), thickness);
    cv::putText(m_gameCanvas, std::to_string(m_players[1].score()), cv::Point2i(c_gameCanvasSize.width - 100 - textSize.width, 100 + textSize.height), cv::HersheyFonts::FONT_HERSHEY_SIMPLEX, fontScale, cv::Scalar(), thickness);

    for (const auto& player : m_players)
    {
        player.draw(m_gameCanvas);
    }

    for (const auto& ball : m_balls)
    {
        ball.draw(m_gameCanvas);
    }

    if(!m_running)
    {
        const auto text = "Press SPACE to start.";
        const auto spaceTextSize = cv::getTextSize(text, cv::HersheyFonts::FONT_HERSHEY_SIMPLEX, fontScale / 2.0, thickness, nullptr);
        cv::putText(m_gameCanvas, text, cv::Point2i((c_gameCanvasSize.width - spaceTextSize.width) / 2.0, (c_gameCanvasSize.height + spaceTextSize.height) / 2.0), cv::HersheyFonts::FONT_HERSHEY_SIMPLEX, fontScale / 2.0, cv::Scalar(0, 0, 255), thickness);
    }
}

void Game::drawTouches(const std::vector<cv::RotatedRect>& touches, int count)
{
    if(count == -1 || count > touches.size())
    {
        count = touches.size();
    }
    for(auto i = 0; i < count; i++)
    {
        drawCircle(touches[i].center);
    }
}

void Game::show()
{
    m_canvas = cv::Mat::zeros(m_canvas.rows, m_canvas.cols, m_canvas.type());

    if (m_isFullscreen)
    {
        m_calibrator.gameToProjector(m_gameCanvas, m_canvas);
    }
    else
	{
	    drawText(2, 2 + c_lineHeight, "Move to correct screen,");
	    drawText(2, 2 + 2 * c_lineHeight + 5, "press F for fullscreen.");
	}

    cv::imshow(c_windowName, m_canvas);
}

void Game::drawCircle(const cv::Point2f& position)
{
    cv::circle(m_gameCanvas, m_calibrator.cameraToGame(position), 20, cv::Scalar(0, 0, 255), 5);
}

std::string Game::windowName()
{
    return c_windowName;
}

std::vector<Player>& Game::players()
{
    return m_players;
}

Player::Player(int direction)
{
    for(auto i = 0; i < c_paddleCount; ++i)
    {
        m_paddles.emplace_back(i, direction);
    }
}

int Player::score() const
{
    return m_score;
}

std::vector<Paddle>& Player::paddles()
{
    return m_paddles;
}

void Player::touchdown()
{
    m_score++;
}

void Player::update(const double deltaSeconds)
{
    for (auto& paddle : m_paddles)
    {
        paddle.update(deltaSeconds);
    }
}

void Player::draw(cv::Mat& canvas) const
{
    for (auto paddle : m_paddles)
    {
        paddle.draw(canvas);
    }
}

Paddle::Paddle(const int layer, const int direction) : m_layer(layer), m_direction(direction) { }

int Paddle::paddleWidth()
{
    return c_paddleWidth;
}

void Paddle::update(const double deltaSeconds)
{
    const auto movement = m_targetPosition - m_position;
    m_position += (std::signbit(movement) ? -1 : 1) * std::min(c_paddleMovementSpeed * deltaSeconds, std::abs(movement));
}

void Paddle::draw(cv::Mat& canvas) const
{
    const auto color = m_active ? cv::Scalar(0, 0, 255) : cv::Scalar();
    cv::rectangle(canvas, cv::Rect2d(center() - c_paddleHeight / 2.0, m_position - c_paddleWidth / 2.0, c_paddleHeight, c_paddleWidth), color, -1);
}

int Paddle::direction() const
{
    return m_direction;
}

int Paddle::center() const
{
    auto xPos = c_paddleSpacing * (m_layer + 1) + c_paddleHeight * (m_layer + 0.5);
    if(m_direction < 0)
    {
        xPos = Game::gameCanvasSize().width - xPos;
    }
    return xPos;
}

void Paddle::setTargetPosition(const double targetPosition)
{
    m_targetPosition = std::max(std::min(targetPosition, Game::gameCanvasSize().height - c_paddleWidth / 2.0), c_paddleWidth / 2.0);
}

int Paddle::edge() const
{
    return center() + c_paddleHeight * m_direction / 2.0;
}

double Paddle::position() const
{
    return m_position;
}

bool Paddle::active() const
{
    return m_active;
}

void Paddle::setActive(const bool active)
{
    m_active = active;
}

cv::Rect2d Paddle::boundingBox() const
{
    return cv::Rect2d(center() - c_paddleHeight / 2.0, m_position - c_paddleWidth / 2.0, c_paddleHeight, c_paddleWidth);
}

Ball::Ball(const cv::Point2d& position, const cv::Point2d& movement) : m_position(position), m_movement(movement) { }

double Ball::ballMovementSpeed()
{
    return c_ballMovementSpeed;
}

void Ball::update(const double deltaSeconds, const std::vector<Paddle>& paddles)
{
    const auto target = m_position + m_movement * deltaSeconds;

    auto collided = false;

    for (const auto& paddle : paddles)
    {
        if(m_movement.x < 0 == paddle.direction() < 0)
        {
            continue;
        }

        const auto yDist = paddle.position() - m_position.y;

        if(std::abs(yDist) > (c_ballDiamater + Paddle::paddleWidth()) / 2.0)
        {
            continue;
        }

        // TODO: include radius in calculation
        const auto edge = paddle.edge();
        const auto edgeToPosition = m_position.x - edge;
        const auto edgeToTarget = target.x - edge;

        if(edgeToPosition < 0 == edgeToTarget < 0)
        {
            continue;
        }

        const auto angle = c_maxBounceAngle * yDist * 2 / Paddle::paddleWidth();

        const auto deltaY = std::sin(angle) * c_ballMovementSpeed;
        const auto deltaX = std::cos(angle) * c_ballMovementSpeed * -m_movement.x / std::abs(m_movement.x);

        m_movement = cv::Point2d(deltaX, -deltaY);
        collided = true;
    }

    if(target.y < 0 || target.y > Game::gameCanvasSize().height)
    {
        m_movement.y *= -1;
        collided = true;
    }

    if(!collided)
    {
        m_position = target;
    }
}

void Ball::draw(cv::Mat& canvas) const
{
    cv::circle(canvas, m_position, c_ballDiamater / 2, cv::Scalar(), -1);
}

cv::Point2d Ball::position() const
{
    return m_position;
}

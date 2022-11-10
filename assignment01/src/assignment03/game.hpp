#pragma once

#include "fullscreenWindow.hpp"

#include <opencv2/opencv.hpp>
#include "calibration.hpp"
#include <chrono>

class Paddle;
class Player;
class Ball;

class Game : public FullscreenWindow
{
public:
    explicit Game(HomograpyCalibrator& calibrator);
    static std::vector<cv::Point2f> gameCorners();
    static cv::Rect2i gameCanvasSize();
    void reset();
    void start();
    bool initialized() const;
    void update();
    void draw(const cv::Mat & colorFrame);
    void drawTouches(const std::vector<cv::RotatedRect>& touches, int count = -1);
    void show();
    std::vector<Player>& players();
private:
    // framework
    void drawCircle(const cv::Point2f& position);
    std::string windowName() override;

    const std::string c_windowName = "game";
	static const cv::Rect2i c_gameCanvasSize;

    cv::Mat m_gameCanvas;
    HomograpyCalibrator& m_calibrator;

    // pong
    static const int c_ballCount = 2;

    bool m_running = false;
    std::chrono::steady_clock::time_point m_lastUpdate;
    std::vector<Player> m_players;
    std::vector<Ball> m_balls;
};

class Player
{
public:
    explicit Player(int direction);
    int score() const;
    std::vector<Paddle>& paddles();
    void touchdown();
    void update(double deltaSeconds);
    void draw(cv::Mat& canvas) const;
private:
    static const int c_paddleCount = 2;
    int m_score = 0;
    std::vector<Paddle> m_paddles;
};

class Paddle
{
public:
    Paddle(int layer, int direction);
    static int paddleWidth();
    void update(double deltaSeconds);
    void draw(cv::Mat& canvas) const;
    int direction() const;
    int center() const;
    void setTargetPosition(double targetPosition);
    int edge() const;
    double position() const;
    bool active() const;
    void setActive(bool active);
    cv::Rect2d boundingBox() const;
private:
    static const int c_paddleWidth = 200;
    static const int c_paddleHeight = 20;
    static const int c_paddleSpacing = 5;
    constexpr static const double c_paddleMovementSpeed = 150.0;
    int m_layer;
    double m_position = Game::gameCanvasSize().height / 2.0;
    double m_targetPosition = m_position;
    bool m_active = false;
    int m_direction;
};

class Ball
{
public:
    Ball(const cv::Point2d& position, const cv::Point2d& movement);
    static double ballMovementSpeed();
    void update(double deltaSeconds, const std::vector<Paddle>& paddles);
    void draw(cv::Mat& canvas) const;
    cv::Point2d position() const;
private:
    static const int c_ballDiamater = 50;
    constexpr static const double c_ballMovementSpeed = 150.0;
    constexpr static const double c_maxBounceAngle = 1.48353; // 85°

    cv::Point2d m_position;
    cv::Point2d m_movement;
};
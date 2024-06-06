#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

struct Paddle
{
    float x = 0.0f, y = 0.0f;
    float speed = 200.0f;
};

struct Ball
{
    float x = 0.0f, y = 0.0f;
    float vx = 0.0f, vy = 0.0f;
    float speed = 100.0f;
};

class PixelPaddles : public olc::PixelGameEngine
{
public:
    PixelPaddles()
    {
        sAppName = "Pixel Paddles";
    }

private:
    Paddle player, computer;
    Ball ball;
    int playerScore = 0, computerScore = 0;
    olc::Pixel playerColor = olc::BLACK;
    olc::Pixel computerColor = olc::RED;
    olc::Pixel ballColor = olc::WHITE;
    olc::Pixel backgroundColor = olc::DARK_BLUE;
    olc::Pixel middleLineColor = olc::WHITE;
    float computerDifficulty = 0.5f; // Lower values make the computer more likely to miss

public:
    bool OnUserCreate() override
    {
        // Initialize paddles
        player = { 10.0f, ScreenHeight() / 2.0f - 10.0f };
        computer = { ScreenWidth() - 20.0f, ScreenHeight() / 2.0f - 10.0f };

        // Initialize ball
        ball = { ScreenWidth() / 2.0f, ScreenHeight() / 2.0f, -100.0f, 60.0f };

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        HandlePlayerMovement(fElapsedTime);
        HandleComputerMovement(fElapsedTime);
        UpdateBall(fElapsedTime);
        DrawGame();

        return true;
    }

private:
    void HandlePlayerMovement(float fElapsedTime)
    {
        // Player controls
        if (GetKey(olc::UP).bHeld) player.y -= player.speed * fElapsedTime;
        if (GetKey(olc::DOWN).bHeld) player.y += player.speed * fElapsedTime;

        // Ensure paddle stays within screen bounds
        if (player.y < 0) player.y = 0;
        if (player.y > ScreenHeight() - 20) player.y = ScreenHeight() - 20;
    }

    void HandleComputerMovement(float fElapsedTime)
    {
        // Simple AI for computer paddle with randomness
        if (ball.vx > 0)
        {
            float targetY = ball.y - 10.0f;
            float randomness = (float(rand()) / float(RAND_MAX) - 0.5f) * 30.0f; // Adds randomness
            targetY += randomness;

            if (targetY < computer.y) computer.y -= computer.speed * fElapsedTime * computerDifficulty;
            if (targetY > computer.y) computer.y += computer.speed * fElapsedTime * computerDifficulty;
        }

        // Ensure paddle stays within screen bounds
        if (computer.y < 0) computer.y = 0;
        if (computer.y > ScreenHeight() - 20) computer.y = ScreenHeight() - 20;
    }

    void UpdateBall(float fElapsedTime)
    {
        ball.x += ball.vx * fElapsedTime;
        ball.y += ball.vy * fElapsedTime;

        // Ball collision with top and bottom
        if (ball.y <= 0 || ball.y >= ScreenHeight() - 1)
            ball.vy *= -1;

        // Ball collision with paddles
        if (ball.x <= player.x + 2 && ball.y >= player.y && ball.y <= player.y + 20)
            ball.vx *= -1;
        if (ball.x >= computer.x - 2 && ball.y >= computer.y && ball.y <= computer.y + 20)
            ball.vx *= -1;

        // Reset ball if it goes out of bounds
        if (ball.x <= 0)
        {
            computerScore++;
            ResetBall();
        }
        if (ball.x >= ScreenWidth())
        {
            playerScore++;
            ResetBall();
        }
    }

    void ResetBall()
    {
        ball = { ScreenWidth() / 2.0f, ScreenHeight() / 2.0f, -100.0f, 60.0f };
    }

    void DrawGame()
    {
        // Clear screen
        Clear(backgroundColor);

        // Draw paddles
        FillRect(static_cast<int32_t>(player.x), static_cast<int32_t>(player.y), 2, 20, playerColor);
        FillRect(static_cast<int32_t>(computer.x), static_cast<int32_t>(computer.y), 2, 20, computerColor);

        // Draw ball
        FillRect(static_cast<int32_t>(ball.x), static_cast<int32_t>(ball.y), 2, 2, ballColor);

        // Draw middle line
        for (int y = 0; y < ScreenHeight(); y += 10)
        {
            DrawLine(ScreenWidth() / 2, y, ScreenWidth() / 2, y + 5, middleLineColor);
        }

        // Draw score
        DrawString(10, 10, "Player: " + std::to_string(playerScore), olc::WHITE);
        DrawString(ScreenWidth() - 100, 10, "Computer: " + std::to_string(computerScore), olc::WHITE);
    }
};

int main()
{
    PixelPaddles game;
    if (game.Construct(256, 240, 4, 4))
        game.Start();
    return 0;
}

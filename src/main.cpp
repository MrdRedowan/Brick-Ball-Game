// -------------------------------- Header Files --------------------------------
#include <bits/stdc++.h>
#include <raylib.h>

// ---------------------------------- Constants ---------------------------------
const int WIDTH = 1200;
const int HEIGHT = 700;
const float PADDLE_WIDTH = 10;
const float PADDLE_HEIGHT = 100;
const float BALL_SIZE = 10;
const float MAX_BOUNCE_ANGLE = 60 * (PI / 180.0f); // Max bounce angle in radians
const float INITIAL_BALL_SPEED = 300.0f;

// -------------------------- Global Sound Variables -----------------------------
Sound Intro;
Sound Select;
Sound PaddleHit;
Sound WallHit;
Sound ServeLoss;
Sound GameOver;
Sound BotWon;

// ---------------------------------Game settings --------------------------------
const int MAX_LIVES = 3;
const int HIT_SCORE = 10;

// ------------------------------------ Enum ------------------------------------
enum GameMode
{
    SINGLE_PLAYER,
    PLAYER_VS_BOT,
    PLAYER_VS_PLAYER,
    PLAYER_VS_PLAYER_2v2
};

// ------------------------------- Global Variables -------------------------------
GameMode gameMode = PLAYER_VS_BOT;
float botPaddleSpeed = .0f;
float playerPaddleSpeed = 6.0f;
bool playAudio = true;
bool serveDelay = true;
float serveStartTime=0.0f;

// ------------------------------- Function Declarations (Memberwise) -----------------------------

//-------------------------------------------  Nishad  --------------------------------------------
void HandleModeSelection(bool &modeChosen, bool &difficultyChosen);

void HandleDifficultySelection(bool &difficultyChosen, float &botPaddleSpeed, float &ballSpeed);

//--------------------------------------------  Rahul  --------------------------------------------
void HandleGameOver(bool &gameOver, bool &player1Won, bool &modeChosen, bool &difficultyChosen,
                    int &player1Lives, int &player2Lives, int &player1Score, int &player2Score,
                    Vector2 &ballPosition, Vector2 &ballDirection, float &ballSpeed);

//-------------------------------------------  Redowan  --------------------------------------------
void UpdateGameLogic(Vector2 &ballPosition, Vector2 &ballDirection, float &ballSpeed,
                     int &player1Lives, int &player2Lives, int &player1Score, int &player2Score,
                     float &player1PaddleY, float &player2PaddleY, bool &gameOver, bool &player1Won);

void DrawGameState(float player1PaddleY, float player2PaddleY, Vector2 ballPosition,
                   int player1Lives, int player2Lives, int player1Score, int player2Score);

//--------------------------------------------  Theo  ----------------------------------------------
void ResetBall(Vector2 &ballPosition, Vector2 &ballDirection, float &ballSpeed);

void waitingForServe();

//-------------------------------------------  Hakim  ----------------------------------------------
void HandlePaddleMovement(float &player1PaddleY, float &player2PaddleY, const Vector2 &ballPosition);

void HandleBallCollision(Vector2 &ballDirection, float &ballSpeed, int &player1Score, int &player2Score,
                         const float player1PaddleY, const float player2PaddleY, Vector2 &ballPosition);

// ------------------------------------------ Main Function -----------------------------------------
int main()
{
    InitWindow(WIDTH, HEIGHT, "Brick Ball Game");
    SetTargetFPS(60);

    InitAudioDevice();
    // Initialize sounds
    Intro = LoadSound("Sounds/Intro.wav");
    Select = LoadSound("Sounds/Select.wav");
    PaddleHit = LoadSound("Sounds/PaddleHit.wav");
    WallHit = LoadSound("Sounds/WallHit.wav");
    ServeLoss = LoadSound("Sounds/ServeLoss.wav");
    GameOver = LoadSound("Sounds/GameOver.wav");
    BotWon = LoadSound("Sounds/BotWon.wav");

    // Game state variables
    bool modeChosen = false;
    bool difficultyChosen = false;

    // Paddle positions
    float player1PaddleY = (HEIGHT - PADDLE_HEIGHT) / 2;
    float player2PaddleY = (HEIGHT - PADDLE_HEIGHT) / 2;

    // Ball state
    Vector2 ballPosition = {WIDTH / 2, HEIGHT / 2};
    float ballSpeed = INITIAL_BALL_SPEED;
    Vector2 ballDirection = {1.0f, 1.0f};

    // Player stats
    int player1Lives = MAX_LIVES;
    int player2Lives = MAX_LIVES;
    int player1Score = 0;
    int player2Score = 0;

    // Game over state
    bool gameOver = false;
    bool player1Won = false;

    while (!WindowShouldClose())
    {
        
        if (!modeChosen)
        {
            playAudio = true;
            HandleModeSelection(modeChosen, difficultyChosen);
            continue;
        }

        if ((gameMode == PLAYER_VS_BOT || gameMode == SINGLE_PLAYER) && !difficultyChosen)
        {
            HandleDifficultySelection(difficultyChosen, botPaddleSpeed, ballSpeed);
            continue;
        }

        if (gameOver)
        {
            HandleGameOver(gameOver, player1Won, modeChosen, difficultyChosen, player1Lives, player2Lives, player1Score, player2Score, ballPosition, ballDirection, ballSpeed);
            continue;
        }

        UpdateGameLogic(ballPosition, ballDirection, ballSpeed, player1Lives, player2Lives, player1Score, player2Score, player1PaddleY, player2PaddleY, gameOver, player1Won);

        // Drawing
        DrawGameState(player1PaddleY, player2PaddleY, ballPosition, player1Lives, player2Lives, player1Score, player2Score);
    }

    // Unload sounds
    UnloadSound(Intro);
    UnloadSound(Select);
    UnloadSound(PaddleHit);
    UnloadSound(WallHit);
    UnloadSound(ServeLoss);
    UnloadSound(GameOver);
    UnloadSound(BotWon);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}


// ------------------------------ Function Definitions --------------------------------

void HandleModeSelection(bool &modeChosen, bool &difficultyChosen)
{
    BeginDrawing();
    ClearBackground(BLACK);

    if (!IsSoundPlaying(Intro)) 
    {
            PlaySound(Intro);
    }
    
    DrawText("Simple Brick Ball Game", (WIDTH - MeasureText("Simple Brick Ball Game", 90)) / 2, (HEIGHT/2) - 300, 90, RED);
    DrawText("Select Game Mode:", (WIDTH - MeasureText("Select Game Mode:", 60)) / 2, (HEIGHT/2)-130, 60, WHITE);
    DrawText("1. Singleplayer", (WIDTH - MeasureText("1. Singleplayer", 40)) / 2, (HEIGHT/2-20), 40, GREEN);
    DrawText("2. Player vs Bot", (WIDTH - MeasureText("2. Player vs Bot", 40)) / 2, (HEIGHT/2+30), 40, GREEN);
    DrawText("3. Player vs Player", (WIDTH - MeasureText("3. Player vs Player", 40)) / 2, (HEIGHT/2+80), 40, BLUE);
    DrawText("4. Player vs Player(2v2)", (WIDTH - MeasureText("4. Player vs Player(2v2)", 40)) / 2, (HEIGHT/2+130), 40, BLUE);

    DrawText("F1. How To Play?", 50 , 650, 20, GOLD);
    DrawText("F10. Leaderboard", 975 , 650, 20, GOLD);
    DrawText("Esc. Exit", 550 , 650, 20, GOLD);


    if (IsKeyPressed(KEY_ONE))
    {
        PlaySound(Select);
        gameMode = SINGLE_PLAYER;
        modeChosen = true;
        StopSound(Intro);
    }

    if (IsKeyPressed(KEY_TWO))
    {
        PlaySound(Select);
        gameMode = PLAYER_VS_BOT;
        modeChosen = true;
        StopSound(Intro);
    }
    if (IsKeyPressed(KEY_THREE))
    {
        PlaySound(Select);
        gameMode = PLAYER_VS_PLAYER;
        modeChosen = true;
        difficultyChosen = true; // Skip difficulty selection for Player vs Player
        StopSound(Intro);
    }

    if (IsKeyPressed(KEY_FOUR))
    {
        PlaySound(Select);
        gameMode = PLAYER_VS_PLAYER_2v2;
        modeChosen = true;
        difficultyChosen = true; // Skip difficulty selection for Player vs Player (2v2)
        StopSound(Intro);
    }

    EndDrawing();
}

void HandleDifficultySelection(bool &difficultyChosen, float &botPaddleSpeed , float &ballSpeed)
{
    BeginDrawing();
    ClearBackground(BLACK);

    DrawText("Simple Brick Ball Game", (WIDTH - MeasureText("Simple Brick Ball Game", 90)) / 2, (HEIGHT/2) - 250, 90, RED);

    DrawText("Select Difficulty:", WIDTH / 2 - MeasureText("Select Difficulty:", 60) / 2, HEIGHT / 2 - 80, 60, WHITE);
    DrawText("1. Easy", WIDTH / 2 - MeasureText("1. Easy", 40) / 2, (HEIGHT / 2)+30, 40, GREEN);
    DrawText("2. Normal", WIDTH / 2 - MeasureText("2. Normal", 40) / 2, HEIGHT / 2 + 80, 40, YELLOW);
    DrawText("3. Difficult", WIDTH / 2 - MeasureText("3. Difficult", 40) / 2, HEIGHT / 2 + 130, 40, RED);
    DrawText("4. HardCore", WIDTH / 2 - MeasureText("4. HardCore", 40) / 2, HEIGHT / 2 + 180, 40, RED);

    DrawText("F1. How To Play?", 50 , 650, 20, GOLD);
    DrawText("F10. Leaderboard", 975 , 650, 20, GOLD);
    DrawText("Esc. Exit", 550 , 650, 20, GOLD);

    if (IsKeyPressed(KEY_ONE))
    {
        PlaySound(Select);
        botPaddleSpeed = 5.0f;
        difficultyChosen = true;
    }
    if (IsKeyPressed(KEY_TWO))
    {
        PlaySound(Select);
        botPaddleSpeed = 6.0f;
        playerPaddleSpeed = 8.0f;
        ballSpeed = 400.0f;
        difficultyChosen = true;
    }
    if (IsKeyPressed(KEY_THREE))
    {
        PlaySound(Select);
        botPaddleSpeed = 8.0f;
        playerPaddleSpeed = 10.0f;
        ballSpeed = 500.0f;
        difficultyChosen = true;
    }
    if (IsKeyPressed(KEY_FOUR))
    {
        PlaySound(Select);
        botPaddleSpeed = 10.0f;
        playerPaddleSpeed = 12.0f;
        ballSpeed = 600.0f;
        difficultyChosen = true;
    }

    EndDrawing();
}

void HandleGameOver(bool &gameOver, bool &player1Won, bool &modeChosen, bool &difficultyChosen,
                            int &player1Lives, int &player2Lives, int &player1Score, int &player2Score,
                                         Vector2 &ballPosition, Vector2 &ballDirection, float &ballSpeed)
{
    BeginDrawing();
    ClearBackground(BLACK);

    if (player1Won && (gameMode == PLAYER_VS_BOT || gameMode == PLAYER_VS_PLAYER))
    {
        DrawText("Player 1 Wins!", (WIDTH - MeasureText("Player 1 Wins!", 70)) / 2, HEIGHT / 2 - 80, 70, GOLD);

        if (!IsSoundPlaying(GameOver) && playAudio) 
        {
            PlaySound(GameOver);
            playAudio = false;
        }
    }
    else if(!player1Won && gameMode == PLAYER_VS_BOT)
    {
        DrawText("Bot Wins!", (WIDTH - MeasureText("Bot Wins!", 70)) / 2, HEIGHT / 2 - 150, 70, RED);
        DrawText("AI Will Take Over The World!", (WIDTH - MeasureText("AI Will Take Over The World!", 50)) / 2, HEIGHT / 2 - 40, 50, RED);
        if (!IsSoundPlaying(BotWon) && playAudio) 
        {
            SetSoundVolume(BotWon, 0.2);
            PlaySound(BotWon);
            playAudio = false;
        }
    }
    else if(!player1Won && gameMode == PLAYER_VS_PLAYER)
    {
        DrawText("Player 2 Wins!", (WIDTH - MeasureText("Player 2 Wins!", 70)) / 2, HEIGHT / 2 - 80, 70, BLUE);

        if (!IsSoundPlaying(GameOver) && playAudio) 
        {
            PlaySound(GameOver);
            playAudio = false;
        }
    }

    DrawText("Press ENTER to Play Again", (WIDTH - MeasureText("Press ENTER to Play Again", 40)) / 2, HEIGHT / 2 + 50, 40, WHITE);
    DrawText("Esc. Exit", 550 , 650, 20, GOLD);

    if (IsKeyPressed(KEY_ENTER))
    {
        PlaySound(Select);
        // Reset game
        player1Lives = MAX_LIVES;
        player2Lives = MAX_LIVES;
        player1Score = 0;
        player2Score = 0;
        gameOver = false;

        // Reset ball position
        ResetBall(ballPosition, ballDirection, ballSpeed);
        ballSpeed = INITIAL_BALL_SPEED;

        // Allow mode & difficulty selection again
        modeChosen = false;
        difficultyChosen = false;
    }

    EndDrawing();
}

void UpdateGameLogic(Vector2 &ballPosition, Vector2 &ballDirection, float &ballSpeed,
                     int &player1Lives, int &player2Lives, int &player1Score, int &player2Score,
                     float &player1PaddleY, float &player2PaddleY, bool &gameOver, bool &player1Won)
{

    // Paddle's movement Handling
    HandlePaddleMovement(player1PaddleY, player2PaddleY, ballPosition);

    // Handle serve delay
    if (serveDelay)
    {
        waitingForServe();
        return;
    }
    // Ball's movement Handling
    ballPosition.x += ballDirection.x * ballSpeed * GetFrameTime();
    ballPosition.y += ballDirection.y * ballSpeed * GetFrameTime();

    // Collision Handling
    HandleBallCollision(ballDirection, ballSpeed, player1Score, player2Score, player1PaddleY, player2PaddleY, ballPosition);

    // Checking if the ball goes out of bounds
    if (ballPosition.x < 0)
    {
        player1Lives--;
        ResetBall(ballPosition, ballDirection, ballSpeed);
        player2Score+=100;
    }
    else if (ballPosition.x > WIDTH && gameMode !=SINGLE_PLAYER)
    {
        player2Lives--;
        ResetBall(ballPosition, ballDirection, ballSpeed);
        player1Score+=100;
    }

    // Checking for game over
    if (player1Lives == 0 || player2Lives == 0)
    {
        gameOver = true;
        player1Won = player1Lives > 0;
    }
}

void DrawGameState(float player1PaddleY, float player2PaddleY, Vector2 ballPosition, int player1Lives, int player2Lives, int player1Score, int player2Score)
{
    BeginDrawing();
    ClearBackground(BLACK);

    // Draw paddles and ball
    DrawRectangle(0, player1PaddleY, PADDLE_WIDTH, PADDLE_HEIGHT, WHITE);
    if(gameMode !=SINGLE_PLAYER)
    {
        DrawRectangle(WIDTH - PADDLE_WIDTH, player2PaddleY, PADDLE_WIDTH, PADDLE_HEIGHT, WHITE);
    }
    DrawCircle(ballPosition.x, ballPosition.y, BALL_SIZE, WHITE);


    // Draw scores and lives
    DrawText(TextFormat("P1 Lives: %d", player1Lives), 20, 20, 20, GREEN);
    DrawText(TextFormat("P1 Score: %d", player1Score), 20, 50, 20, GREEN);
    if(gameMode !=SINGLE_PLAYER)
    {
        DrawText(TextFormat("P2 Lives: %d", player2Lives), WIDTH - 150, 20, 20, BLUE);
        if(gameMode == PLAYER_VS_PLAYER)
        {
            DrawText(TextFormat("P2 Score: %d", player2Score), WIDTH - 150, 50, 20, BLUE);
        }
    }

    EndDrawing();
}

void ResetBall(Vector2 &ballPosition, Vector2 &ballDirection, float &ballSpeed)
{
    serveDelay = true;
    //waitingForServe();
    ballPosition = {WIDTH / 2, HEIGHT / 2};
    ballDirection = {ballDirection.x, ballDirection.y > 0 ? 1.0f : -1.0f};
    ballSpeed = ballSpeed;
    PlaySound(ServeLoss);
}

void waitingForServe()
{
    if (serveStartTime < 1.0)
    {
        serveStartTime = GetTime(); // Record the start time of the delay
    }

    // Wait for 1 second
    if (GetTime() - serveStartTime >= 1.0)
    {
        serveDelay = false;
        serveStartTime = 0.0;
    }
}

void HandleBallCollision(Vector2 &ballDirection, float &ballSpeed, int &player1Score, int &player2Score, const float player1PaddleY, const float player2PaddleY, Vector2 &ballPosition)
{
    if (ballPosition.x >= WIDTH &&  gameMode ==SINGLE_PLAYER ) // Bounce with the right wall
    { 
        //ballDirection.y = -ballDirection.y;
        ballDirection.x =-(ballDirection.x) ;
        player1Score += HIT_SCORE;
        PlaySound(WallHit);
    }
    if ((ballPosition.y <= 0 || ballPosition.y >= HEIGHT)) // Bounce with the top and bottom walls
    { 
        ballDirection.y = -ballDirection.y;
        PlaySound(WallHit);
    }

    if (ballPosition.x < PADDLE_WIDTH && 
            ballPosition.y > player1PaddleY && 
                ballPosition.y < player1PaddleY + PADDLE_HEIGHT)
    {
        ballDirection.x = fabs(ballDirection.x); // Bounce with the player1 paddle
        player1Score += HIT_SCORE;
        PlaySound(PaddleHit);
    }

    if (ballPosition.x >= WIDTH-PADDLE_WIDTH-BALL_SIZE
                    && ballPosition.y >= player2PaddleY &&
                        ballPosition.y <= player2PaddleY + PADDLE_HEIGHT && gameMode !=SINGLE_PLAYER) // Bounce with the player2 paddle
    {
        ballDirection.x = -fabs(ballDirection.x);
        player2Score += HIT_SCORE;
        PlaySound(PaddleHit);
    }
}
void HandlePaddleMovement(float &player1PaddleY, float &player2PaddleY, const Vector2 &ballPosition)
{ // Player 1 movement
    if (IsKeyDown(KEY_W) && player1PaddleY >= 0)
        player1PaddleY -= playerPaddleSpeed;
    if (IsKeyDown(KEY_S) && player1PaddleY <= HEIGHT - PADDLE_HEIGHT)
        player1PaddleY += playerPaddleSpeed;
    // Player 2 or Bot movement
    if (gameMode == PLAYER_VS_PLAYER)
    {
        if (IsKeyDown(KEY_UP) && player2PaddleY >= 0)
            player2PaddleY -= playerPaddleSpeed;
        if (IsKeyDown(KEY_DOWN) && player2PaddleY <= HEIGHT - PADDLE_HEIGHT)
            player2PaddleY += playerPaddleSpeed;
    }
    else
    {
        // Movement Logic for Bot AI
        if (player2PaddleY + PADDLE_HEIGHT / 2 < ballPosition.y)
            player2PaddleY += botPaddleSpeed;
        if (player2PaddleY + PADDLE_HEIGHT / 2 > ballPosition.y)
            player2PaddleY -= botPaddleSpeed;
        if (player2PaddleY < 0)
            player2PaddleY = 0;
        if (player2PaddleY > HEIGHT - PADDLE_HEIGHT)
            player2PaddleY = HEIGHT - PADDLE_HEIGHT;
    }
}

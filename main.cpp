#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;

static bool canMove = false;
Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};

int size = 30;
int count = 25;
int padding = 75;

double lastUpdate = 0;

bool IsInDeque(Vector2 element, deque<Vector2> deque)
{
    for (unsigned int i = 0; i < deque.size(); i++)
    {
        if (Vector2Equals(deque[i], element))
        {
            return true;
        }
    }
    return false;
}

bool IsEventTriggered(double interval)
{
    double currentTime = GetTime();
    if (currentTime - lastUpdate >= interval)
    {
        lastUpdate = currentTime;
        return true;
    }
    return false;
}

class Snake
{
public:
    deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    Vector2 direction = {1, 0};
    bool grow = false;

    void Draw()
    {
        for (unsigned int i = 0; i < body.size(); i++)
        {
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = Rectangle{padding + x * size, padding + y * size, (float)size, (float)size};
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
    }

    void Update()
    {
        body.push_front(Vector2Add(body[0], direction));
        if (grow == true)
        {
            grow = false;
        }
        else
        {
            body.pop_back();
        }
    }

    void Reset()
    {
        body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        direction = {1, 0};
    }
};

class Food
{

public:
    Vector2 position;
    Texture2D texture;

    Food(deque<Vector2> snakeBody)
    {
        Image image = LoadImage("Graphics/food.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPos(snakeBody);
    }

    ~Food()
    {
        UnloadTexture(texture);
    }

    void Draw()
    {
        DrawTexture(texture, padding + position.x * size, padding + position.y * size, WHITE);
    }

    Vector2 GenerateRandomCell()
    {
        float x = GetRandomValue(0, count - 1);
        float y = GetRandomValue(0, count - 1);
        return Vector2{x, y};
    }

    Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
    {
        Vector2 position = GenerateRandomCell();
        while (IsInDeque(position, snakeBody))
        {
            position = GenerateRandomCell();
        }
        return position;
    }
};

class Game
{
public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool isRunning = true;
    int score = 0;
    int highScore = 0;
    Sound eatSound;
    Sound wallSound;

    Game()
    {
        InitAudioDevice();
        eatSound = LoadSound("Sounds/eat.mp3");
        wallSound = LoadSound("Sounds/wall.mp3");
    }

    ~Game()
    {
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }

    void Draw()
    {
        food.Draw();
        snake.Draw();
    }

    void Update()
    {
        if (isRunning)
        {
            snake.Update();
            CheckFoodCollision();
            CheckEdgeCollision();
            CheckTailCollision();
        }
    }

    void CheckFoodCollision()
    {
        if (Vector2Equals(snake.body[0], food.position))
        {
            food.position = food.GenerateRandomPos(snake.body);
            snake.grow = true;
            score++;
            PlaySound(eatSound);
        }
    }

    void CheckEdgeCollision()
    {
        // Wrap from right edge to left edge
        if (snake.body[0].x >= count)
        {
            snake.body[0].x = 0;
        }
        // Wrap from left edge to right edge
        else if (snake.body[0].x < 0)
        {
            snake.body[0].x = count - 1;
        }

        // Wrap from bottom edge to top edge
        if (snake.body[0].y >= count)
        {
            snake.body[0].y = 0;
        }
        // Wrap from top edge to bottom edge
        else if (snake.body[0].y < 0)
        {
            snake.body[0].y = count - 1;
        }
        // if (snake.body[0].x == count || snake.body[0].x == -1)
        // {
        //     GameOver();
        // }
        // if (snake.body[0].y == count || snake.body[0].y == -1)
        // {
        //     GameOver();
        // }
    }

    void GameOver()
    {
        if (score > highScore)
        {
            highScore = score; 
        }
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        isRunning = false;
        score = 0;
        PlaySound(wallSound);
    }

    void CheckTailCollision()
    {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (IsInDeque(snake.body[0], headlessBody))
        {
            GameOver();
        }
    }
};

int main()
{
    cout << "Starting the game..." << endl;
    InitWindow(2 * padding + size * count, 2 * padding + size * count, "Snake's Greedy Hunt");
    SetTargetFPS(60);

    Game game = Game();

    while (WindowShouldClose() == false)
    {
        BeginDrawing();

        if (IsEventTriggered(0.2))
        {
            canMove = true;
            game.Update();
        }

        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1 && canMove)
        {
            game.snake.direction = {0, -1};
            game.isRunning = true;
            canMove = false;
        }
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1 && canMove)
        {
            game.snake.direction = {0, 1};
            game.isRunning = true;
            canMove = false;
        }
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1 && canMove)
        {
            game.snake.direction = {-1, 0};
            game.isRunning = true;
            canMove = false;
        }
        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1 && canMove)
        {
            game.snake.direction = {1, 0};
            game.isRunning = true;
            canMove = false;
        }

        // Drawing
        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)padding - 5, (float)padding - 5, (float)size * count + 10, (float)size * count + 10}, 5, darkGreen);
        DrawText("Snake's Greedy Hunt", padding - 5, 20, 40, darkGreen);
        DrawText(TextFormat("%i", game.score), padding - 5, padding + size * count + 10, 40, darkGreen);
        DrawText(TextFormat("High Score: %i", game.highScore), padding - 5 + (size * count) / 2, padding + size * count + 10, 40, darkGreen); 
        game.Draw();

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
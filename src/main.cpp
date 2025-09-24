#include <raylib.h>
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <time.h>
#include <string>
#include <random>
// Global Variables

//Gamestates
typedef enum GameScreen { LOGO = 0, GAMESTART, GAMEPLAY } GameScreen;
int gameActive = 0;
int framesCounter = 0;
GameScreen currentScreen = LOGO;

// Window variables
float windowWidth = 1280;
float windowHeight = 1024;
float windowHalfWidth = windowWidth / 2;
float windowHalfHeight = windowHeight / 2;

// Global player variables
float playerXPosition = windowHalfWidth;
float playerYPosition = windowHalfHeight;
float playerSpeed = 500.f;

//Scoring
int Score = 0;
int Highscore = 0;
bool scored;
// Texture for paperplane

Texture2D paperplane;

float imageSizeX = 140;
float imageSizeY = 67;
float imageHalfSizeX = imageSizeX / 2;
float imageHalfSizeY = imageSizeY / 2;

//clouds variables
double movingX = 1000.f;
int randomY = 200;
double cloudsSize = 100.f;
Color cloudsC = WHITE;

double leftCloudX = 0;
double leftCloudY = 0;
double leftCloudSize = 0;

double rightCloudX = 0;
double rightCloudY = 0;
double rightCloudSize = 0;

double downCloudX = 0;
double downCloudY = 0;
double downCloudSize = 0;
float playerMovementSpeed = 5.f;

Color skye = { 116, 253, 255, 255 };

Vector2 myPlayerPosition{ 0,0 };

//Helper function
void DrawCenteredText(const char* text, int posY, int fontSize, Color color)
{
	// Measure the width of the text
	float textWidth = MeasureText(text, fontSize);

	// Calculate the X position to center the text
	// GetScreenWidth() is a Raylib function that returns the current screen width
	float posX = (GetScreenWidth() / 2.0f) - (textWidth / 2.0f);

	// Draw the text at the calculated position
	DrawText(text, (int)posX, posY, fontSize, color);
}

void Clouds() {
	leftCloudX = movingX - (cloudsSize * 1.25);
	leftCloudY = randomY + (cloudsSize * 0.5);
	leftCloudSize = cloudsSize * 0.75;

	rightCloudX = movingX + (cloudsSize * 1.25);
	rightCloudY = randomY + (cloudsSize * 0.60);
	rightCloudSize = cloudsSize * 0.90;

	rightCloudX = movingX + (cloudsSize * 1.25);
	rightCloudY = randomY + (cloudsSize * 0.60);
	rightCloudSize = cloudsSize * 0.90;

	downCloudX = movingX;
	downCloudY = randomY + (cloudsSize + 0.9);
	downCloudSize = cloudsSize * 0.7;

	DrawCircle(movingX, randomY, cloudsSize, cloudsC);
	DrawCircle(leftCloudX, leftCloudY, leftCloudSize, cloudsC);
	DrawCircle(rightCloudX, rightCloudY, rightCloudSize, cloudsC);
	DrawCircle(downCloudX, downCloudY, downCloudSize, cloudsC);
}
void MovingCloud() {
	if (rightCloudX < 0 - rightCloudSize) {
		movingX = movingX + (windowWidth * 1.5);
		rightCloudX = rightCloudX + (windowWidth * 1.5);
		leftCloudX = leftCloudX + (windowWidth * 1.5);
		randomY = GetRandomValue(100, windowHeight);
		cloudsSize = GetRandomValue(50, 100);
	}
	else {
		movingX -= playerMovementSpeed;
		rightCloudX -= playerMovementSpeed;
		leftCloudX -= playerMovementSpeed;
	}
}

// rectangle

float rectangleX = windowWidth;
float rectangleY = 0.f;
float rectangleHeight = 300.f;
float rectangleWidth = 200.f;

float rectangle2X = windowWidth;
float rectangle2Y = 0.f;
float rectangle2Height = 400.f;
float rectangle2Width = 200.f;

float SizeBetween = 200;

Rectangle tryRectangle = { 0, 0, 0, 0 };
Rectangle tryRectangle2 = { 0, 0, 0, 0 };

void rectangleHitbox() {
	rectangleY = windowHeight - rectangleHeight;
	rectangleWidth = 200;
	rectangleX -= playerMovementSpeed;
	tryRectangle = { rectangleX,rectangleY,rectangleWidth,rectangleHeight };
	if (rectangleX < 0 - rectangleWidth) {
		rectangleX = windowWidth;
		rectangleHeight = GetRandomValue(300, 800);
		scored = false;
	}
}
void rectangleHitbox2() {
	rectangle2Y = 0;
	rectangle2Width = 200;
	rectangle2X -= playerMovementSpeed;

	tryRectangle2 = { rectangle2X,rectangle2Y,rectangle2Width,rectangle2Height };
	rectangle2Height = windowHeight - rectangleHeight - SizeBetween;
	if (rectangle2X < 0 - rectangle2Width) {
		rectangle2X = windowWidth;
		SizeBetween = GetRandomValue(200, 250);
		playerMovementSpeed += 0.1;
		std::cout << playerMovementSpeed;
	}
}

void collisionDetection()
{
	myPlayerPosition = { playerXPosition,playerYPosition };
	if (CheckCollisionCircleRec(myPlayerPosition, 33.f, tryRectangle))
	{
		std::cout << "You hit number 1";
		Score = 0;
		currentScreen = GAMESTART;
		playerYPosition = windowHalfHeight;
			return;
	}
	myPlayerPosition = { playerXPosition,playerYPosition };
	if (CheckCollisionCircleRec(myPlayerPosition, 33.f, tryRectangle2))
	{
		std::cout << "You hit number 2";
		Score = 0;
		currentScreen = GAMESTART;
		playerYPosition = windowHalfHeight;
		return;
	}
}
void DrawPng() {
	DrawTexture(paperplane, playerXPosition - imageHalfSizeX, playerYPosition - imageHalfSizeY, WHITE);
	DrawRectangle(rectangleX, rectangleY, rectangleWidth, rectangleHeight, GREEN);
	DrawRectangle(rectangle2X, rectangle2Y, rectangle2Width, rectangle2Height, GREEN);
}
//Changed PlayerControlls
void PlayerController() {
	if (playerYPosition < 0 || playerYPosition > windowHeight)
	{
		Score = 0;
		playerYPosition = windowHalfHeight;
		playerXPosition = windowHalfWidth;
		gameActive = 4;
		if (gameActive == 4)
		{
			currentScreen = GAMESTART;
		}
	}
	if (IsKeyDown(KEY_SPACE)) {
		playerYPosition -= playerSpeed * GetFrameTime();
	}
	else {
		playerYPosition += (playerSpeed / 2) * GetFrameTime();
		GetApplicationDirectory();
	}
}

void UpdateGameScore()
{
	if (!scored && rectangleX < playerXPosition)
	{
		Score++;
		scored = true;
	}

	if (Score > Highscore) Highscore = Score;
}

void drawplayer() {
	float playerSize = 33.f;
	Color playerColor = { 102, 0, 102, 255 };

	//Circle for testing hitbox
	//DrawCircle(playerXPosition, playerYPosition, playerSize, YELLOW);
}

int main()
{
	InitWindow(windowWidth, windowHeight, "My first window!!");
	SetTargetFPS(120);

	paperplane = LoadTexture("src/Resources/Paperplane.png");

	while (!WindowShouldClose()) {
		switch (currentScreen)
		{
		case LOGO:
		{
			// TODO: Update LOGO screen variables here!
			gameActive = 2; // set gameActive to 2 when showing logo
			framesCounter++;

			// Wait for 3 seconds (360/120 frames per second) before jumping to GAMESTART screen
			if (framesCounter > 360)
			{
				currentScreen = GAMESTART;
			}
		} break;
		case GAMESTART:
		{
			// TODO: Update GAMESTART screen variables here!
			// Press enter to change to GAMEPLAY screen
			playerMovementSpeed = 0;
			playerSpeed = 0;
			rectangleX = windowWidth;
			rectangle2X = windowWidth;

			if (IsKeyReleased(KEY_SPACE)  )
			{

				currentScreen = GAMEPLAY;
				gameActive = 1; // set gameActive to 1 when starting the game
			}
		} break;
		case GAMEPLAY:
		{
			playerMovementSpeed = 5.f;
			playerSpeed = 500.f;
			// TODO: Update GAMEPLAY screen variables here!
		} break;
		default: break;
		}

		BeginDrawing();

		ClearBackground(skye);

		switch (currentScreen)
		{
		case LOGO:
		{
			DrawCenteredText("LOGO SCREEN", (float)windowHalfHeight, 50, LIGHTGRAY);
			DrawCenteredText("WAIT for 3 SECONDS...", (float)windowHalfHeight - 100, 40, GRAY);
		} break;
		case GAMESTART:
		{
			
			drawplayer();
			DrawText(TextFormat("Score: %01i", Score), 50, 50, 25, BLACK);
			DrawText(TextFormat("Highscore: %01i", Highscore), 50, 100, 25, GREEN);

			DrawFPS(10, 10);

			DrawPng();

			Clouds();
			
			// TODO: Draw GAMESTART screen here!
			//Insert drawings here, set speed to 0

			DrawCenteredText("FLAPPY PLAAAAANE", (float)windowHalfHeight - 100, 50, DARKGREEN);
			DrawCenteredText("PRESS SPACE/Left Click to Play", (float)windowHalfHeight - 200, 40, DARKGREEN);
		} break;
		case GAMEPLAY:
		{
			// TODO: Draw GAMEPLAY screen here!
			PlayerController();
			collisionDetection();
			UpdateGameScore();
			drawplayer();
			DrawText(TextFormat("Score: %01i", Score), 50, 50, 25, BLACK);
			DrawText(TextFormat("Highscore: %01i", Highscore), 50, 100, 25, GREEN);

			DrawFPS(10, 10);

			DrawPng();

			Clouds();
			MovingCloud();
			rectangleHitbox();
			rectangleHitbox2();
			//PLAYER GOES HERE
		} break;
		default: break;
		}

		
		EndDrawing();
	}
	UnloadTexture(paperplane);
	CloseWindow();

	return 0;
}
#include <raylib.h>
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <time.h>
#include <string>
#include <random>

// Global Variables
//Gamestates------------------------------------------------------
typedef enum GameScreen { LOGO = 0, GAMESTART, GAMEPLAY } GameScreen;
bool gameActive = false;
int framesCounter = 0;
GameScreen currentScreen = LOGO;

// Window variables-----------------------------------------------
float windowWidth = 1280;
float windowHeight = 1024;
float windowHalfWidth = windowWidth / 2;
float windowHalfHeight = windowHeight / 2;

// Global player variables----------------------------------------
float playerXPosition = windowHalfWidth;
float playerYPosition = windowHalfHeight;
float playerSpeedY = 0;
const float playerRadius = 33.f;
const float gravity = 5000.f;
const float terminalVelocity = 1000.f;
const float jumpForce = -500.f;
const float jumpSustainForce = 750.f;

//Scoring---------------------------------------------------------
int Score = 0;
int Highscore = 0;
bool scored;

// Texture for paperplane-----------------------------------------
Texture2D paperplane;
float imageSizeX = 140;
float imageSizeY = 67;
float imageHalfSizeX = imageSizeX / 2;
float imageHalfSizeY = imageSizeY / 2;

//clouds variables------------------------------------------------
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
float objectMovementSpeed = 5.f;

Color skye = { 116, 253, 255, 255 };

Vector2 myPlayerPosition{ 0,0 };

// rectangle variables-------------------------------------------------
Rectangle tryRectangle = { 0, 0, 0, 0 };
Rectangle tryRectangle2 = { 0, 0, 0, 0 };

float rectangleX = windowWidth;
float rectangleY = 0.f;
float rectangleHeight = 300.f;
float rectangleWidth = 200.f;

float rectangle2X = windowWidth;
float rectangle2Y = 0.f;
float rectangle2Height = 400.f;
float rectangle2Width = 200.f;

float SizeBetween = 200;
const float initialRectangleGap = 200.f;
const float widthRectangle = 200.f;

//Helper function--------------------------------------------------------
void drawPlayerTest() {
	//Draws a Circle for testing hitbox
	float playerSize = 33.f;
	Color playerColor = BLACK;
	DrawCircle(myPlayerPosition.x, myPlayerPosition.y, playerSize, YELLOW);
}

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
		movingX -= objectMovementSpeed;
		rightCloudX -= objectMovementSpeed;
		leftCloudX -= objectMovementSpeed;
	}
}

void rectangleHitbox() {
	rectangleY = windowHeight - rectangleHeight;
	rectangleWidth = 200;
	rectangleX -= objectMovementSpeed;
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
	rectangle2X -= objectMovementSpeed;

	tryRectangle2 = { rectangle2X,rectangle2Y,rectangle2Width,rectangle2Height };
	rectangle2Height = windowHeight - rectangleHeight - SizeBetween;
	if (rectangle2X < 0 - rectangle2Width) {
		rectangle2X = windowWidth;
		SizeBetween = GetRandomValue(200, 250);
		objectMovementSpeed += 0.1;
		std::cout << objectMovementSpeed;
	}
}

void collisionDetection()
{
	myPlayerPosition = { playerXPosition,playerYPosition };
	if (CheckCollisionCircleRec(myPlayerPosition, playerRadius, tryRectangle) || CheckCollisionCircleRec(myPlayerPosition, playerRadius, tryRectangle2))
	{
		std::cout << "You hit one of the rectangles";
		std::cout << tryRectangle.x;
		currentScreen = GAMESTART;
		playerYPosition = windowHalfHeight - 100;
		return;
	}
	//Not needed as both rectangles are checked in one if statement
	/*myPlayerPosition = { playerXPosition,playerYPosition };
	if (CheckCollisionCircleRec(myPlayerPosition, playerRadius, tryRectangle2))
	{
		std::cout << "You hit number 2";
		std::cout << tryRectangle2.x;
		currentScreen = GAMESTART;
		playerYPosition = windowHalfHeight - 100;
		return;
	}*/
}

void DrawPlayerWithRotation() {
	// Calculate a rotation angle based on vertical velocity.
	// We multiply by a small factor to make the rotation look good.
	// We also clamp it so it doesn't spin too far.
	float rotation = playerSpeedY * 0.05f;
	if (rotation > 45.0f) rotation = 70.0f;   // Clamp max downward angle
	if (rotation < -30.0f) rotation = -30.0f; // Clamp max upward angle

	// Define the how big the texture is and attach it to the rectangle
	Rectangle sourceRec = { 0.0f, 0.0f, (float)paperplane.width, (float)paperplane.height };
	Rectangle destRec = { playerXPosition, playerYPosition, imageSizeX, imageSizeY };

	// The origin is the point the sprite rotates around. For centered rotation,
	// it's the middle of the image.
	Vector2 origin = { imageHalfSizeX, imageHalfSizeY };

	// Drawing the texture with rotation!
	DrawTexturePro(paperplane, sourceRec, destRec, origin, rotation, WHITE);
}

void DrawPng() {
	// Draw the player with rotation based on vertical speed
	DrawPlayerWithRotation();
	//alternative without rotation
	//DrawTexture(paperplane, playerXPosition - imageHalfSizeX, playerYPosition - imageHalfSizeY, WHITE);
	DrawRectangle(rectangleX, rectangleY, rectangleWidth, rectangleHeight, GREEN);
	DrawRectangle(rectangle2X, rectangle2Y, rectangle2Width, rectangle2Height, GREEN);
}
// PlayerControlls
//todo make player tilt up and down when moving

void PlayerController() {
	if (playerYPosition < 0 || playerYPosition > windowHeight)
	{
		playerYPosition = windowHalfHeight;
		playerXPosition = windowHalfWidth;

		// Reset game state to GAMESTART
		currentScreen = GAMESTART;
	}
	if (IsKeyDown(KEY_SPACE))
	{
		// Apply upward force when the space key is pressed
		playerSpeedY = jumpForce;
	}
	else if (IsKeyDown(KEY_SPACE))
	{
		// Apply upward force when the space key is pressed
		playerSpeedY -= jumpSustainForce;
	}
	// Apply gravity to the player's vertical speed
	playerSpeedY += gravity * GetFrameTime();

	//Caps terminal velocity to avoid going too fast
	if (playerSpeedY > terminalVelocity)
	{
		playerSpeedY = terminalVelocity;
	}
	// Update the player's vertical position based on speed
	playerYPosition += playerSpeedY * GetFrameTime();
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

int main()
{
	InitWindow(windowWidth, windowHeight, "My first window!!");
	SetTargetFPS(120);

	paperplane = LoadTexture("src/Resources/Paperplane.png");

	while (!WindowShouldClose()) {
		//UPDATE LOGIC--------------------------
		switch (currentScreen)
		{
		case LOGO:
		{
			// TODO: Update LOGO screen variables here!
			gameActive = false; // set gameActive to 2 when showing logo
			framesCounter++;

			// Wait for 3 seconds (360/120 frames per second) before jumping to GAMESTART screen
			if (framesCounter > 120)
			{
				currentScreen = GAMESTART;
			}
		} break;
		case GAMESTART:
		{
			// TODO: Update GAMESTART screen variables here!
			objectMovementSpeed = 0;
			playerSpeedY = 0;
			rectangleX = windowWidth;
			rectangle2X = windowWidth;

			if (IsKeyPressed(KEY_SPACE))
			{
				currentScreen = GAMEPLAY;
				objectMovementSpeed = 5.f;
				playerSpeedY = 500.f;
				Score = 0;
				gameActive = true; // set gameActive to true when game is running
			}
		} break;
		case GAMEPLAY:
		{
			// TODO: Update GAMEPLAY screen variables here!
		} break;
		default: break;
		}

		BeginDrawing();

		ClearBackground(skye);
		//DRAW LOGIC--------------------------
		switch (currentScreen)
		{
		case LOGO:
		{
			DrawCenteredText("LOGO SCREEN", (float)windowHalfHeight, 50, LIGHTGRAY);
			DrawCenteredText("WAIT for 3 SECONDS...", (float)windowHalfHeight - 100, 40, GRAY);
		} break;
		case GAMESTART:
		{
			//For testing hitbox
			//drawPlayerTest();
			//Draw GAMESTART screen here!

			DrawText(TextFormat("Score: %01i", Score), 50, 50, 25, BLACK);
			DrawText(TextFormat("Highscore: %01i", Highscore), 50, 100, 25, BLACK);

			DrawFPS(10, 10);

			Clouds();

			DrawPng();

			DrawCenteredText("FLAPPY PLAAAAANE", (float)windowHalfHeight - 100, 50, DARKGREEN);
			DrawCenteredText("PRESS SPACE/Left Click to Play", (float)windowHalfHeight - 200, 40, DARKGREEN);
		} break;
		case GAMEPLAY:
		{
			//For testing hitbox
			//drawPlayerTest();

			//Draw GAMEPLAY screen here!
			PlayerController();
			rectangleHitbox();
			rectangleHitbox2();

			collisionDetection();
			UpdateGameScore();

			DrawFPS(10, 10);

			Clouds();

			DrawPng();
			MovingCloud();

			DrawText(TextFormat("Score: %01i", Score), 50, 50, 25, BLACK);
			DrawText(TextFormat("Highscore: %01i", Highscore), 50, 100, 25, BLACK);
		} break;

		default: break;
		}
		EndDrawing();
	}
	UnloadTexture(paperplane);
	CloseWindow();

	return 0;
}
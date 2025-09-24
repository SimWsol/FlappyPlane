#include <raylib.h>
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <time.h>
#include <string>
#include <random>

// Global Variables
float windowWidth = 1280;
float windowHeight = 1024;
float windowHalfWidth = windowWidth / 2;
float windowHalfHeight = windowHeight / 2;
float windowHeight100 = windowHeight - 100;

// Global player variables
float playerXPosition = windowHalfWidth;
float playerYPosition = windowHalfHeight;

// Texture for paperplane

Texture2D paperplane;

float imageSizeX = 140;
float imageSizeY = 67;
float imageHalfSizeX = imageSizeX / 2;
float imageHalfSizeY = imageSizeY / 2;

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
//Added half speed
float playerMovementSpeedHalf = playerMovementSpeed / 2;
Color skye = { 116, 253, 255, 255 };

Vector2 myPlayerPosition{ 0,0 };

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
void DrawPng() {
	DrawTexture(paperplane, playerXPosition - imageHalfSizeX, playerYPosition - imageHalfSizeY, WHITE);
	DrawRectangle(rectangleX, rectangleY, rectangleWidth, rectangleHeight, GREEN);
	DrawRectangle(rectangle2X, rectangle2Y, rectangle2Width, rectangle2Height, GREEN);
}
//Changed PlayerControlls
void PlayerController() {
	float playerSpeed = 500.f;
	if (IsKeyDown(KEY_SPACE)) {
		playerYPosition -= playerSpeed * GetFrameTime();
		playerMovementSpeed = playerMovementSpeedHalf;
	}
	else {
		playerYPosition += (playerSpeed / 2) * GetFrameTime();
		GetApplicationDirectory();
		playerMovementSpeed = playerMovementSpeedHalf * 2;
	}
}

void drawplayer() {
	float playerSize = 33.f;
	Color playerColor = { 102, 0, 102, 255 };
	// To make the hat scale with the player model and move with playercontroller

	//DrawCircle(playerXPosition, playerYPosition, playerSize, YELLOW);
}

int main()
{
	InitWindow(windowWidth, windowHeight, "My first window!!");
	SetTargetFPS(120);

	paperplane = LoadTexture("src/Resources/Paperplane.png");

	while (!WindowShouldClose()) {
		BeginDrawing();

		ClearBackground(skye);
		PlayerController();
		myPlayerPosition = { playerXPosition,playerYPosition };
		if (CheckCollisionCircleRec(myPlayerPosition, 33.f, tryRectangle))
		{
			std::cout << "You hit";
			playerMovementSpeed = 5;
		}
		myPlayerPosition = { playerXPosition,playerYPosition };
		if (CheckCollisionCircleRec(myPlayerPosition, 33.f, tryRectangle2))
		{
			std::cout << "You hit";
			playerMovementSpeed = 5;
		}
		drawplayer();

		DrawFPS(10, 10);

		EndDrawing();

		DrawPng();

		Clouds();
		MovingCloud();
		rectangleHitbox();
		rectangleHitbox2();
	}
	UnloadTexture(paperplane);
	CloseWindow();

	return 0;
}
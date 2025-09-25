#include <raylib.h>
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <time.h>
#include <string>
#include <random>

// Global Variables
//Gamestates------------------------------------------------------
typedef enum GameScreen { LOGO = 0, GAMESTART, GAMEPLAY, GAMEOVER, PAUSED } GameScreen;
int framesCounter = 0;
GameScreen currentScreen = LOGO;

// Window variables-----------------------------------------------
float windowWidth = 1280;
float windowHeight = 1024;
float windowHalfWidth = windowWidth / 2;
float windowHalfHeight = windowHeight / 2;

// Global player variables----------------------------------------
float playerXPosition = windowHalfWidth - 300;
float playerYPosition = windowHalfHeight;
float playerSpeedY = 0;
const float playerRadius = 33.f/4;
const float gravity = 5000.f;
const float terminalVelocity = 1000.f;
const float jumpForce = -650.f;
const float jumpSustainForce = 6000.f;
// The fastest the player can move up

const float maxUpwardSpeed = -500.f; 

//Scoring---------------------------------------------------------
int Score = 0;
int Highscore = 0;
bool scored;

//Logo varriable!------------------------------------------------
float logoRotation = 0.0f;
Texture2D logoTexture;
const float LogoRotationSpeed = 180.0f;

//GameOver varriable!------------------------------------------------
double gameOverTimer = 0.0;
// Delay in seconds before allowing restart
const float RestartDelay = 0.5f; 

// Textures -----------------------------------------
Texture2D characterTexture;
Texture2D cloudTexture;
Texture2D paperplaneTexture;
Texture2D jetplaneTexture;
Texture2D backgroundTexture;
Texture2D topHindranceTexture;
Texture2D bottomHindranceTexture;

//moving background variables-----------------------------------------------
float scrollingBackX = 0.0f;

//amount of planes----------------------------------------------
const int TotalPlanes = 2;
int planeChoice = 0;

//Plane speeds-----------------------------------------------	
const float paperplaneSpeed = 5.f;
const float jetplaneSpeed = 8.f;

//To be removed?? image size variables-------------------------------
// image size variables currently used in player drawing-------------------------------
float imageSizeX = 140/2;
float imageSizeY = 67/2;
float imageHalfSizeX = imageSizeX / 2;
float imageHalfSizeY = imageSizeY / 2;

//cloud variables------------------------------------------------
struct Cloud
{
	Vector2 position;
	float speed;
	float scale;
	};

const int maxClouds = 6;
Cloud clouds[maxClouds];

//Init function for clouds
void InitClouds(void) 
{
	for (int i = 0; i < maxClouds; i++)
	{
		//Give Each cloud a random starting position, speed, and size
		clouds[i].position.x = GetRandomValue(0, windowWidth);
		clouds[i].position.y = GetRandomValue(50, windowHeight - 200);
		clouds[i].speed = GetRandomValue(20, 80);
		clouds[i].scale = GetRandomValue(5, 15) / 10.0f;
	}
}
//Cloud update function
void UpdateCloud(Cloud& cloud)
{
	cloud.position.x -= cloud.speed * GetFrameTime();
	// If the cloud has moved off-screen to the left, reset its position to the right
	if (cloud.position.x < -(cloudTexture.width * cloud.scale))
	{
		cloud.position.x = windowWidth;
		cloud.position.y = GetRandomValue(50, windowHeight - 200);
		cloud.speed = GetRandomValue(20, 80);
		cloud.scale = GetRandomValue(5, 15) / 10.0f;
	}
}

//Cloud draw function
void DrawCloud(const Cloud& cloud)
{
	DrawTextureEx(cloudTexture, cloud.position, 0.0f, cloud.scale, WHITE);
}

//universal speed of objects------------------------------------------------
float objectMovementSpeed = 5.f;

//Background color in game------------------------------------------------
Color skye = { 116, 253, 255, 255 };

Vector2 myPlayerPosition{ 0,0 };



//rectangle variables-------------------------------------------------
struct Hindrance {
	Rectangle topRect;
	Rectangle bottomRect;
	bool scored;
	};

const int maxHindrances = 2;
const float hindranceSpacing = 800.f;
Hindrance hindrances[maxHindrances];

// Initialize hindrances with positions off-screen to the right
void initHindrances(void)
{
	for (int i = 0; i < maxHindrances; i++)
	{
		// Set top rectangle position and size
		hindrances[i].topRect.width = 200.f;
		hindrances[i].topRect.height = 400;

		// Position them off-screen to the right initially
		hindrances[i].topRect.x = windowWidth + (i * hindranceSpacing);
		hindrances[i].topRect.y = 0.f;

		// Set bottom rectangle position and size based on top rectangle
		hindrances[i].bottomRect.width = 200.f;
		hindrances[i].bottomRect.height = 424;

		// Position them off-screen to the right initially
		hindrances[i].bottomRect.x = windowWidth + (i * hindranceSpacing);
		hindrances[i].bottomRect.y = 600;

		// Initialize scored flag to false
		hindrances[i].scored = false;
	}
}

//Helper functions--------------------------------------------------------
void ResetGame()
{
	playerXPosition = windowHalfWidth - 300;
	playerYPosition = windowHalfHeight;
	playerSpeedY = 0.0f;
	Score = 0;
	initHindrances();
	InitClouds();
}

void updateHindrances(Hindrance& hindrance)
{
	// Move the hindrance left based on object movement speed
	hindrance.topRect.x -= objectMovementSpeed;
	hindrance.bottomRect.x -= objectMovementSpeed;

	// If the hindrance has moved off-screen to the left, reset its position
	if (hindrance.topRect.x + hindrance.topRect.width < 0)
	{
		// Reset X position to the right side of the screen behind the last hindrance
		hindrance.topRect.x += maxHindrances * hindranceSpacing;
		hindrance.bottomRect.x += maxHindrances * hindranceSpacing;

		// Randomize the height of the top rectangle
		// Minimum gap between top and bottom rectangles (higher number means easier game)
		float gap = 250.0f; 
		float randomHeight = GetRandomValue(200, windowHeight - gap - 200);

		// Set the bottom rectangle's height and position based on the top rectangle
		hindrance.bottomRect.height = randomHeight;
		hindrance.bottomRect.y = windowHeight - randomHeight;

		// Set the top rectangle's height and position
		hindrance.topRect.height = windowHeight - randomHeight - gap;
		hindrance.topRect.y = 0;

		// Reset scored flag
		hindrance.scored = false;
	}
}
// Draw the hindrance rectangles
void DrawHindrance(const Hindrance& hindrance)
{
	// Draw the top texture (construction truss)
	DrawTexturePro(topHindranceTexture,	{ 0, 0, (float)topHindranceTexture.width, (float)topHindranceTexture.height }, 	hindrance.topRect,{ 0, 0 }, 0.0f, WHITE);

	// Draw the bottom texture (broken building)
	DrawTexturePro(bottomHindranceTexture,{ 0, 0, (float)bottomHindranceTexture.width, (float)bottomHindranceTexture.height },  hindrance.bottomRect,{ 0, 0 }, 0.0f, WHITE);
}

void HandleCharacterSelection()
{
	// Cycle through character choices when 'X' is pressed
	if (IsKeyPressed(KEY_X))
	{
		planeChoice = (planeChoice + 1) % TotalPlanes;
	}
	// Update the character texture based on the current choice
	switch (planeChoice)
	{
	case 0:
		characterTexture = paperplaneTexture;
		break;
	case 1:
		characterTexture = jetplaneTexture;
		break;
	default:
		characterTexture = paperplaneTexture;
		break;
	}
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

//CLOUDS-------------------------------------------------------------
//TODO Needs to be optimized and made into a class later

void collisionDetection()
{
	myPlayerPosition = { playerXPosition,playerYPosition };
	for (int i = 0; i < maxHindrances; i++)
	{
		if (CheckCollisionCircleRec(myPlayerPosition, playerRadius, hindrances[i].topRect) || CheckCollisionCircleRec(myPlayerPosition, playerRadius, hindrances[i].bottomRect))
		{
			currentScreen = GAMEOVER;
			gameOverTimer = GetTime();
		}
	}
}

void DrawPlayerWithRotation(Texture2D characterTexture) {
	// Calculate a rotation angle based on vertical velocity.
	// We multiply by a small factor to make the rotation look good.
	// We also clamp it so it doesn't spin too far.
	float rotation = playerSpeedY * 0.025f;
	if (rotation > 45.0f) rotation = 60.0f;   // Clamp max downward angle
	if (rotation < -30.0f) rotation = -30.0f; // Clamp max upward angle

	// Define the how big the texture is and attach it to the rectangle
	Rectangle sourceRec = { 0.0f, 0.0f, (float)characterTexture.width, (float)characterTexture.height };
	Rectangle destRec = { playerXPosition, playerYPosition, imageSizeX, imageSizeY };

	// The origin is the point the sprite rotates around. For centered rotation,
	// it's the middle of the image.
	Vector2 origin = { imageHalfSizeX, imageHalfSizeY };

	// Drawing the texture with rotation!
	DrawTexturePro(characterTexture, sourceRec, destRec, origin, rotation, WHITE);
}

void drawGameplayScene() {
	//Draw Background stuff
	ClearBackground(skye);

	//draw clouds
	for (int i = 0; i < maxClouds; i++)
	{
		DrawCloud(clouds[i]);
	}

	//Draw the background to fill the screen
	DrawTexture(backgroundTexture, scrollingBackX, 0, WHITE);
	DrawTexture(backgroundTexture, scrollingBackX + backgroundTexture.width, 0, WHITE);
	
	

	// Draw the player with rotation based on vertical speed
	DrawPlayerWithRotation(characterTexture);

	//Draww obstacles
	for (int i = 0; i < maxHindrances; i++)
	{
		DrawHindrance(hindrances[i]);
	}

	//Draw UI stuff
	DrawText(TextFormat("Score: %01i", Score), 50, 50, 25, BLACK);
	DrawText(TextFormat("Highscore: %01i", Highscore), 50, 100, 25, BLACK);
	DrawFPS(10, 10);

	}


// PlayerControlls
void PlayerController() {
	// Reset player position and game state if player goes out of bounds
	if (playerYPosition < 0 || playerYPosition > windowHeight)
	{
		// Reset game state to GAMESTART
		currentScreen = GAMEOVER;
		gameOverTimer = GetTime();
	}
	if (IsKeyPressed(KEY_SPACE))
	{
		// Apply upward force when the space key is pressed
		playerSpeedY = jumpForce;
	}
	else if (IsKeyDown(KEY_SPACE))
	{
		// Apply upward force when the space key is pressed
		playerSpeedY -= jumpSustainForce * GetFrameTime();
	}
	// Apply gravity to the player's vertical speed
	playerSpeedY += gravity * GetFrameTime();

	//Caps terminal velocity to avoid going too fast
	if (playerSpeedY > terminalVelocity)
	{
		playerSpeedY = terminalVelocity;
	}
	//Cap the maximum upward speed to avoid going too fast upwards
	if (playerSpeedY < maxUpwardSpeed)
	{
		playerSpeedY = maxUpwardSpeed;
	}
	// Update the player's vertical position based on speed
	playerYPosition += playerSpeedY * GetFrameTime();
}

void UpdateGameScore()
{
	for (int i = 0; i < maxHindrances; i++)
	{
		// Check if the player has passed the hindrance and hasn't scored for it yet
		if (!hindrances[i].scored && hindrances[i].topRect.x < playerXPosition)
		{
			Score++;
			hindrances[i].scored = true;
		}
	}
	if (Score > Highscore) Highscore = Score;
}

int main()
{
	InitWindow(windowWidth, windowHeight, "My first window!!");
	SetTargetFPS(120);
	initHindrances();
	InitClouds();

	//Load textures
	paperplaneTexture = LoadTexture("src/Resources/Paperplane.png");
	jetplaneTexture = LoadTexture("src/Resources/JetPlane.png");
	logoTexture = LoadTexture("src/Resources/TeamLogo.png");
	cloudTexture = LoadTexture("src/Resources/Cloud.png");
	backgroundTexture = LoadTexture("src/Resources/Cityscape.png");
	topHindranceTexture = LoadTexture("src/Resources/ConstructionTrust.png");
	bottomHindranceTexture = LoadTexture("src/Resources/BrokenBuilding2.png");


	while (!WindowShouldClose()) {
		//UPDATE LOGIC--------------------------
		//The brains and meat of the operation!
		switch (currentScreen)
		{
		case LOGO:
		{
			//Count frames for 3 seconds
			framesCounter++;

			//Update the rotation angle every frame to spin it
			logoRotation += LogoRotationSpeed * GetFrameTime();

			// Press X to change character to jetplane or other planes later
			HandleCharacterSelection();

			// Wait for 3 seconds (360/120 frames per second) before jumping to GAMESTART screen
			if (framesCounter > 360 || IsKeyPressed(KEY_SPACE))
			{
				ResetGame();
				currentScreen = GAMESTART;
			}
		} break;

		case GAMESTART:
		{
			//reset player position and speed of objects
			objectMovementSpeed = 0;
			HandleCharacterSelection();
			if (IsKeyPressed(KEY_SPACE))
			{
				// Set object movement speed based on selected plane
				switch (planeChoice)
				{
				case 0: // Paperplane
					objectMovementSpeed = paperplaneSpeed; break;
				case 1: // Jetplane
					objectMovementSpeed = jetplaneSpeed; break;
				}
				currentScreen = GAMEPLAY;
			}
		} break;

		case GAMEPLAY:
		{
			if (IsKeyPressed(KEY_P))
			{
				currentScreen = PAUSED;
			}
			PlayerController();

			for (int i = 0; i < maxHindrances; i++)
			{
				updateHindrances(hindrances[i]);
			}

			collisionDetection();
			UpdateGameScore();
			//Update clouds
			for (int i = 0; i < maxClouds; i++)
			{
				UpdateCloud(clouds[i]);
			}
			//Scrolls background for paralax effect
			scrollingBackX -= (objectMovementSpeed / 2.0f);
			if (scrollingBackX <= -backgroundTexture.width) scrollingBackX = 0;
			
		} break;

		case GAMEOVER:
		{
			// Stop all movement
			objectMovementSpeed = 0;

			// Wait for the player to press SPACE to restart
			if (IsKeyPressed(KEY_SPACE)&& (GetTime() - gameOverTimer > RestartDelay))
			{
				ResetGame();
				currentScreen = GAMESTART;
			}
		} break;

		case PAUSED:
		{
			if (IsKeyPressed(KEY_P))
			{
				currentScreen = GAMEPLAY;
			}
		} break;

		default: break;
		}

		BeginDrawing();

		//DRAW LOGIC--------------------------
		//Everything visual goes into this "EYES" section
		switch (currentScreen)
		{
		case LOGO:
		{
			//DrawPlayerWithRotation(characterTexture);
			ClearBackground(WHITE);
			// set size of logo
			float logoWidth = 256;
			float logoHeight = 256;

			// Define the source and destination rectangles for drawing
			Rectangle sourceRec = { 0.0f, 0.0f, (float)logoTexture.width, (float)logoTexture.height };
			Rectangle destRec = { windowHalfWidth, windowHalfHeight, logoWidth, logoHeight };

			// Center origin for proper rotation
			Vector2 origin = { logoWidth / 2.0f, logoHeight / 2.0f };

			//draw logo with rotation
			DrawTexturePro(logoTexture, sourceRec, destRec, origin, logoRotation, WHITE);

			//Logo text
			DrawCenteredText("FLAPPY PLAAAAANE", (float)windowHalfHeight-300, 100, BLACK);
		} break;
		case GAMESTART:
		{
			//Draws scene
			drawGameplayScene();

			DrawCenteredText("FLAPPY PLAAAAANE", (float)windowHalfHeight - 100, 50, DARKGREEN);
			DrawCenteredText("PRESS SPACE to play", (float)windowHalfHeight - 200, 40, DARKGREEN);
			DrawCenteredText("PRESS X TO SWAP TO JETPLANE!", (float)windowHalfHeight - 300, 40, DARKGREEN);
		} break;
		case GAMEPLAY:
		{
			//Draw the gameplay screen
			drawGameplayScene();

		} break;
		case GAMEOVER:
		{
			//  Draw the frozen game scene ----------------------------
			drawGameplayScene(); 
			
			// Draw the "Game Over" overlay -----------------
			// Darken the screen
			DrawRectangle(0, 0, windowWidth, windowHeight, { 0, 0, 0, 150 }); 
			//Draw game over text
			DrawCenteredText("GAME OVER", windowHalfHeight - 40, 80, RED);
			DrawCenteredText("Press SPACE to Restart", windowHalfHeight + 40, 20, WHITE);
			DrawCenteredText(TextFormat("Your High score was: %i", Highscore), windowHalfHeight + 80, 20, WHITE);


		} break;
		case PAUSED:
		{
			//Draw the paused game scene ----------------------------
			drawGameplayScene(); 

			// Overlay a semi-transparent layer to indicate pause
			DrawRectangle(0, 0, windowWidth, windowHeight, { 0, 0, 0, 100 });
			DrawCenteredText("PAUSED", windowHalfHeight - 20, 80, YELLOW);
			DrawCenteredText("Press P to Resume", windowHalfHeight + 40, 20, WHITE);
		} break;
		default: break;
		}
		EndDrawing();
	}
	// Unload textures
	UnloadTexture(paperplaneTexture);
	UnloadTexture(jetplaneTexture);
	UnloadTexture(backgroundTexture);
	UnloadTexture(logoTexture);
	UnloadTexture(cloudTexture);
	UnloadTexture(topHindranceTexture);
	UnloadTexture(bottomHindranceTexture);

	CloseWindow();

	return 0;
}
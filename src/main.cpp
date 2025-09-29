#include <raylib.h>
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <time.h>
#include <string>
#include <random>

/*
README-------------------------------------
Team roles: 
Noah: Group lead, basic game mechanics, basic game design,
Simon: Main programmer, advanced game mechanics, advanced game design,
Alvin: Help with programming, scoreboards, file handling, Calculations
Oskar: Lead designer, graphics, 2D art, Audio design
(Note, any team member can help with any task, but these are the main roles)
___________________________________________________
To do list Noah: 23/09
Add basic object system:
Add basic spawn and despawn system for hindrance objects
rectangles that move left across the screen, despawn when off screen
random height of base, identical height increase (gap) between top and bottom
top and bottom rectangles are tall enough so player cannot go "above"
Fixed width of rectangles

Controls:
Space to increase height of player, gravity based.
P to pause game
X to change plane type


current Game overview list-----------------------------------------------------------------------
Gamestart state, Space to start, X to change plane type
High score display top left screen

Main game:
Player moves only Y axis
Player rotates slightly when moving up or down
Player has multiple ships with different speeds to choose from
Gravity pulls player down when not pressing space

Objects move only X axis
Objects spawn on right side of screen and despawn on left side of screen
objects spawn at random heights but fixed distance between top and bottom and fixed width
objects spawn are tall enough to leave a gap for the player to pass through

Touching the "ground" or "floor" ends the game
Touching an object ends the game
Score increases by 1 for each object that passes the player

Background: exchangable background image 
Clouds that move slowly across the screen to indicate speed(parallax effect)
Perhaps ground or floor at the bottom of the screen moves slightly to indicate speed(parallax effect)

Game over screen with score and high score:
Pause Menu with Pause text
Space to restart
Game overview end-------------------------------------------------------------------

TO DO LATER (if time):
Noah 25/09 Planned:
Music: (Background music, menu music)
Sound effects: game over(wumpwump), score increase (Ding), height increase sound (Swosh)

Add features:
Powerups?(NOT YET Different object types(ADDED system, still just one style) Make theme system.
Add more menus: Settings menu? (Not yet, add theme picker for different backgrounds or textures) 

Advanced graphics: animated player? animated objects? animated background?

Scoreboards: 
file handling to save high scores to a file ?

END README------------------------------
---------------------------------------------
*/

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
Texture2D cloudTexture;
Texture2D backgroundTexture;
//plane textures-----------------------------------------------
Texture2D characterTexture;
Texture2D paperplaneTexture;
Texture2D jetplaneTexture;
//Hindrance textures-----------------------------------------------
Texture2D topHindranceTexture;
Texture2D bottomHindranceTexture;

//Audio files-----------------------------------------------
#define PlaneJumpSoundsMax 10
// Sounds for the paper plane
Sound paperplaneJumpSound;
Sound paperplaneSounds[PlaneJumpSoundsMax] = { 0 };
// Sounds for the jet plane
Sound jetplaneThrustSound;
Sound jetplaneSounds[PlaneJumpSoundsMax] = { 0 };
int currentSound;

//Background music
Music backgroundMusic;
Music backgroundMusic2;
Music ChosenBackgroundMusic;



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



//Hindrance variables-------------------------------------------------
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
void drawPlayerTest() {
	//Draws a Circle for testing hitbox (easier to see than the plane rectangle, as the actual hitbox is a circle)
	Color playerColor = BLACK;
	DrawCircle(myPlayerPosition.x, myPlayerPosition.y, playerRadius, YELLOW);
}

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
		if (planeChoice == 0) {
			// Change background music to match jetplane theme
			StopMusicStream(ChosenBackgroundMusic);
			ChosenBackgroundMusic = backgroundMusic;
			PlayMusicStream(ChosenBackgroundMusic);
		}
		else if (planeChoice == 1) {
			// Change background music back to paperplane theme
			StopMusicStream(ChosenBackgroundMusic);
			ChosenBackgroundMusic = backgroundMusic2;
			PlayMusicStream(ChosenBackgroundMusic);
		}
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
		// Play jump sound depending on the plane type usign switch
		//plays an array of 10 sound slots of the same sound to avoid cutting off, loops at end of array
		switch (planeChoice)
		{
		case 0: // Paperplane
			PlaySound(paperplaneSounds[currentSound]);
			break;
		case 1: // Jetplane
			PlaySound(jetplaneSounds[currentSound]);
			break;
		}
		currentSound++;                        
		if (currentSound >= PlaneJumpSoundsMax) currentSound = 0;

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
	InitAudioDevice();
	SetTargetFPS(120);
	initHindrances();
	InitClouds();

	//Load Music
	// .mp3, .ogg, .wav, or .flac accepted
	backgroundMusic = LoadMusicStream("src/resources/BackgroundSong2.mp3");
	backgroundMusic2 = LoadMusicStream("src/resources/BackgroundSong.mp3");
	ChosenBackgroundMusic = backgroundMusic;
	PlayMusicStream(backgroundMusic);


	// Load sound aliases into the array
	paperplaneJumpSound = LoadSound("src/resources/SwoshSound.wav");
	jetplaneThrustSound = LoadSound("src/resources/JetPlaneSwosh.wav");
	
	// Load an alias of the sound into slots 1-9. These do not own the sound data, but can be played
	for (int i = 1; i < PlaneJumpSoundsMax; i++)
	{
		jetplaneSounds[i] = LoadSoundAlias(jetplaneThrustSound);
		paperplaneSounds[i] = LoadSoundAlias(paperplaneJumpSound);
	}
		currentSound = 0;


	//Load textures
	paperplaneTexture = LoadTexture("src/Resources/Paperplane.png");
	jetplaneTexture = LoadTexture("src/Resources/JetPlane.png");
	logoTexture = LoadTexture("src/Resources/TeamLogo.png");
	cloudTexture = LoadTexture("src/Resources/Cloud.png");
	backgroundTexture = LoadTexture("src/Resources/Cityscape.png");
	topHindranceTexture = LoadTexture("src/Resources/ConstructionTrust.png");
	bottomHindranceTexture = LoadTexture("src/Resources/BrokenBuilding2.png");


	while (!WindowShouldClose()) {
		//Background music update
		UpdateMusicStream(ChosenBackgroundMusic);
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
			//Make sure background music is playing
			ResumeMusicStream(ChosenBackgroundMusic);

			//Pausing on P input
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
			// Pause the background music
			PauseMusicStream(ChosenBackgroundMusic);

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

	// Close audio device and unload sound aliases and background music
	for (int i = 0; i < PlaneJumpSoundsMax; i++) 
	{
		UnloadSoundAlias(paperplaneSounds[i]);
		UnloadSoundAlias(jetplaneSounds[i]);
	}

	UnloadSound(paperplaneJumpSound);
	UnloadSound(jetplaneThrustSound);
	UnloadMusicStream(backgroundMusic);
	UnloadMusicStream(backgroundMusic2);

	CloseAudioDevice();


	CloseWindow();

	return 0;
}
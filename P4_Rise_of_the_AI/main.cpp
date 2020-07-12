/*
Luke Koppenheffer
Project 4 - Rise of the AI
main.cpp
*/


#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"

#define PLATFORM_COUNT 11
#define ENEMY_COUNT 3
struct GameState {
	Entity* player;
	Entity* platforms;
	Entity* enemies;
	// Decide whether to display the winning text or the losing text depending on the conditions
	Entity* text;
	// Keep track of when the game ends (either player defeated all the enemies or the player died)
	bool gameOver;
	int totEnemies;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint LoadTexture(const char* filePath) {
	int w, h, n;
	unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

	if (image == NULL) {
		std::cout << "Unable to load image. Make sure the path is correct\n";
		assert(false);
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(image);
	return textureID;
}


void Initialize() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("AI!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 640, 480);

	program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

	viewMatrix = glm::mat4(1.0f);
	modelMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);

	glUseProgram(program.programID);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// Initialize Game Objects

	// track how many enemies player has defeated
	state.totEnemies = 0;
	state.gameOver = false;

	state.text = new Entity();
	state.text->textureID = LoadTexture("font1.png");

	// Initialize Player
	state.player = new Entity();
	// Setting entity Type so we can know what type of object this is when we check for collisions
	state.player->entityType = PLAYER;
	state.player->position = glm::vec3(-4, -1, 0);
	state.player->movement = glm::vec3(0);
	// Setting acceleration to gravity
	state.player->acceleration = glm::vec3(0, -19.62f, 0);
	state.player->jumpPower = 8.0f;
	state.player->speed = 3.0f;
	state.player->textureID = LoadTexture("george_0.png");

	state.player->animRight = new int[4]{ 3, 7, 11, 15 };
	state.player->animLeft = new int[4]{ 1, 5, 9, 13 };
	state.player->animUp = new int[4]{ 2, 6, 10, 14 };
	state.player->animDown = new int[4]{ 0, 4, 8, 12 };

	state.player->animIndices = state.player->animRight;
	state.player->animFrames = 4;
	state.player->animIndex = 0;
	state.player->animTime = 0;
	state.player->animCols = 4;
	state.player->animRows = 4;

	// When we checked collisions prior, the sprite appeared to be floating because the sprite actually had a bit of a border around it
	// We fix this by setting the size of the player sprite
	state.player->height = 0.8f;
	state.player->width = 0.5f;

	

	// Initialize platforms
	state.platforms = new Entity[PLATFORM_COUNT];
	// Setting entity Type so we can know what type of object this is when we check for collisions
	state.platforms->entityType = PLATFORM;
	GLuint platformTextureID = LoadTexture("platformPack_tile001.png");

	for (int i = 0; i < PLATFORM_COUNT; i++) {
		state.platforms[i].textureID = platformTextureID;
		state.platforms[i].position = glm::vec3(-5 + i, -3.25f, 0);
	}

	for (int i = 0; i < PLATFORM_COUNT; i++) {
		// Update the platforms one time so that their model matrix will update
		state.platforms[i].Update(0, NULL, NULL, NULL, 0, 0);
	}

	//Enemy Initialization
	state.enemies = new Entity[ENEMY_COUNT];
	// Add more of a buffer for enemy collisions to feel more fair to the player
	state.enemies->width = 0.8f;
	GLuint enemyTextureID = LoadTexture("ctg.png");

	// Setting entity Type so we can know what type of object this is when we check for collisions
	state.enemies[0].entityType = ENEMY;
	// Specifying the type of AI character this entity is
	state.enemies[0].aiType = WAITANDGO;
	// Specifying the state that the AI is in. Is it walking, idle, attacking, etc.
	state.enemies[0].aiState = IDLE;
	state.enemies[0].textureID = enemyTextureID;
	state.enemies[0].position = glm::vec3(4, -2.0f, 0);
	state.enemies[0].speed = 1;
	// Apply gravity to the AI
	state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);

	// Jumper Enemy
	state.enemies[1].entityType = ENEMY;
	state.enemies[1].aiType = JUMPER;
	state.enemies[1].textureID = enemyTextureID;
	state.enemies[1].position = glm::vec3(0, -2.0f, 0);
	state.enemies[1].jumpPower = 2;
	state.enemies[1].acceleration = glm::vec3(0, -4.9f, 0);

	// Patroller Enemy
	state.enemies[2].entityType = ENEMY;
	state.enemies[2].aiType = PATROL;
	state.enemies[2].textureID = enemyTextureID;
	state.enemies[2].position = glm::vec3(5, -2.0f, 0);
	state.enemies[2].speed = 1;
	state.enemies[2].acceleration = glm::vec3(0, -4.9f, 0);
}

void ProcessInput() {

	state.player->movement = glm::vec3(0);

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
		case SDL_WINDOWEVENT_CLOSE:
			gameIsRunning = false;
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {

			case SDLK_SPACE:
				// Jump (note we only check for a single press of the button)
				if (state.player->collidedBottom) {
					// Can only jump if the player is colliding with something below them
					state.player->jump = true;
				}
				break;
			}
			break; // SDL_KEYDOWN
		}
	}

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	// Don't process input on game end
	if (!state.gameOver) {
		if (keys[SDL_SCANCODE_LEFT]) {
			state.player->movement.x = -1.0f;
			state.player->animIndices = state.player->animLeft;
		}
		else if (keys[SDL_SCANCODE_RIGHT]) {
			state.player->movement.x = 1.0f;
			state.player->animIndices = state.player->animRight;
		}

		if (glm::length(state.player->movement) > 1.0f) {
			state.player->movement = glm::normalize(state.player->movement);
		}
	}

}

/*-----FIXED TIMESTEP CODE-----*/
#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;
void Update() {

	// Only update movements if the game isn't over
	if (!state.gameOver) {

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float deltaTime = ticks - lastTicks;
		lastTicks = ticks;
		deltaTime += accumulator;
		if (deltaTime < FIXED_TIMESTEP) {
			accumulator = deltaTime;
			return;
		}
		while (deltaTime >= FIXED_TIMESTEP) {
			// Update. Notice it's FIXED_TIMESTEP. Not deltaTime
			// Updating Player and passing in the player entity, the platforms, and enemies which we check collision with
			state.player->Update(FIXED_TIMESTEP, state.player, state.platforms, state.enemies, PLATFORM_COUNT, ENEMY_COUNT);

			for (int i = 0; i < ENEMY_COUNT; i++) {
				// Updating Enemies and passing in the player entity, the platforms, and enemies which we check collision with
				state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.platforms, state.enemies, PLATFORM_COUNT, ENEMY_COUNT);
			}

			// Have enemy jump everytime he's on the floor
			if (state.enemies[1].collidedBottom) {
				// Can only jump if the enemy is colliding with something below them
				state.enemies[1].jump = true;
			}

			// If we have enough time to do another update, we're telling the player to Update
			deltaTime -= FIXED_TIMESTEP;

			// We're updating a consistent amount over time
		}
		accumulator = deltaTime;

	}

	// If player collides with an enemy who is NOT dead
	if (state.player->enemyCollidedWith != nullptr && !state.player->enemyCollidedWith->dead && state.player->lastCollision == ENEMY) {
		// If the player jumped and killed an enemy, remove that enemy
		if (state.player->collidedBottom && state.player->velocity.y < 0) {
			// Set the enemy we collide with to dead, do not render it
			state.player->enemyCollidedWith->dead = true;

		}
		// If player collided with an enemy without jumping on them
		else if (state.player->collidedLeft || state.player->collidedRight || state.player->collidedTop) {
			// Player did not jump on the enemy, game over
			state.player->dead = true;
		}

	}

	// Check for how many alive enemies there are
	for (int i = 0; i < ENEMY_COUNT; i++) {
		if (state.enemies[i].dead) {
			state.totEnemies += 1;
		}
	}
	// if there are some enemies left, reset the counter and check again
	if (state.totEnemies < ENEMY_COUNT) {
		state.totEnemies = 0;
	}

	if (state.totEnemies == ENEMY_COUNT) {
		// Player won, display victory text!
		state.text->endGameText = "You win!";
		state.gameOver = true;

	}
	if (state.player->dead) {
		// Player got hit
		state.text->endGameText = "You lose!";
		state.gameOver = true;

	}

}


void Render() {
	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < PLATFORM_COUNT; i++) {
		state.platforms[i].Render(&program);
	}

	for (int i = 0; i < ENEMY_COUNT; i++) {
		// Only draw the enemy if it hasn't been killed by the player
		if (!state.enemies[i].dead) {
			state.enemies[i].Render(&program);
		}
	}

	state.player->Render(&program);

	// If either end game condition is met, display the ending text
	if (state.gameOver) {
		state.text->DrawText(&program, state.text->textureID, state.text->endGameText, 0.5f, -0.25f, glm::vec3(-1.5f, -0.5f, 0));
	}

	SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
	SDL_Quit();
}

int main(int argc, char* argv[]) {
	Initialize();

	while (gameIsRunning) {
		ProcessInput();
		Update();
		Render();
	}

	Shutdown();
	return 0;
}

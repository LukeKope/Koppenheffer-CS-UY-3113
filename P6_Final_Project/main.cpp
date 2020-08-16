/*
Luke Koppenheffer
Final Project - JRPG
main.cpp
*/


#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Util.h"
#include "Effects.h"
#include "Entity.h"
#include "Map.h"
// Including the levels
#include "Scene.h"
#include "Menu.h"
#include "Level1.h"
#include "Battle.h"
#include "Win_Screen.h"
#include "Lose_Screen.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Scene* currentScene;
// Hold an array of levels and we can point to the current level
Scene* sceneList[5];

// Sound effects
Mix_Chunk* player_attack;
Mix_Chunk* fireball;

Effects* effects;


struct GlobalGameState {
	Entity* player;
	// Keep track of all the enemies in the overworld
	// Pass this vector into the level1 and battle files
	Entity* enemies;
	Entity* lastCollision;
	bool playerWins = false;
	// On initialization, set playerPosition to center screen
	glm::vec3 playerPosition = glm::vec3(11, -8, 0);
	int enemyCount = 4;
	// Track which enemy the player enters battle with so we can update dead to true for that enemy
	int enemyBattling = -1;
	float* player_health = new float(210.0f);
};

GlobalGameState globalState;

void SwitchToScene(Scene* scene) {
	currentScene = scene;
	currentScene->Initialize(globalState.player_health, globalState.enemies, globalState.enemyBattling, globalState.playerPosition);
}

void Initialize() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Slime Hunter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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

	GLuint enemyTextureID = Util::LoadTexture("slime.png");
	GLuint bossEnemyTextureID = Util::LoadTexture("boss_slime.png");

	// Initialize the global enemies
	globalState.enemies = new Entity[globalState.enemyCount];

// Enemy 1
	// Setting entity Type so we can know what type of object this is when we check for collisions
	globalState.enemies[0].entityType = ENEMY;
	// Specifying the type of AI character this entity is
	globalState.enemies[0].aiType = WALKER;
	// Specifying the globalState that the AI is in. Is it walking, idle, attacking, etc.
	globalState.enemies[0].aiState = IDLE;
	globalState.enemies[0].textureID = enemyTextureID;
	globalState.enemies[0].position = glm::vec3(5, -2, 0);
	globalState.enemies[0].speed = 1;
	globalState.enemies[0].health = new float(100.0f);
	// name, mp, damage
	Abilities Punch = Abilities("Punch", 10, 10);
	Abilities Scratch = Abilities("Scratch", 10, 5);

	globalState.enemies[0].moveset.push_back(Punch);
	globalState.enemies[0].moveset.push_back(Scratch);

// Enemy 2
	globalState.enemies[1].entityType = ENEMY;
	globalState.enemies[1].aiType = WAITANDGO;
	globalState.enemies[1].aiState = IDLE;
	globalState.enemies[1].textureID = enemyTextureID;
	globalState.enemies[1].position = glm::vec3(15, -5, 0);
	globalState.enemies[1].speed = 1;
	globalState.enemies[1].health = new float(100.0f);
	// Giving enemy own moveset
	globalState.enemies[1].moveset.push_back(Punch);
	globalState.enemies[1].moveset.push_back(Scratch);

// Enemy 3
	globalState.enemies[2].entityType = ENEMY;
	globalState.enemies[2].aiType = WAITANDGO;
	globalState.enemies[2].aiState = IDLE;
	globalState.enemies[2].textureID = enemyTextureID;
	globalState.enemies[2].position = glm::vec3(20, -11, 0);
	globalState.enemies[2].speed = 1;
	globalState.enemies[2].health = new float(100.0f);
	// Giving enemy own moveset
	globalState.enemies[2].moveset.push_back(Punch);
	globalState.enemies[2].moveset.push_back(Scratch);

// Boss (Stays still)
	globalState.enemies[3].entityType = ENEMY;
	globalState.enemies[3].aiState = IDLE;
	globalState.enemies[3].textureID = bossEnemyTextureID;
	globalState.enemies[3].position = glm::vec3(3, -14, 0);
	globalState.enemies[3].speed = 1;
	globalState.enemies[3].health = new float(200.0f);
	// Giving enemy own moveset
	globalState.enemies[3].moveset.push_back(Punch);
	globalState.enemies[3].moveset.push_back(Scratch);

	// Initializing our levels and starting at level 1
	sceneList[0] = new Menu();
	sceneList[1] = new Level1();
	sceneList[2] = new Battle();
	sceneList[3] = new Win_Screen();
	sceneList[4] = new Lose_Screen();
	SwitchToScene(sceneList[0]);

	// Loading sound
	player_attack = Mix_LoadWAV("hit_sound.wav");
	fireball = Mix_LoadWAV("fireball.wav");

	// Passing in the projection and view matrixes so that it knows the dimension of the window
	effects = new Effects(projectionMatrix, viewMatrix);

	// Starting the fade in effect immediately
	effects->Start(FADEIN, 0.2f);
}

void ProcessInput() {
	// Have checks in place to not do anything with the player entity in the menu because the player entity does not exist in the menu
	if (currentScene != sceneList[0] && currentScene != sceneList[3] && currentScene != sceneList[4]) {
		currentScene->state.player->movement = glm::vec3(0);
	}
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
		case SDL_WINDOWEVENT_CLOSE:
			gameIsRunning = false;
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
				// Allow player to advance to the first level from the menu
			case SDLK_RETURN:
				if (currentScene == sceneList[0]) {
					// This sends a notice to main that we want to change levels to level 1
					currentScene->state.nextScene = 1;
				}
				break;

				
			case SDLK_1:
				// Do move 1
				// FOR THE BATTLE STAGE, ALLOW PLAYER TO SELECT THEIR MOVES
				if (currentScene == sceneList[2]) {
					currentScene->state.player->currMove = 0;
					// Play sound effect for fireball											
					Mix_PlayChannel(-1, fireball, 0);
				}
				
				break;
			case SDLK_2:
				// Do move 2
				// FOR THE BATTLE STAGE, ALLOW PLAYER TO SELECT THEIR MOVES
				if (currentScene == sceneList[2]) {
					currentScene->state.player->currMove = 1;
					Mix_PlayChannel(-1, player_attack, 1);
				}
				
				break;
			case SDLK_3:
				// Do move 3
				// FOR THE BATTLE STAGE, ALLOW PLAYER TO SELECT THEIR MOVES
				if (currentScene == sceneList[2]) {
					currentScene->state.player->currMove = 2;
					// Play sound effect for attack
					Mix_PlayChannel(-1, player_attack, 0);
				}
				
				break;
			
			}
		}
		break; // SDL_KEYDOWN
	}


	const Uint8* keys = SDL_GetKeyboardState(NULL);

	// Allow movement on the overworld screen
	if (currentScene == sceneList[1]) {

		if (keys[SDL_SCANCODE_LEFT]) {
			if (currentScene->state.player->position.x > 1) {
				currentScene->state.player->movement.x = -1.0f;
				currentScene->state.player->animIndices = currentScene->state.player->animLeft;
			}
		}
		else if (keys[SDL_SCANCODE_RIGHT]) {
			if (currentScene->state.player->position.x < 20) {
				currentScene->state.player->movement.x = 1.0f;
				currentScene->state.player->animIndices = currentScene->state.player->animRight;
			}
		}
		if (keys[SDL_SCANCODE_UP]) {
			if (currentScene->state.player->position.y < -1) {
				currentScene->state.player->movement.y = 1.0f;
				currentScene->state.player->animIndices = currentScene->state.player->animUp;
			}
		}
		else if (keys[SDL_SCANCODE_DOWN]) {
			if (currentScene->state.player->position.y > -14) {
				currentScene->state.player->movement.y = -1.0f;
				currentScene->state.player->animIndices = currentScene->state.player->animDown;
			}
		}


		if (glm::length(currentScene->state.player->movement) > 1.0f) {
			currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
		}
	}

}


/*-----FIXED TIMESTEP CODE-----*/
#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

bool playerHit = false;

void Update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float deltaTime = ticks - lastTicks;
	lastTicks = ticks;
	deltaTime += accumulator;
	if (deltaTime < FIXED_TIMESTEP) {
		accumulator = deltaTime;
		return;
	}
	while (deltaTime >= FIXED_TIMESTEP) {
		currentScene->Update(FIXED_TIMESTEP);

		if (playerHit == true) {
			effects->Start(SHAKE, 0.1f);

		}
		if (currentScene != sceneList[0] && currentScene != sceneList[3] && currentScene != sceneList[4]) {
			playerHit = currentScene->state.player->hit;
		}

		effects->Update(FIXED_TIMESTEP);

		// If we have enough time to do another update, we're telling the player to Update
		deltaTime -= FIXED_TIMESTEP;

		// We're updating a consistent amount over time
	}
	accumulator = deltaTime;

	// When player collides with an enemy, enter the battle phase
	// If player collides with an enemy who is NOT dead
	if (currentScene == sceneList[1]) {
		if (currentScene->state.player->enemyCollidedWith != nullptr && !currentScene->state.player->enemyCollidedWith->dead && currentScene->state.player->lastCollision == ENEMY) {
			// Save the enemy we colluded with so we know where to place the player post-battle and which enemy to not render if the player wins
			globalState.lastCollision = currentScene->state.player->enemyCollidedWith;
			// Save position to place player post battle
			globalState.playerPosition = currentScene->state.player->enemyCollidedWith->position;

			for (int i = 0; i < globalState.enemyCount; i++) {
				if (&globalState.enemies[i] == currentScene->state.player->enemyCollidedWith) {
					// Search for the enemy in the global state array, once we have the index, store it so that if the player wins, we can set this enemy to dead					
					globalState.enemyBattling = i;
				}
			}
			// Starting the fade in effect immediately
			effects->Start(FADEIN, 0.8f);
			// Enter the battle screen
			currentScene->state.nextScene = 2;
		}
	}

	// Don't have camera track player in the menu
	if (currentScene != sceneList[0] && currentScene != sceneList[3] && currentScene != sceneList[4]) {
		// Updating the camera based on the player x position
		viewMatrix = glm::mat4(1.0f);
		// If our x coordinate is past 5, then start following him (note how 5 is the player's starting position)
		if (currentScene->state.player->position.x > 5) {
			viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, -currentScene->state.player->position.y, 0));
		}
		// Otherwise, lock the camera
		else {
			viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, -currentScene->state.player->position.y, 0));
		}
	}

/*---------------------------- EFFECTS AND WIN CONDITION CHECKS ----------------------------*/
	// Our effects will shake the camera view while the shake effect is active
	viewMatrix = glm::translate(viewMatrix, effects->viewOffset);

	// Constantly check for the player falling off screen and put them back to the start if they fall
	if (currentScene != sceneList[0] && currentScene != sceneList[3] && currentScene != sceneList[4]) {
		if (*(globalState.player_health) <= 0.0f) {
			// let the game know the player has died so position can be reset
			currentScene->state.player->dead = true;
		}
		if (*(currentScene->state.enemies[0].health) <= 0.0f) {
			currentScene->state.enemies[0].dead = true;
		}

		// Check victory condition (all enemies defeated)
		int enemiesDefeated = 0;
		for (int i = 0; i < globalState.enemyCount; i++) {
			if (globalState.enemies[i].dead) {
				enemiesDefeated++;
			}
		}
		if (enemiesDefeated == globalState.enemyCount) {
			// Proceed to the victory screen!
			effects->Start(FADEIN, 0.5f);
			currentScene->state.nextScene = 3;
		}

		if (currentScene->state.player->dead) {
			// Proceeed to the death screen
			effects->Start(FADEIN, 0.5f);
			currentScene->state.nextScene = 4;

		}
	}

}


void Render() {
	glClear(GL_COLOR_BUFFER_BIT);

	//Setting the camera in render
	program.SetViewMatrix(viewMatrix);

	// Have this so the effect scan use the right program
	glUseProgram(program.programID);
	// Render the current scene
	currentScene->Render(&program);	

	// Render the effects last as they're more of an overlay
	effects->Render();

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
		// After we update our scene, if there's an indicator to change levels, change levels
		if (currentScene->state.nextScene >= 0) {
			SwitchToScene(sceneList[currentScene->state.nextScene]);
		}

		Render();
	}

	Shutdown();
	return 0;
}

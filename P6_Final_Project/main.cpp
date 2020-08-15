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


SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Scene* currentScene;
// Hold an array of levels and we can point to the current level
Scene* sceneList[3];

// Sound effects
Mix_Chunk* jump;

Effects* effects;

void SwitchToScene(Scene* scene) {
	currentScene = scene;
	currentScene->Initialize();
}

void Initialize() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("JRPG", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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

	// Initializing our levels and starting at level 1
	sceneList[0] = new Menu();
	sceneList[1] = new Level1();
	sceneList[2] = new Battle();
	SwitchToScene(sceneList[0]);

	// Loading sound
	jump = Mix_LoadWAV("jump.wav");

	// Passing in the projection and view matrixes so that it knows the dimension of the window
	effects = new Effects(projectionMatrix, viewMatrix);

	// Starting the fade in effect immediately
	effects->Start(FADEIN, 1.0f);
}

void ProcessInput() {
	// Have checks in place to not do anything with the player entity in the menu because the player entity does not exist in the menu
	if (currentScene != sceneList[0]) {
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

				// FOR THE BATTLE STAGE, ALLOW PLAYER TO SELECT THEIR MOVES
				if (currentScene == sceneList[2]) {
					case SDLK_1:
						// Do move 1
						currentScene->state.player->currMove = 0;
						break;
					case SDLK_2:
						// Do move 2
						currentScene->state.player->currMove = 1;
						break;
					case SDLK_3:
						// Do move 3
						currentScene->state.player->currMove = 2;
						break;
				}
			}
		}
		break; // SDL_KEYDOWN
	}


	const Uint8* keys = SDL_GetKeyboardState(NULL);

	// Allow movement on the overworld screen
	if (currentScene == sceneList[1]) {

		if (keys[SDL_SCANCODE_LEFT]) {
			currentScene->state.player->movement.x = -1.0f;
			currentScene->state.player->animIndices = currentScene->state.player->animLeft;
		}
		else if (keys[SDL_SCANCODE_RIGHT]) {
			currentScene->state.player->movement.x = 1.0f;
			currentScene->state.player->animIndices = currentScene->state.player->animRight;
		}
		else if (keys[SDL_SCANCODE_UP]) {
			currentScene->state.player->movement.y = 1.0f;
			currentScene->state.player->animIndices = currentScene->state.player->animUp;
		}
		else if (keys[SDL_SCANCODE_DOWN]) {
			currentScene->state.player->movement.y = -1.0f;
			currentScene->state.player->animIndices = currentScene->state.player->animDown;
		}


		if (glm::length(currentScene->state.player->movement) > 1.0f) {
			currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
		}
	}

	// Defining controls for the battle screen
	if (currentScene == sceneList[2]) {
		// Player can navigate a UI and select the move they want to use
		if (currentScene->state.playerTurn) {
			// Allow player to select an attack option
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
		if (currentScene != sceneList[0]) {
			playerHit = currentScene->state.player->hit;
		}

		effects->Update(FIXED_TIMESTEP);

		// If we have enough time to do another update, we're telling the player to Update
		deltaTime -= FIXED_TIMESTEP;

		// We're updating a consistent amount over time
	}
	accumulator = deltaTime;

	// Don't have camera track player in the menu
	if (currentScene != sceneList[0]) {
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

	// Our effects will shake the camera view while the shake effect is active
	viewMatrix = glm::translate(viewMatrix, effects->viewOffset);

	// Constantly check for the player falling off screen and put them back to the start if they fall
	if (currentScene != sceneList[0]) {
		// If the player falls off the screen, deduct a life
		if (currentScene->state.player->health < 0.0f) {
			// let the game know the player has died so position can be reset
			currentScene->state.player->dead = true;
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

	if (currentScene != sceneList[0]) {

		// Draw you lose when you lose
		if (currentScene->state.player->dead) {
			Util::DrawText(&program, Util::LoadTexture("font1.png"), "Game Over", 0.5, 0.05, glm::vec3(2.5, -2.5, 0));
		}
	}



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

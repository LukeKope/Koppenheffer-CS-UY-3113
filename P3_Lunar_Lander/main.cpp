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

#define PLATFORM_COUNT 24
struct GameState {
	Entity* player;
	Entity* platforms;
	Entity* text;
	// Var to keep track of whether the player loses or wins so we know when to draw the end game text
	bool gameOver = false;
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
	displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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

	// Initialize Player
	state.player = new Entity();
	state.player->entityType = PLAYER;
	state.player->position = glm::vec3(0, 4.0f, 0);
	state.player->movement = glm::vec3(0);
	// Setting acceleration to gravity (lowering gravity for assignment specs)
	state.player->acceleration = glm::vec3(0.0, -0.40f, 0);
	state.player->speed = 2.0f;
	state.player->textureID = LoadTexture("player.png");


	// When we checked collisions prior, the sprite appeared to be floating because the sprite actually had a bit of a border around it
	// We fix this by setting the size of the player sprite
	state.player->height = 1.0f;
	state.player->width = 1.0f;

	state.player->jumpPower = 3.0f;

	state.text = new Entity();
	state.text->textureID = LoadTexture("font1.png");

	// Initialize platforms
	state.platforms = new Entity[PLATFORM_COUNT];

	// WINNING PLATFORMS
	GLuint goodPlatformTextureID = LoadTexture("landing_platform.png");
	state.platforms[0].textureID = goodPlatformTextureID;
	state.platforms[0].entityType = VICTORY_PLATFORM;
	state.platforms[0].position = glm::vec3(-3.5f, -3.25f, 0);

	state.platforms[1].textureID = goodPlatformTextureID;
	state.platforms[1].entityType = VICTORY_PLATFORM;
	state.platforms[1].position = glm::vec3(-2.5f, -3.25f, 0);

	// LOSING PLATFORMS
	GLuint badPlatformTextureID = LoadTexture("bad_platform.png");
	state.platforms[2].textureID = badPlatformTextureID;
	state.platforms[2].entityType = PLATFORM;
	state.platforms[2].position = glm::vec3(-1.5f, -3.25f, 0);

	state.platforms[3].textureID = badPlatformTextureID;
	state.platforms[3].entityType = PLATFORM;
	state.platforms[3].position = glm::vec3(-0.5f, -3.25f, 0);

	state.platforms[4].textureID = badPlatformTextureID;
	state.platforms[4].entityType = PLATFORM;
	state.platforms[4].position = glm::vec3(0.5f, -3.25f, 0);

	state.platforms[5].textureID = badPlatformTextureID;
	state.platforms[5].entityType = PLATFORM;
	state.platforms[5].position = glm::vec3(1.5f, -3.25f, 0);

	state.platforms[6].textureID = badPlatformTextureID;
	state.platforms[6].entityType = PLATFORM;
	state.platforms[6].position = glm::vec3(2.5f, -3.25f, 0);

	state.platforms[7].textureID = badPlatformTextureID;
	state.platforms[7].entityType = PLATFORM;
	state.platforms[7].position = glm::vec3(3.5f, -3.25f, 0);

	// Tiles for the right side wall going upward
	for (int i = 0; i < 8; i++) {
		state.platforms[i + 8].textureID = badPlatformTextureID;
		state.platforms[i + 8].entityType = PLATFORM;
		state.platforms[i + 8].position = glm::vec3(4.5, -3.25f + i, 0);
	}

	// Tiles for the left side wall going upward
	for (int i = 0; i < 8; i++) {
		state.platforms[i + 15].textureID = badPlatformTextureID;
		state.platforms[i + 15].entityType = PLATFORM;
		state.platforms[i + 15].position = glm::vec3(-4.5, -3.25f + i, 0);
	}

	// mid level platforms to avoid
	state.platforms[22].textureID = badPlatformTextureID;
	state.platforms[22].entityType = PLATFORM;
	state.platforms[22].position = glm::vec3(0, 0.25f, 0);

	state.platforms[23].textureID = badPlatformTextureID;
	state.platforms[23].entityType = PLATFORM;
	state.platforms[23].position = glm::vec3(1.0f, 0.25f, 0);


	for (int i = 0; i < PLATFORM_COUNT; i++) {
		// Update the platforms one time so that their model matrix will update
		state.platforms[i].Update(0, NULL, 0);
	}
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
		}
	}

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	// Prevent movement once game is over
	if (!state.gameOver) {
		if (keys[SDL_SCANCODE_LEFT]) {
			// Move with acceleration when holding down arrow key (modifying the acceleration vec3)
			state.player->acceleration.x = -1.0f;
		}
		else if (keys[SDL_SCANCODE_RIGHT]) {
			// Move with acceleration when holding down arrow key (modifying the acceleration vec3)
			state.player->acceleration.x = 1.0f;
		}
		else {
			// reset x acceleration if player releases arrow key, keep gravity
			state.player->acceleration.x = 0.0f;
		}
	}
	// if game is over...
	else {
		// Stop player movement once game is won
		state.player->velocity.x = 0;
		state.player->velocity.y = 0;
		state.player->acceleration.y = 0.0f;
	}

}

/*-----FIXED TIMESTEP CODE-----*/
#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

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
		// Update. Notice it's FIXED_TIMESTEP. Not deltaTime
		// Updating Player and passing in platforms which we check collision with
		state.player->Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT);

		// If we have enough time to do another update, we're telling the player to Update
		deltaTime -= FIXED_TIMESTEP;

		// We're updating a consistent amount over time
	}
	accumulator = deltaTime;

	// Once the player collides with a platform, check if it's the winning platform
	if (state.player->collidedBottom || state.player->collidedTop || state.player->collidedLeft || state.player->collidedRight) {
		if (state.player->lastCollision == PLATFORM) {
			// PLAYER LOSES, display losing text
			state.text->endGameText = "Mission Failed.";
			state.gameOver = true;
		}
		else if (state.player->lastCollision == VICTORY_PLATFORM) {
			// PLAYER WINS, display victory text
			state.text->endGameText = "Mission Successful!";
			state.gameOver = true;
		}
	}

}

void Render() {

	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < PLATFORM_COUNT; i++) {
		state.platforms[i].Render(&program);
	}

	state.player->Render(&program);

	// once player lands or loses, draw the text
	if (state.gameOver) {
		state.text->DrawText(&program, state.text->textureID, state.text->endGameText, 0.5f, -0.25f, glm::vec3(-1.5f, -0.5f, 0));;
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

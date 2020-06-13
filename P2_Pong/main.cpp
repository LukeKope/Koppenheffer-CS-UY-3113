/*
Luke Koppenheffer
Project 2 - Pong
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

// Must include STB_IMAGE_IMPLEMENTATION in one of the files you are including it from
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, rightPaddleMatrix, ballMatrix, projectionMatrix;

// Player Movement Vars
// Start at -4.5, 0, 0 (left side of the screen)
glm::vec3 left_player_position = glm::vec3(-4.5f, 0.0f, 0.0f);
glm::vec3 left_player_movement = glm::vec3(0.0f, 0.0f, 0.0f);
float left_player_speed = 3.0f;

glm::vec3 right_player_position = glm::vec3(4.5f, 0.0f, 0.0f);
glm::vec3 right_player_movement = glm::vec3(0.0f, 0.0f, 0.0f);
float right_player_speed = 3.0f;

glm::vec3 ball_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 ball_movement = glm::vec3(0.0f, 0.0f, 0.0f);
float ball_speed = 2.0f;
float ball_x_direction = 1.0f;
float ball_y_direction = 1.0f;

// Set the vars for the bounding box of the paddles and ball
float paddle_width = 1.0f;
float paddle_height = 1.0f;
float ball_width = 1.0f;
float ball_height = 1.0f;

// bool to keep track of when to start the game and move the ball
bool startGame = false;
bool endGame = false;


// Initializing a paddleTextureID to be used in Render() 
GLuint paddleTextureID;
//Load texture function returns the id number of a texture given a file path
GLuint LoadTexture(const char* filePath) {
	int w, h, n;
	unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

	if (image == NULL) {
		std::cout << "Unable to load image. Make sure the path is correct\n";
		assert(false);
	}

	GLuint textureID;
	//Generate one texture ID for use
	glGenTextures(1, &textureID);
	//Bind that texture ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	//Here is the raw pixel data, put that onto the video card at this ID number
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	//Using the nearest filter for the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//Freeing the image from out main memory
	stbi_image_free(image);
	//Return the texture ID
	return textureID;
}

void Initialize() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Pong!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 640, 480);

	// Variable which is a shader program
	// using _textured loads a shader that can handle textured polygons
	program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

	viewMatrix = glm::mat4(1.0f);
	modelMatrix = glm::mat4(1.0f);
	rightPaddleMatrix = glm::mat4(1.0f);
	ballMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);
	//program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);

	glUseProgram(program.programID);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	// Enable blenidng so that you don't have the black box around the sprite
	glEnable(GL_BLEND);
	// Good setting for transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Load the texture in
	paddleTextureID = LoadTexture("paddle.png");
}

void ProcessInput() {

	// Reset the player movements
	left_player_movement = glm::vec3(0);
	right_player_movement = glm::vec3(0);

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
				startGame = true;
				break;
			}
			break; // SDL_KEYDOWN
		}
	}

	// Not inside the while loop. This is NOT an event. Checking for key held down
	// We're just getting a pointer and looking at the values in there
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	/*-----LEFT PADDLE CONTROLS-----*/
	if (keys[SDL_SCANCODE_W]) {
		// While keyboard is HELD down, move up
		// Prevent player from going off screen
		if (left_player_position.y + 0.5f < 3.75f) {
			left_player_movement.y = 1.0f;
		}
	}
	else if (keys[SDL_SCANCODE_S]) {
		// While keyboard is HELD down, move left
		// Prevent player from going off screen
		// The -0.5f accounts for the sprite being centered at the origin, i.e. it makes it so that the bottom of the sprite doesn't go off screen
		if (left_player_position.y - 0.5f > -3.75f) {
			left_player_movement.y = -1.0f;
		}
	}

	/*-----RIGHT PADDLE CONTROLS-----*/
	if (keys[SDL_SCANCODE_UP]) {
		// While keyboard is HELD down, move up
		// Prevent player from going off screen
		if (right_player_position.y + 0.5f < 3.75f) {
			right_player_movement.y = 1.0f;
		}
	}
	else if (keys[SDL_SCANCODE_DOWN]) {
		// While keyboard is HELD down, move left
		// Prevent player from going off screen
		// The -0.5f accounts for the sprite being centered at the origin, i.e. it makes it so that the bottom of the sprite doesn't go off screen
		if (right_player_position.y - 0.5f > -3.75f) {
			right_player_movement.y = -1.0f;
		}
	}

	// No need to normalize movement as it is only up and down, no diagonal
	// If they hold down both keys they cannot move faster than 1.0f
	/*if (glm::length(left_player_movement) > 1.0f) {
		left_player_movement = glm::normalize(left_player_movement);
	}*/

}


// Passing in the two matrices of the paddle position (left or right) and the ball
// anytime the ball collides with a paddle or with the ceiling or floor, have the y direction of movement change
bool checkCollision(glm::vec3& paddle, glm::vec3& ball) {
	// X Distance = XDiff - (W1 + W2)/2
	float xdiff = (fabs(paddle.x - ball.x) - (paddle_width + ball_width) / 2.0f);
	// Y Distance = YDiff - (H1 + H2)/2
	float ydiff = (fabs(paddle.y - ball.y) - (paddle_height + ball_height) / 2.0f);

	// Colliding!
	if (xdiff < 0 && ydiff < 0) {
		return true;
	}
	else {
		return false;
	}
}


float lastTicks = 0.0f;
//We do all of our transformations in the update function. 
//Every frame, we update the triangles position, rotation, or scale
//Think of this like step in game maker
void Update() {
	// While the game is still playable (i.e. nobody has won yet)
	if (!endGame) {
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float deltaTime = ticks - lastTicks;
		lastTicks = ticks;


		// Add (direction * units per second * elapsed time)
		left_player_position += left_player_movement * left_player_speed * deltaTime;
		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, left_player_position);

		// Right player position updates
		right_player_position += right_player_movement * right_player_speed * deltaTime;
		rightPaddleMatrix = glm::mat4(1.0f);
		rightPaddleMatrix = glm::translate(rightPaddleMatrix, right_player_position);


		// Player has pressed space, start the game
		if (startGame) {
			ball_movement.x = 1.0f * ball_x_direction; // * ball_movement.x * ball_speed * deltaTime;
			ball_movement.y = 1.0f * ball_y_direction; // *ball_movement.y* ball_speed* deltaTime;
			ball_position += ball_movement * ball_speed * deltaTime;
			ballMatrix = glm::mat4(1.0f);
			ballMatrix = glm::translate(ballMatrix, ball_position);
		}

		bool leftPaddleCollision = checkCollision(left_player_position, ball_position);
		bool rightPaddleCollision = checkCollision(right_player_position, ball_position);

		if (leftPaddleCollision) {
			// Have ball move towards right
			ball_x_direction = 1.0f;
			leftPaddleCollision = false;
		}
		if (rightPaddleCollision) {
			// shift ball to the left
			ball_x_direction = -1.0f;
			rightPaddleCollision = false;
		}

		if (ball_position.y >= 3.25 || ball_position.y <= -3.25) {
			//reverse ball_y_direction whenever it hits the ceiling or floor 
			ball_y_direction *= -1.0f;
		}

		// Check for the ball hitting the wall
		if (ball_position.x >= 4.5f || ball_position.x <= -4.5f) {
			// End Game, ball has hit the wall
			ball_movement.x = 0.0f;
			endGame = true;
		}
	}
}

void DrawLeftPaddle() {
	program.SetModelMatrix(modelMatrix);
	glBindTexture(GL_TEXTURE_2D, paddleTextureID);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DrawRightPaddle() {
	program.SetModelMatrix(rightPaddleMatrix);
	glBindTexture(GL_TEXTURE_2D, paddleTextureID);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DrawBall() {
	program.SetModelMatrix(ballMatrix);
	glBindTexture(GL_TEXTURE_2D, paddleTextureID);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

// We need to update our rendiring so that it's using the coordinates of the vertices and the texture
void Render() {
	// Clear the background
	glClear(GL_COLOR_BUFFER_BIT);

	// Coordinates for vertices and texture UV coords
	float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
	float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

	// Functions for saying use this list of vertices and coordinates
	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);
	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program.texCoordAttribute);

	// Draw Objects
	DrawLeftPaddle();
	DrawRightPaddle();
	DrawBall();

	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);

	// Switch window which we always do last
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

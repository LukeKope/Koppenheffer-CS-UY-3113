/*
Luke Koppenheffer
Project 1 - Simple 2D Scene
main.cpp
*/

/*
Luke Koppenheffer
Textures Example Code
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

//Must include STB_IMAGE_IMPLEMENTATION in one of the files you are including it from
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;

/*-----Creating the matrix for the object, setting variables to transform said object, and initialize textureID-----*/
//Creating matrices for all objects
glm::mat4 viewMatrix, modelMatrix, UFOMatrix, projectionMatrix;
/*-----Variables to be used in Update()-----*/
float player_x = 0;
float player_rotate = 0;
// Variables for object 2, ufoBlue.png
float ufo_x = 0;
float ufo_rotate = 0;
// Initializing a textureID's to be used in Render() 
GLuint playerTextureID;
GLuint UFOTextureID;


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
	displayWindow = SDL_CreateWindow("Simple 2D Scene!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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
	UFOMatrix = glm::mat4(2.0f);
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

	/*-----Load the texture in using the file name-----*/
	playerTextureID = LoadTexture("ctg.png");
	UFOTextureID = LoadTexture("ufoBlue.png");
}

void ProcessInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			gameIsRunning = false;
		}
	}
}


float lastTicks = 0.0f;
// Perform transformations every frame
void Update() {
	// Calculate deltaTime
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float deltaTime = ticks - lastTicks;
	lastTicks = ticks;

	/*-----Update variables that transform objects-----*/
	player_x += 1.0f * deltaTime;
	player_rotate += 90.0f * deltaTime;
	ufo_x += 0.5f * deltaTime;
	ufo_rotate += 30.0f * deltaTime;


	/*-----Perform transformations on the matrixes-----*/
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(player_x, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(player_rotate), glm::vec3(0.0f, 0.0f, 1.0f));

	UFOMatrix = glm::mat4(1.0f);
	UFOMatrix = glm::translate(UFOMatrix, glm::vec3(ufo_x, 0.0f, 0.0f));
	UFOMatrix = glm::rotate(UFOMatrix, glm::radians(ufo_rotate), glm::vec3(0.0f, 0.0f, 1.0f));

}


void Render() {
	// Coordinates for vertices and texture UV coords
	float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
	float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

	// Functions for saying use this list of vertices and coordinates
	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);
	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program.texCoordAttribute);

	/*-----SET OBJECTS HERE-----*/
	// OBJECT 1, PLAYER
	program.SetModelMatrix(modelMatrix);
	glBindTexture(GL_TEXTURE_2D, playerTextureID);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	// OBJECT 2, UFO
	program.SetModelMatrix(UFOMatrix);
	glBindTexture(GL_TEXTURE_2D, UFOTextureID);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);

	// Switch window which we always do last
	SDL_GL_SwapWindow(displayWindow);

	// Clear the background
	glClear(GL_COLOR_BUFFER_BIT);



}

/*-----SHUTDOWN-----*/
void Shutdown() {
	SDL_Quit();
}


/*-----MAIN LOOP-----*/
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

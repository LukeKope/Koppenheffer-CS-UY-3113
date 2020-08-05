#pragma once
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
#include "Map.h"
#include <vector>

enum EntityType {PLAYER, PLATFORM, ENEMY};
enum AIType { WALKER, WAITANDGO, JUMPER, PATROL };
enum AIState { IDLE, WALKING, ATTACKING };

class Entity {
public:
	EntityType entityType;
	EntityType lastCollision;
	AIType aiType;
	AIState aiState;
	std::vector<std::string> moveset; 
	glm::vec3 position;
	glm::vec3 movement;
	glm::vec3 acceleration;
	glm::vec3 velocity;

	// Units to use for collision detection checks
	float width = 1;
	float height = 1;

	// track last enemy we collided with so we can remove it from the screen if player kills it
	Entity* enemyCollidedWith;

	double health;
	// Keep track of if an entity gets hit or not
	bool dead;


	bool jump = false;
	float jumpPower = 0;

	float speed;

	GLuint textureID;

	glm::mat4 modelMatrix;

	int* animRight = NULL;
	int* animLeft = NULL;
	int* animUp = NULL;
	int* animDown = NULL;

	int* animIndices = NULL;
	int animFrames = 0;
	int animIndex = 0;
	float animTime = 0;
	int animCols = 0;
	int animRows = 0;

	bool isActive = true;

	bool collidedTop = false;
	bool collidedBottom = false;
	bool collidedLeft = false;
	bool collidedRight = false;

	Entity();

	bool CheckCollision(Entity* other);
	void CheckCollisionsY(Entity* objects, int objectCount);
	void CheckCollisionsX(Entity* objects, int objectCount);
	// Checking collisions with the map
	void CheckCollisionsX(Map* map);
	void CheckCollisionsY(Map* map);

	void Update(float deltaTime, Entity* player, Entity* objects, int objectCount, Map* map);
	void Render(ShaderProgram* program);
	void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index);

	void AI(Entity* player);
	void AIWalker();
	void AIWaitAndGo(Entity* player);
	void AIJumper();
	void AIPatrol();
};

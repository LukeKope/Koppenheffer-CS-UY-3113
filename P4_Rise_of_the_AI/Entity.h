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
	glm::vec3 position;
	glm::vec3 movement;
	glm::vec3 acceleration;
	glm::vec3 velocity;
	
	// track last enemy we collided with so we can remove it from the screen if player kills it
	Entity* enemyCollidedWith;
	// Keep track of alive or dead
	bool dead;

	// Text to display at the end of the game
	std::string endGameText = "";

	// Units to use for collision detection checks
	float width = 1;
	float height = 1;

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
	//void CheckEnemyY(Entity* other);
	void CheckEnemiesX(Entity* enemies, int numEnemies);
	void CheckEnemiesY(Entity* enemies, int numEnemies);
	void CheckCollisionsY(Entity* objects, int objectCount);
	void CheckCollisionsX(Entity* objects, int objectCount);

	void Update(float deltaTime, Entity* player, Entity* platforms, Entity* enemies, int platformCount, int enemyCount);
	void Render(ShaderProgram* program);
	void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index);
	void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position);

	void AI(Entity* player);
	void AIWalker();
	void AIWaitAndGo(Entity* player);
	void AIJumper();
	void AIPatrol();
};

#include "Level1.h"
#include "Battle.h"
#include<vector>

#define LEVEL1_ENEMY_COUNT 2

#define LEVEL1_WIDTH 22
#define LEVEL1_HEIGHT 16
//1 is cobblestone, 2 is wood, 3 is brick

unsigned int level1_data[] =
{
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
	3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
	3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
	3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
	3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
	3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 3,
	3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 3,
	3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 3,
	3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
	3, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
	3, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
	3, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
	3, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
	3, 1, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
};

void Level1::InitPlayer(float* playerHealth) {
	state.player = new Entity();
	// Setting entity Type so we can know what type of object this is when we check for collisions
	state.player->entityType = PLAYER;
	state.player->position = state.startPosition;
	state.player->movement = glm::vec3(0, 0, 0);
	// Setting acceleration to gravity
	//state.player->acceleration = glm::vec3(0, -9.81f, 0);
	state.player->speed = 4.0f;
	state.player->jumpPower = 6.0f;
	state.player->health = playerHealth;

	state.player->textureID = Util::LoadTexture("characters.png");

	/*state.player->animRight = new int[4]{ 20, 21, 22, 23 };
	state.player->animLeft = new int[4]{ 1, 5, 9, 13 };
	state.player->animUp = new int[4]{ 2, 6, 10, 14 };
	state.player->animDown = new int[4]{ 0, 4, 8, 12 };

	state.player->animIndices = state.player->animRight;
	state.player->animFrames = 4;
	state.player->animIndex = 0;
	state.player->animTime = 0;
	state.player->animCols = 4;
	state.player->animRows = 4;*/

	// When we checked collisions prior, the sprite appeared to be floating because the sprite actually had a bit of a border around it
	// We fix this by setting the size of the player sprite
	state.player->height = 0.8f;
	state.player->width = 0.8f;

}

void Level1::InitEnemies(Entity* globalEnemies) {
	
	// Setting a pointer to the global enemies which we manage in main
	state.enemies = globalEnemies;

	//// Setting entity Type so we can know what type of object this is when we check for collisions
	//state.enemies[0].entityType = ENEMY;
	//// Specifying the type of AI character this entity is
	//state.enemies[0].aiType = WALKER;
	//// Specifying the state that the AI is in. Is it walking, idle, attacking, etc.
	//state.enemies[0].aiState = IDLE;
	//state.enemies[0].textureID = enemyTextureID;
	//state.enemies[0].position = glm::vec3(5, -2, 0);
	//state.enemies[0].speed = 1;
	//state.enemies[0].health = 100.0f;


	//state.enemies[1].entityType = ENEMY;
	//state.enemies[1].aiType = WAITANDGO;
	//state.enemies[1].aiState = IDLE;
	//state.enemies[1].textureID = enemyTextureID;
	//state.enemies[1].position = glm::vec3(15, -5, 0);
	//state.enemies[1].speed = 1;
	//state.enemies[1].health = 100.0f;
}

void Level1::Initialize(float* playerHealth, Entity* enemiesAlive, int currEnemyIndex, glm::vec3 playerPosition) {
	// Make sure that the next scene is initialized to -1
	state.nextScene = -1;
	state.startPosition = playerPosition;

	GLuint mapTextureID = Util::LoadTexture("tileset.png");
	state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);

	// Initialize text
	state.text = new Entity();
	// Initialize font
	state.text->textureID = Util::LoadTexture("font1.png");



	// ------ Initialize Game Objects -----
	// Initialize Player
	InitPlayer(playerHealth);

	//Enemy Initialization
	InitEnemies(enemiesAlive);

}


void Level1::Update(float deltaTime) {
	state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
	for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
		state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
	}

	//// When player collides with an enemy, enter the battle phase
	//// If player collides with an enemy who is NOT dead
	//if (state.player->enemyCollidedWith != nullptr && !state.player->enemyCollidedWith->dead && state.player->lastCollision == ENEMY) {
	//	// Enter the battle screen
	//	state.nextScene = 2;
	//}
}
void Level1::Render(ShaderProgram* program) {
	state.map->Render(program);
	state.player->Render(program);
	for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
		// Only draw the enemy if it hasn't been killed by the player
		if (!state.enemies[i].dead) {
			state.enemies[i].Render(program);
		}
	}
}
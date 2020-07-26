#include "Level3.h"

#define LEVEL3_ENEMY_COUNT 1

#define LEVEL3_WIDTH 14
#define LEVEL3_HEIGHT 8

unsigned int level3_data[] =
{
	3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	3, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
	3, 1, 1, 1, 1, 1, 1, 0, 0, 3, 2, 2, 2, 2,
	3, 2, 2, 2, 2, 2, 2, 0, 0, 3, 2, 2, 2, 2
};

void Level3::Initialize() {
	// Make sure the next scene is initialized to -1
	state.nextScene = -1;

	state.startPosition = glm::vec3(2, 0, 0);

	// Setting tileset for this level
	GLuint mapTextureID = Util::LoadTexture("tileset.png");
	state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 4, 1);

	state.flag = new Entity();
	state.flag->textureID = Util::LoadTexture("Flag.png");
	state.flag->position = glm::vec3(12, -4, 0);

//----- Initialize Game Objects ------

	// Initialize Player
	state.player = new Entity();
	// Setting entity Type so we can know what type of object this is when we check for collisions
	state.player->entityType = PLAYER;
	state.player->position = state.startPosition;
	state.player->movement = glm::vec3(0);
	// Setting acceleration to gravity
	state.player->acceleration = glm::vec3(0, -9.81f, 0);
	state.player->speed = 4.0f;
	state.player->jumpPower = 6.0f;

	state.player->textureID = Util::LoadTexture("george_0.png");

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
	state.player->width = 0.8f;

	//Enemy Initialization
	state.enemies = new Entity[LEVEL3_ENEMY_COUNT];
	GLuint enemyTextureID = Util::LoadTexture("slime.png");

	// Setting entity Type so we can know what type of object this is when we check for collisions
	state.enemies[0].entityType = ENEMY;
	// Specifying the type of AI character this entity is
	state.enemies[0].aiType = WAITANDGO;
	// Specifying the state that the AI is in. Is it walking, idle, attacking, etc.
	state.enemies[0].aiState = IDLE;
	state.enemies[0].textureID = enemyTextureID;
	state.enemies[0].position = glm::vec3(6, 2.0f, 0);
	state.enemies[0].speed = 1;
	// Apply gravity to the AI
	state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);
}
void Level3::Update(float deltaTime) {
	state.player->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);
	state.enemies[0].Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);
	state.flag->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);

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

	// CONDITION TO ADVANCE THE PLAYER TO THE NEXT LEVEL (if they're far enough to the right, go to the next level)
	if (state.player->position.x >= 12) {
		// Notify the game that the player won so we can draw the winning text
		state.playerWins = true;
	}	
}
void Level3::Render(ShaderProgram* program) {
	state.map->Render(program);
	state.player->Render(program);
	state.flag->Render(program);
	for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
		// Only draw the enemy if it hasn't been killed by the player
		if (!state.enemies[i].dead) {
			state.enemies[i].Render(program);
		}
	}
	if (state.playerWins) {
		Util::DrawText(program, Util::LoadTexture("font1.png"), "You win!", 0.5, 0.05, glm::vec3(state.player->position.x, -1.5, 0));
	}
}
#include "Battle.h"
#include<vector>

#define BATTLE_ENEMY_COUNT 1

#define BATTLE_WIDTH 10
#define BATTLE_HEIGHT 8

unsigned int Battle_data[] =
{
	3,3,3,3,3,3,3,3,3,3,
	3,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,3,
	3,2,2,2,2,2,2,2,2,3,
	3,2,2,2,2,2,2,2,2,3,
	3,2,2,2,2,2,2,2,2,3,
};

void Battle::InitPlayer() {
	state.player = new Entity();
	// Setting entity Type so we can know what type of object this is when we check for collisions
	state.player->entityType = PLAYER;
	state.player->position = state.startPosition;
	state.player->movement = glm::vec3(0, 0, 0);
	// Setting acceleration to gravity
	//state.player->acceleration = glm::vec3(0, -9.81f, 0);
	state.player->speed = 4.0f;
	state.player->jumpPower = 6.0f;
	state.player->health = 100.0f;

	state.player->hit = false;

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

	state.player->moveset.push_back("Lightning");
	state.player->moveset.push_back("Fireball");
	state.player->moveset.push_back("Arcane Light");
}


void Battle::enemyAttack() {
	// Define enemy AI behavior for battle phase


	// Have a timer so it seems like enemy is thinking/taking time to move
	state.player->hit = true;

	// Update and notify that it's the player's turn for combat
	state.playerTurn = true;
}

void Battle::playerAttack() {
	// Once player chooses their attack, handle the logic here
	state.player->hit = false;

	// Update and notify that it's the enemy's turn for combat
	state.playerTurn = false;
}

void Battle::Initialize() {
	state.playerTurn = true;

	// Make sure that the next scene is initialized to -1
	state.nextScene = -1;
	state.startPosition = glm::vec3(2, -4, 0);

	GLuint mapTextureID = Util::LoadTexture("tileset.png");
	state.map = new Map(BATTLE_WIDTH, BATTLE_HEIGHT, Battle_data, mapTextureID, 1.0f, 4, 1);

	// Initialize text
	state.text = new Entity();
	// Initialize font
	state.text->textureID = Util::LoadTexture("font1.png");



	// ------ Initialize Game Objects -----

// Initialize Player
	InitPlayer();

	//Enemy Initialization
	state.enemies = new Entity[BATTLE_ENEMY_COUNT];
	GLuint enemyTextureID = Util::LoadTexture("slime.png");

	// Setting entity Type so we can know what type of object this is when we check for collisions
	state.enemies[0].entityType = ENEMY;
	// Specifying the type of AI character this entity is
	state.enemies[0].aiType = WALKER;
	// Specifying the state that the AI is in. Is it walking, idle, attacking, etc.
	state.enemies[0].aiState = IDLE;
	state.enemies[0].textureID = enemyTextureID;
	state.enemies[0].position = glm::vec3(10, 0, 0);
	state.enemies[0].speed = 1;
	state.enemies[0].hit = false;
}


void Battle::Update(float deltaTime) {
	state.player->Update(deltaTime, state.player, state.enemies, BATTLE_ENEMY_COUNT, state.map);
	state.enemies[0].Update(deltaTime, state.player, state.enemies, BATTLE_ENEMY_COUNT, state.map);

	// In battle phase, enter turn based combat
	if (state.playerTurn) {
		playerAttack();
	}
	else {
		enemyAttack();
	}


	// CONDITION TO ADVANCE THE PLAYER TO THE NEXT LEVEL (if they're far enough to the right, go to the next level)
	//if (state.player->position.x >= 17) {
	//	// this sends a notice to main that we want to change levels to level 2
	//	state.nextScene = 2;
	//}
}
void Battle::Render(ShaderProgram* program) {
	state.map->Render(program);
	state.player->Render(program);


	for (int i = 0; i < BATTLE_ENEMY_COUNT; i++) {
		// Only draw the enemy if it hasn't been killed by the player
		if (!state.enemies[i].dead) {
			state.enemies[i].Render(program);
		}
	}
}
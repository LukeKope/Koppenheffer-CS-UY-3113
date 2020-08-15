#include "Battle.h"
#include<vector>

#define BATTLE_ENEMY_COUNT 1

#define BATTLE_WIDTH 11
#define BATTLE_HEIGHT 9

unsigned int Battle_data[] =
{
	3,3,3,3,3,3,3,3,3,3,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,0,0,0,0,0,0,0,0,0,3,
	3,2,2,2,2,2,2,2,2,2,3,
	3,2,2,2,2,2,2,2,2,2,3,
	3,2,2,2,2,2,2,2,2,2,3,
	3,2,2,2,2,2,2,2,2,2,3
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

	// Add abilities to move set

	Abilities Lightning = Abilities("Lightning", 10, 20);
	Abilities Fireball = Abilities("Fireball", 15, 15);
	Abilities ArcaneLight = Abilities("Arcane Light", 5, 50);

	state.player->moveset.push_back(Lightning);
	state.player->moveset.push_back(Fireball);
	state.player->moveset.push_back(ArcaneLight);

	state.player->currMove = -1;
}


void Battle::enemyAttack() {
	// Define enemy AI behavior for battle phase
	int enemyTimer = 5000;

	while (enemyTimer != 0) {
		enemyTimer--;
	}

	// Have a timer so it seems like enemy is thinking/taking time to move
	// state.player->hit = true;

	// Update and notify that it's the player's turn for combat
	state.playerTurn = true;
}

void Battle::playerAttack() {

	// currMove changes depending on player input
	Abilities attack = state.player->moveset[state.player->currMove];

	int damage = attack.getDamage();

	if (state.enemies[0].health - damage <= 0) {
		state.enemies[0].dead = true;
	}
	else {
		// Decrement the enemies health
		state.enemies[0].health -= damage;
	}

	// Once player chooses their attack, handle the logic here
	state.player->hit = false;

	// Update and notify that it's the enemy's turn for combat
	state.playerTurn = false;

	// Reset currMove
	state.player->currMove = -1;
}

void Battle::Initialize() {
	state.playerTurn = false;

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
	// state.enemies[0].aiType = WALKER;
	// Specifying the state that the AI is in. Is it walking, idle, attacking, etc.
	state.enemies[0].aiState = IDLE;
	state.enemies[0].textureID = enemyTextureID;
	state.enemies[0].position = glm::vec3(7, -4, 0);
	state.enemies[0].speed = 1;
	state.enemies[0].hit = false;
	state.enemies[0].health = 100;
}


void Battle::Update(float deltaTime) {
	state.player->Update(deltaTime, state.player, state.enemies, BATTLE_ENEMY_COUNT, state.map);
	state.enemies[0].Update(deltaTime, state.player, state.enemies, BATTLE_ENEMY_COUNT, state.map);

	// In battle phase, enter turn based combat
	if (!state.playerTurn) {
		// Call enemy attack when it's not players turn. This updates to say player's turn is happening. If it's player's turn, allow player to press 1-3 to select move and attack
		enemyAttack();
	}
	else {
		if (state.player->currMove != -1) {
			playerAttack();
		}
	}


	// CONDITION TO ADVANCE THE PLAYER TO THE NEXT LEVEL (if they're far enough to the right, go to the next level)
	if (state.enemies[0].dead) {
		// Display victory screen

		// this sends a notice to main that we want to change level back to main world (level 1)
		state.nextScene = 1;
	}
}


//void Battle::ProcessInput() {
//	SDL_Event event;
//	while (SDL_PollEvent(&event)) {
//		switch (event.type) {		
//		case SDL_KEYDOWN:
//			switch (event.key.keysym.sym) {
//			case SDLK_1:
//				// Do move 1
//				playerAttack(state.player->moveset[0]);
//				break;
//			case SDLK_2:
//				// Do move 2
//				playerAttack(state.player->moveset[1]);
//				break;
//			case SDLK_3:
//				// Do move 3
//				playerAttack(state.player->moveset[2]);
//				break;
//			}
//		}
//		break; // SDL_KEYDOWN
//	}
//}


void Battle::Render(ShaderProgram* program) {
	state.map->Render(program);
	state.player->Render(program);

	if (state.playerTurn) {
		Util::DrawText(program, state.text->textureID, "1)" + state.player->moveset[0].getName(), 0.3, 0.03, glm::vec3(1, -5, 0));
		Util::DrawText(program, state.text->textureID, "Damage:" + std::to_string(state.player->moveset[0].getDamage()), 0.3, 0.03, glm::vec3(6, -5, 0));
		Util::DrawText(program, state.text->textureID, "2)" + state.player->moveset[1].getName(), 0.3, 0.03, glm::vec3(1, -6, 0));
		Util::DrawText(program, state.text->textureID, "Damage:" + std::to_string(state.player->moveset[1].getDamage()), 0.3, 0.03, glm::vec3(6, -6, 0));
		Util::DrawText(program, state.text->textureID, "3)" + state.player->moveset[2].getName(), 0.3, 0.03, glm::vec3(1, -7, 0));
		Util::DrawText(program, state.text->textureID, "Damage:" + std::to_string(state.player->moveset[2].getDamage()), 0.3, 0.03, glm::vec3(6, -7, 0));

		Util::DrawText(program, state.text->textureID, "Health:" + std::to_string(static_cast<int>(state.player->health)), 0.3, 0.03, glm::vec3(1, -3, 0));
		Util::DrawText(program, state.text->textureID, "Health:" + std::to_string(static_cast<int>(state.enemies[0].health)), 0.3, 0.03, glm::vec3(6, -3, 0));
	}

	for (int i = 0; i < BATTLE_ENEMY_COUNT; i++) {
		// Only draw the enemy if it hasn't been killed by the player
		if (!state.enemies[i].dead) {
			state.enemies[i].Render(program);
		}
	}
}
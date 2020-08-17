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

void Battle::InitPlayer(float* playerHealth) {
	state.player = new Entity();
	// Setting entity Type so we can know what type of object this is when we check for collisions
	state.player->entityType = PLAYER;
	state.player->position = state.startPosition;
	state.player->movement = glm::vec3(0, 0, 0);
	state.player->health = playerHealth;

	state.player->hit = false;

	state.player->textureID = Util::LoadTexture("character.png");

	state.player->animRight = new int[1]{ 3 };
	state.player->animLeft = new int[1]{ 1 };
	state.player->animUp = new int[1]{ 2 };
	state.player->animDown = new int[1]{ 0 };

	state.player->animIndices = state.player->animRight;
	state.player->animFrames = 0;
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


	state.player->moveset.push_back(Fireball);
	state.player->moveset.push_back(Lightning);
	state.player->moveset.push_back(ArcaneLight);

	state.player->currMove = -1;
}

void Battle::InitEnemy(Entity* globalEnemies, int currEnemyIndex) {

	// Specifying which enemy we are fighting, i.e. which enemy we last collided with
	state.enemies = &(globalEnemies[currEnemyIndex]);

	state.enemies->position = glm::vec3(7, -4, 0);
	// Prevent AI from moving on battle screen
	state.enemies->speed = 0;

	state.enemies->enemyTimer = 1.5f;
}

void Battle::Initialize(float* playerHealth, Entity* globalEnemies, int currEnemyIndex, glm::vec3 playerPosition) {
	state.playerTurn = false;
	state.nextScene = -1;
	state.startPosition = glm::vec3(2, -4, 0);



	// Initialize map
	GLuint mapTextureID = Util::LoadTexture("tileset.png");
	state.map = new Map(BATTLE_WIDTH, BATTLE_HEIGHT, Battle_data, mapTextureID, 1.0f, 4, 1);

	// Initialize text
	state.text = new Entity();
	state.text->textureID = Util::LoadTexture("pixel_font.png");

	// Initialize Player and Enemy
	InitPlayer(playerHealth);
	InitEnemy(globalEnemies, currEnemyIndex);

}

void Battle::enemyAttack(int moveIndex) {
	// Define enemy AI behavior for battle phase

	// Enemy does a random move from their moveset (This will generate a random number between 0, 1, and 2

	int damage = state.enemies[0].moveset[moveIndex].getDamage();

	if (*(state.player->health) - damage <= 0) {
		state.player->dead = true;
	}
	else {
		*(state.player->health) -= damage;
	}

	// Have a timer so it seems like enemy is thinking/taking time to move
	// state.player->hit = true;

	// Let render know that the enemy has chosen a move so that it can be displayed
	state.enemies[0].moveChosen = true;

	// Update and notify that it's the player's turn for combat
	state.playerTurn = true;


}

void Battle::playerAttack() {

	int random_move = rand() % 2;
	state.enemies[0].currMove = random_move;
	state.enemies[0].moveChosen = false;

	// currMove changes depending on player input
	Abilities attack = state.player->moveset[state.player->currMove];

	int damage = attack.getDamage();

	if (*(state.enemies[0].health) - damage <= 0) {
		state.enemies[0].dead = true;
	}
	else {
		// Decrement the enemies health
		*(state.enemies[0].health) -= damage;
	}

	// Once player chooses their attack, handle the logic here
	state.player->hit = false;

	// Update and notify that it's the enemy's turn for combat
	state.playerTurn = false;

	// Reset currMove
	state.player->currMove = -1;
}



void Battle::Update(float deltaTime) {
	state.player->Update(deltaTime, state.player, state.enemies, BATTLE_ENEMY_COUNT, state.map);
	state.enemies[0].Update(deltaTime, state.player, state.enemies, BATTLE_ENEMY_COUNT, state.map);

	// In battle phase, enter turn based combat
	if (!state.playerTurn) {		

		state.enemies[0].enemyTimer -= deltaTime;
		// Call enemy attack when it's not players turn. This updates to say player's turn is happening. If it's player's turn, allow player to press 1-3 to select move and attack
		if (state.enemies[0].enemyTimer <= 0) {
			enemyAttack(state.enemies[0].currMove);
			state.enemies[0].enemyTimer = 1.0f;
		}
	}
	else {
		if (state.player->currMove != -1) {
			playerAttack();
		}
	}

	// CONDITION TO ADVANCE THE PLAYER TO THE NEXT LEVEL (Enemy defeated)
	if (state.enemies[0].dead) {
		// this sends a notice to main that we want to change level back to main world (level 1)
		state.nextScene = 1;
	}
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
	// Draw the UI for the player to select moves from
	if (state.playerTurn) {
		Util::DrawText(program, state.text->textureID, "1)" + state.player->moveset[0].getName(), 0.3, 0.03, glm::vec3(1, -5, 0));
		Util::DrawText(program, state.text->textureID, "Damage:" + std::to_string(state.player->moveset[0].getDamage()), 0.3, 0.03, glm::vec3(6, -5, 0));
		Util::DrawText(program, state.text->textureID, "2)" + state.player->moveset[1].getName(), 0.3, 0.03, glm::vec3(1, -6, 0));
		Util::DrawText(program, state.text->textureID, "Damage:" + std::to_string(state.player->moveset[1].getDamage()), 0.3, 0.03, glm::vec3(6, -6, 0));
		Util::DrawText(program, state.text->textureID, "3)" + state.player->moveset[2].getName(), 0.3, 0.03, glm::vec3(1, -7, 0));
		Util::DrawText(program, state.text->textureID, "Damage:" + std::to_string(state.player->moveset[2].getDamage()), 0.3, 0.03, glm::vec3(6, -7, 0));
	}
	else {
		Util::DrawText(program, state.text->textureID, "Enemy Turn", 0.3, 0.03, glm::vec3(3, -5, 0));
		// Show the name of the move the enemy hits the player with

		if (state.enemies[0].currMove == 0 || state.enemies[0].currMove == 1 || state.enemies[0].currMove == 2) {
			Util::DrawText(program, state.text->textureID, "Enemy used:" + state.enemies[0].moveset[state.enemies[0].currMove].getName(), 0.2, 0.03, glm::vec3(1, -1, 0));
			Util::DrawText(program, state.text->textureID, "Damage:" + std::to_string(state.enemies[0].moveset[state.enemies[0].currMove].getDamage()), 0.2, 0.03, glm::vec3(1, -2, 0));
		}

	}

	Util::DrawText(program, state.text->textureID, "Health:" + std::to_string(static_cast<int>(*(state.player->health))), 0.3, 0.03, glm::vec3(1, -3, 0));
	Util::DrawText(program, state.text->textureID, "Health:" + std::to_string(static_cast<int>(*(state.enemies[0].health))), 0.3, 0.03, glm::vec3(6, -3, 0));
}
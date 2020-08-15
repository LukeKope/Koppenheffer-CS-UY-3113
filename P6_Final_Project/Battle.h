/*
Battle screen for when the player enters battle with an enemy
- Same for all enemies, just need to pass in the last enemy collided with to fight against
- Need to pass in the player along with player's current health
*/

#pragma once
#include "Scene.h"
#include "Abilities.h"
class Battle : public Scene {
public:
	void InitPlayer();
	void enemyAttack();
	void playerAttack();
	void Initialize() override;
	void Update(float deltaTime) override;
	// void ProcessInput();
	void Render(ShaderProgram* program) override;
};
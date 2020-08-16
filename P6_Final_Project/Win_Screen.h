/*
Screen shown if the player defeats all enemies and wins the game
*/
#pragma once
#include "Scene.h"
class Win_Screen : public Scene {
public:
	void ScreenInitialize();
	void Initialize(float* playerHealth, Entity* enemiesAlive, int currEnemyIndex, glm::vec3 playerPosition) override;
	void Update(float deltaTime) override;
	void Render(ShaderProgram* program) override;
};

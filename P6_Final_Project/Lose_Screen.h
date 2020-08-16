/* 
Screen shown if the player dies and loses the game
*/

#include "Scene.h"
class Lose_Screen : public Scene {
public:	
	void ScreenInitialize();
	void Initialize(float* playerHealth, Entity* enemiesAlive, int currEnemyIndex, glm::vec3 playerPosition) override;
	void Update(float deltaTime) override;
	void Render(ShaderProgram* program) override;
};

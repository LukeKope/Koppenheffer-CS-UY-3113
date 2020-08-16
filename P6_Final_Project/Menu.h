#include "Scene.h"
class Menu : public Scene {
public:
	void MenuInitialize();
	void Initialize(float* playerHealth, Entity* enemiesAlive, int currEnemyIndex, glm::vec3 playerPosition) override;
	void Update(float deltaTime) override;
	void Render(ShaderProgram* program) override;
};
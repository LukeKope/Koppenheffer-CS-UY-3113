/* Main overworld that the player can traverse and encounter enemies in
*/

#include "Scene.h"
class Level1 : public Scene {
public:
	void InitPlayer(float* playerHealth);
	void InitEnemies(Entity* globalEnemies);
	void Initialize(float* playerHealth, Entity* enemiesAlive, int currEnemyIndex, glm::vec3 playerPosition) override;
	void Update(float deltaTime) override;
	void Render(ShaderProgram* program) override;
};
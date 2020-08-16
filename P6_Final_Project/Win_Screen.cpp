#include "Win_Screen.h"

void Win_Screen::ScreenInitialize() {
	// Make sure the next scene is initialized to -1
	state.nextScene = -1;
	// Initialize text
	state.text = new Entity();
	// Initialize font
	state.text->textureID = Util::LoadTexture("font1.png");
}

// Need this initialize for other levels, but not for menu
void Win_Screen::Initialize(float* playerHealth, Entity* enemiesAlive, int currEnemyIndex, glm::vec3 playerPosition) {
	Win_Screen::ScreenInitialize();
}

void Win_Screen::Update(float deltaTime) {

}
void Win_Screen::Render(ShaderProgram* program) {
	Util::DrawText(program, state.text->textureID, "YOU WIN!", 1.0, -0.5, glm::vec3(1, -1, 0));
	Util::DrawText(program, state.text->textureID, "You defeated the slimes!", 0.5, -0.2, glm::vec3(1, -2, 0));
}
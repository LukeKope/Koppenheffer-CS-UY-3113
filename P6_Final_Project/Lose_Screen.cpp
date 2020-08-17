#include "Lose_Screen.h"

void Lose_Screen::ScreenInitialize() {
	// Make sure the next scene is initialized to -1
	state.nextScene = -1;
	// Initialize text
	state.text = new Entity();
	// Initialize font
	state.text->textureID = Util::LoadTexture("pixel_font.png");	


}

// Need this initialize for other levels, but not for menu
void Lose_Screen::Initialize(float* playerHealth, Entity* enemiesAlive, int currEnemyIndex, glm::vec3 playerPosition) {
	Lose_Screen::ScreenInitialize();
}

void Lose_Screen::Update(float deltaTime) {

}
void Lose_Screen::Render(ShaderProgram* program) {
	Util::DrawText(program, state.text->textureID, "GAME OVER", 0.8, 0.2, glm::vec3(1, -1, 0));
	Util::DrawText(program, state.text->textureID, "The slimes defeated you", 0.2, 0.2, glm::vec3(0.5, -2, 0));
}
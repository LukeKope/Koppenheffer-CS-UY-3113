#include "Menu.h"

void Menu::Initialize() {	
	// Make sure the next scene is initialized to -1
	state.nextScene = -1;
	// Initialize text
	state.text = new Entity();
	// Initialize font
	state.text->textureID = Util::LoadTexture("font1.png");
}
void Menu::Update(float deltaTime) {	
	
}
void Menu::Render(ShaderProgram* program) {
	Util::DrawText(program, state.text->textureID, "Jump Man!", 1.0, -0.5, glm::vec3(-4.25,0,0));	
	Util::DrawText(program, state.text->textureID, "Press enter to continue!", 0.5, -0.2, glm::vec3(-4.25,-1,0));	
}
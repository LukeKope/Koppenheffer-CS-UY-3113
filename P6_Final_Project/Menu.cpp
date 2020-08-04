#include "Menu.h"

void Menu::Initialize() {	
	// Make sure the next scene is initialized to -1
	state.nextScene = -1;
	// Initialize text
	state.text = new Entity();
	// Initialize font
	state.text->textureID = Util::LoadTexture("font1.png");

	// Music vars
	Mix_Music* bg_music;
	

	// Start Audio
	// Putting it up near the top so that if the game takes a while to load, the music can start playing while the player waits
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	bg_music = Mix_LoadMUS("dooblydoo.mp3");
	// Loop the music forever
	Mix_PlayMusic(bg_music, -1);
	Mix_VolumeMusic(MIX_MAX_VOLUME / 4);

	
}
void Menu::Update(float deltaTime) {	
	
}
void Menu::Render(ShaderProgram* program) {
	Util::DrawText(program, state.text->textureID, "JRPG", 1.0, -0.5, glm::vec3(-4.25, 0, 0));	
	Util::DrawText(program, state.text->textureID, "Press enter to continue!", 0.5, -0.2, glm::vec3(-4.25,-1,0));	
}
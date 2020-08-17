#include "Menu.h"

void Menu::MenuInitialize() {
	// Make sure the next scene is initialized to -1
	state.nextScene = -1;
	// Initialize text
	state.text = new Entity();
	// Initialize font
	state.text->textureID = Util::LoadTexture("pixel_font.png");

	// Music vars
	Mix_Music* bg_music;
	

	// Start Audio
	// Putting it up near the top so that if the game takes a while to load, the music can start playing while the player waits
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	bg_music = Mix_LoadMUS("music.mp3");
	// Loop the music forever
	Mix_PlayMusic(bg_music, -1);
	Mix_VolumeMusic(MIX_MAX_VOLUME / 4);

	
}

// Need this initialize for other levels, but not for menu
void Menu::Initialize(float* playerHealth, Entity* enemiesAlive, int currEnemyIndex, glm::vec3 playerPosition) {
	Menu::MenuInitialize();
}

void Menu::Update(float deltaTime) {	
	
}
void Menu::Render(ShaderProgram* program) {
	Util::DrawText(program, state.text->textureID, "SLIME HUNTER", 0.5, 0.2, glm::vec3(-4.25, 0, 0));	
	Util::DrawText(program, state.text->textureID, "Press enter to continue!", 0.2, 0.2, glm::vec3(-4.5,-1,0));	
}
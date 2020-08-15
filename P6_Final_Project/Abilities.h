#pragma once
#define GL_SILENCE_DEPRECATION
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Util.h"

class Abilities {
public:
	Abilities(const std::string& moveName, int mpAmnt, int damageAmnt);

	const std::string& getName() const;

	const int getDamage() const;

private: 
	std::string name;
	int mp;
	int damage;
};
#include "Effects.h"
Effects::Effects(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
	// Non textured Shader
	program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");
	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);
	currentEffect = NONE;
	alpha = 0;
	speed = 0.4;

	viewOffset = glm::vec3(0, 0, 0);
}

void Effects::DrawOverlay()
{
	glUseProgram(program.programID);
	float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program.positionAttribute);
}

void Effects::Start(EffectType effectType, float effectSpeed)
{
	currentEffect = effectType;
	speed = effectSpeed;

	switch (currentEffect) {
	case NONE:
		break;

	case FADEIN:
		alpha = 1.0;
		break;

	case FADEOUT:
		alpha = 0.0;
		break;

	case GROW:
		size = 0.0f;
		break;

	case SHRINK:
		size = 10.0f;
		break;

	case SHAKE: 
		timeLeft = 1.0f; // shake for one second
		break;
	}
}
void Effects::Update(float deltaTime)
{
	switch (currentEffect) {
	case NONE:
		break;

	case FADEIN:
		// Decreasing the alpha from 1 to 0 over 1 second
		alpha -= deltaTime * speed;
		// Once we've finished our effect, let the program know
		if (alpha <= 0) currentEffect = NONE;
		break;

	case FADEOUT:
		alpha += deltaTime * speed;
		// if (alpha >= 1) currentEffect = NONE; // We might want to keep the drawing after it's there
		break;

	case GROW:
		size += deltaTime * speed;
		// if(size >= 10)
		break;

	case SHRINK:
		size -= deltaTime * speed;
		if (size <= 0) currentEffect = NONE;
		break;

	case SHAKE: 
		timeLeft -= deltaTime * speed;
		if (timeLeft <= 0) {
			viewOffset = glm::vec3(0, 0, 0);
			currentEffect = NONE;
		}
		else {
			// Randomizing the camera shake
			float max = 0.1f;
			float min = -0.1f;
			float r = ((float)rand() / RAND_MAX) * (max - min) + min;
			viewOffset = glm::vec3(r, r, 0);
		}
	}
}
void Effects::Render()
{
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	switch (currentEffect) {
	case NONE:
		return;
		break;

		// Uses the same code for fade out and fade in, uses the same alpha
	case FADEOUT:
	case FADEIN:
		modelMatrix = glm::scale(modelMatrix, glm::vec3(10, 10, 1));
		program.SetModelMatrix(modelMatrix);
		program.SetColor(0, 0, 0, alpha);
		DrawOverlay();
		break;

	case SHRINK:
	case GROW:
		modelMatrix = glm::scale(modelMatrix, glm::vec3(size, size * 0.75, 1));
		program.SetModelMatrix(modelMatrix);
		program.SetColor(0, 0, 0, 1);
		DrawOverlay();
		break;

	case SHAKE:
		// Don't need to do anything here, we're not drawing anything
		// We're just moving the viewport around
		break;
	}
}
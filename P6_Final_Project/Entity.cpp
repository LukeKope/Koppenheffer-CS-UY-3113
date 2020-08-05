#include "Entity.h"

Entity::Entity()
{
	// Vars to track player position and speed
	position = glm::vec3(0);
	speed = 0;
	movement = glm::vec3(0);
	acceleration = glm::vec3(0);
	velocity = glm::vec3(0);
	dead = false;

	modelMatrix = glm::mat4(1.0f);
}

bool Entity::CheckCollision(Entity* other) {
	// If either entity is not active, no need to check for collisions
	if (isActive == false || other->isActive == false) return false;

	float x_dist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
	float y_dist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);

	if (x_dist < 0 && y_dist < 0) {
		// Store the type of the lastCollision
		lastCollision = other->entityType;
		return true;
	}
	// Not colliding
	return false;
}

void Entity::CheckCollisionsY(Entity* objects, int objectCount)
{
	for (int i = 0; i < objectCount; i++)
	{
		Entity* object = &objects[i];
		if (CheckCollision(object))
		{
			// Platform collision checks
			if (objects->entityType == PLATFORM) {
				float ydist = fabs(position.y - object->position.y);
				float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
				if (velocity.y > 0) {
					position.y -= penetrationY;
					velocity.y = 0;
					// We went up and collided with something, so top collision flag is true
					collidedTop = true;
				}
				else if (velocity.y < 0) {
					position.y += penetrationY;
					velocity.y = 0;
					// We are moving down and collided with something below, bottom collision flag is true
					collidedBottom = true;
				}
			}
			// Enemy collision checks
			else if (objects->entityType == ENEMY) {
				// Update the pointer to who we collided with
				enemyCollidedWith = object;
				if (object->position.y > position.y) {
					// Enemy was above player, player got jumped on
					collidedTop = true;
				}
				else if (object->position.y < position.y) {
					// Enemy was below player, player jumped on enemy
					collidedBottom = true;
				}
			}
		}
	}
}

void Entity::CheckCollisionsX(Entity* objects, int objectCount)
{
	for (int i = 0; i < objectCount; i++)
	{
		Entity* object = &objects[i];
		if (CheckCollision(object))
		{
			// Collision checks for platform
			if (objects->entityType == PLATFORM) {
				float xdist = fabs(position.x - object->position.x);
				float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
				if (velocity.x > 0) {
					position.x -= penetrationX;
					velocity.x = 0;
					// Collided with something while moving right
					collidedRight = true;
				}
				else if (velocity.x < 0) {
					position.x += penetrationX;
					velocity.x = 0;
					// Collided with something while moving left
					collidedLeft = true;
				}
			}
			// Enemy collision check
			else if (objects->entityType == ENEMY) {
				// Update the pointer to who we collided with
				enemyCollidedWith = object;
				// Enemy is to the right of the player during collision
				if (object->position.x > position.x)
				{
					collidedRight = true;
				}
				// Enemy is to the left of the player during collision
				else if (object->position.x < position.x)
				{
					collidedLeft = true;
				}
			}

		}

	}
}


void Entity::CheckCollisionsY(Map* map)
{
	// Probes for tiles
	glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
	glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
	glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);
	glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
	glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
	glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);
	float penetration_x = 0;
	float penetration_y = 0;

	if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) {
		position.y -= penetration_y;
		velocity.y = 0;
		collidedTop = true;
	}
	else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
		position.y -= penetration_y;
		velocity.y = 0;
		collidedTop = true;
	}
	else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
		position.y -= penetration_y;
		velocity.y = 0;
		collidedTop = true;
	}
	if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
		position.y += penetration_y;
		velocity.y = 0;
		collidedBottom = true;
	}
	else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
		position.y += penetration_y;
		velocity.y = 0;
		collidedBottom = true;
	}
	else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
		position.y += penetration_y;
		velocity.y = 0;
		collidedBottom = true;
	}
}

void Entity::CheckCollisionsX(Map* map)
{
	// Probes for tiles
	glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
	glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);
	float penetration_x = 0;
	float penetration_y = 0;
	if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
		position.x += penetration_x;
		velocity.x = 0;
		collidedLeft = true;
	}
	if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
		position.x -= penetration_x;
		velocity.x = 0;
		collidedRight = true;
	}
}

//AI Functions
//AI Functions
void Entity::AIWalker() {
	movement = glm::vec3(-1, 0, 0);
}

void Entity::AIWaitAndGo(Entity* player) {
	switch (aiState) {

	case IDLE:
		if (glm::distance(position, player->position) < 3.0f) {
			aiState = WALKING;
		}
		break;

	case WALKING:
		// Provide a bit of a buffer so the enemy isn't right on top of you
		if (player->position.x < position.x - 0.5f) {
			movement = glm::vec3(-1, 0, 0);
		}
		else if (player->position.x > position.x + 0.5f) {
			movement = glm::vec3(1, 0, 0);
		}

		break;

	case ATTACKING:
		break;
	}
}

// Jumper enemy AI jumps whenever it collides with the floor
void Entity::AIJumper() {
	if (jump) {
		jump = false;
		velocity.y += jumpPower;
	}
}

// Patrol enemy AI that walks to the left of the screen, then turns around
void Entity::AIPatrol() {
	// On left side of screen, turn and go right
	if (position.x < -4.0f) {
		movement = glm::vec3(1, 0, 0);
	}
	// On right side of the screen, turn around and go left
	else if (position.x > 4.0f) {
		movement = glm::vec3(-1, 0, 0);
	}
}

void Entity::AI(Entity* player) {
	switch (aiType) {

	case WALKER:
		AIWalker();
		break;

	case WAITANDGO:
		AIWaitAndGo(player);
		break;

	case JUMPER:
		AIJumper();
		break;

	case PATROL:
		AIPatrol();
		break;
	}
}

void Entity::Update(float deltaTime, Entity* player, Entity* objects, int objectCount, Map* map)
{
	// if object is not active, do not update it. Exit the update function
	if (isActive == false) return;

	// Collision flags: reset the flags every frame to check for collision in all 4 directions
	collidedTop = false;
	collidedBottom = false;
	collidedLeft = false;
	collidedRight = false;

	// If they're an enemy, call the AI function to execute the AI behavior
	if (entityType == ENEMY) {
		// Passing in a pointer to the player entity
		AI(player);
	}


	if (animIndices != NULL) {
		if (glm::length(movement) != 0) {
			animTime += deltaTime;

			if (animTime >= 0.25f)
			{
				animTime = 0.0f;
				animIndex++;
				if (animIndex >= animFrames)
				{
					animIndex = 0;
				}
			}
		}
		else {
			animIndex = 0;
		}
	}

	// Allow player to jump
	if (entityType == PLAYER) {
		if (jump) {
			jump = false;
			velocity.y += jumpPower;
		}
	}

	// Using the acceleration and velocity variables
	// When our character starts moving, they have instant velocity
	velocity.x = movement.x * speed;
	velocity.y = movement.y * speed;

	// If we're accelerating, we're gonna keep adding to velocity with that acceleration
	velocity += acceleration * deltaTime;

	// Update position, check for collision for y then x
	position.y += velocity.y * deltaTime; // Move on Y
	//CheckCollisionsY(map);
	CheckCollisionsY(objects, objectCount); // Fix if needed

	position.x += velocity.x * deltaTime; // Move on X
	//CheckCollisionsX(map);
	CheckCollisionsX(objects, objectCount); // Fix if needed

	/*-----NEED TO CHECK COLLISIONS WITH THE PLAYER AND THE AI-----*/
	// Only check for collisions with an enemy if you're updating the player (otherwise, the jumper AI will jump infinitely because collidedBottom is always true from calling these checks on itself)
	if (entityType == PLAYER) {
		//CheckCollisionsY(objects, objectCount); // Update proper collision flags, update pointer to Enemy we collided with
		CheckCollisionsX(objects, objectCount); // Update proper collision flags, update pointer to Enemy we collided with
	}



	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index)
{
	float u = (float)(index % animCols) / (float)animCols;
	float v = (float)(index / animCols) / (float)animRows;

	float width = 1.0f / (float)animCols;
	float height = 1.0f / (float)animRows;

	float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
		u, v + height, u + width, v, u, v };

	float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

	glBindTexture(GL_TEXTURE_2D, textureID);

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram* program) {

	// if object is not active, do not render it. Exit the render function
	if (isActive == false) return;

	program->SetModelMatrix(modelMatrix);

	if (animIndices != NULL) {
		DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
		return;
	}

	float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
	float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

	glBindTexture(GL_TEXTURE_2D, textureID);

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

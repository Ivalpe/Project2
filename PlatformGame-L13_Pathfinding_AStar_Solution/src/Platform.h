#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class Platform : public Entity
{
public:

	Platform();
	virtual ~Platform();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void SetPosition(Vector2D pos);

	Vector2D GetPosition();


public:

private:
	int initial_position = 800;
	int end_position = 1000;
	int movment = 5;

	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;
	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idle;
	PhysBody* pbody;
};

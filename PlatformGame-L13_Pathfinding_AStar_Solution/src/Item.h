#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class Item : public Entity
{
public:

	Item();
	virtual ~Item();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

public:

	bool isPicked = false;
	bool isWax = false;
	bool isFeather = false;
	int apear = true;
private:

	SDL_Texture* texture;
	SDL_Texture* Feather_texture;

	const char* texturePath;
	int texW, texH;
	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation* currentAnimation_feather = nullptr;

	Animation idle;
	Animation idle_feather;
	
  
	PhysBody* pbody;


};

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
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
public:

	bool isPicked = false;
	bool isWax = false;
	bool isFeather = false;
	bool isStalactites = false;
	bool isWall = false;
	int apear = true;
private:

	SDL_Texture* texture;
	SDL_Texture* Feather_texture;
	SDL_Texture* Stalactites_texture;
	SDL_Texture* Wall_texture;
	const char* texturePath;
	int texW, texH;
	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation* currentAnimation_feather = nullptr;
	Animation* currentAnimation_stalactities = nullptr;
	Animation* currentAnimation_wall = nullptr;
	Animation idle;
	Animation idle_feather;
	Animation idle_Stalactites;
	Animation idle_stalactites_falls;
	Animation idle_wall;
	Animation idle_raise;
  
	PhysBody* pbody;
	bool changecolision = false;

	bool Wallraise = false;
};

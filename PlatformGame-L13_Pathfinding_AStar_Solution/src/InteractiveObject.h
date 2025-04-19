#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"

struct SDL_Texture;

class InteractiveObject : public Entity
{
public:

	InteractiveObject();
	virtual ~InteractiveObject();

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
	bool isStalactites = false;
	bool isWall = false;
	int apear = true;
private:


	SDL_Texture* Stalactites_texture;
	SDL_Texture* Wall_texture;
	const char* texturePath;
	int texW, texH;
	pugi::xml_node parameters;
	Animation* currentAnimation_stalactities = nullptr;
	Animation* currentAnimation_wall = nullptr;

	Animation idle_Stalactites;
	Animation idle_stalactites_falls;
	Animation idle_wall;
	Animation idle_raise;

	PhysBody* pbody;

	bool damageStalactite = false;
	bool floorStalactite = false;
	bool changecolision = false;

	bool Wallraise = false;
	bool blockText;

};

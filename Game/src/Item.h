#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Physics.h"

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

	void SetPosition(Vector2D pos);

	b2Body* GetBody() {
		return pbody->body;
	}

	PhysBody* GetPhysbody() {
		return pbody;
	}

	bool IsPicked() {
		return isPicked;
	}
public:

	bool isPicked = false;
	bool isWax = false;
	bool isFeather = false;
	bool isGloves = false;
	bool isCloth = false;

	PhysBody* pbody;
	int texW, texH;


private:

	SDL_Texture* texture;
	SDL_Texture* Feather_texture;

	const char* texturePath;
	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation* currentAnimation_feather = nullptr;

	Animation idle;
	Animation idle_feather;
	
  



};

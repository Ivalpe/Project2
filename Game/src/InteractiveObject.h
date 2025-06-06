#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Physics.h"

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

	void SetPosition(Vector2D pos);
	
	b2Body* GetBody() {
		return pbody->body;
	}

public:

	bool isPicked = false;
	bool isStalactites = false;
	bool isWall = false;

	PhysBody* pbody;
	int texW, texH;

private:

	SDL_Texture* Stalactites_texture;
	SDL_Texture* Wall_texture;
	const char* texturePath;
	pugi::xml_node parameters;
	Animation* currentAnimation_stalactities = nullptr;
	Animation* currentAnimation_wall = nullptr;

	Animation idle_Stalactites;
	Animation idle_stalactites_falls;
	Animation idle_wall;
	Animation fade_wall;


	bool damageStalactite = false;
	bool floorStalactite = false;
	bool changecolision = false;

	bool Wallraise = false;
	bool blockText;

	float timer = 0.0f;
	bool waitingToBecomeStatic = false;

};

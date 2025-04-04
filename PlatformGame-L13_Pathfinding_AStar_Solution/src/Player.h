#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Box2D/Box2D.h"
#include "Animation.h"
#include "Platform.h"

struct SDL_Texture;
#define glidDuration 40

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void SetPosition(Vector2D pos);


	Vector2D GetPosition();
	
	void HandleJump(float dt);
	void HandleHide(float dt);
	void HandleClimbing(float dt);
	void HandleGlide(float dt);

	b2Vec2 velocity;
public:

	//Declare player parameters
	float speed = 5.0f;
	SDL_Texture* texture = NULL;
	int texW, texH;

	//Audio fx
	int pickCoinFxId;

	PhysBody* pbody;
	float jumpForce = 2.5f; // The force to apply when jumping
	bool isJumping = false; // Flag to check if the player is currently jumping
	bool canDoubleJump = false;
	bool isHiding = false;
	int lastJump = 0;
	int glid_time = 0;
	int glid_reduce=0;
	float fallForce = 1.5f;

	bool isClimbing=false;
	bool isMoving = false;

	bool isOnPlatform = false;
	Platform* platform=nullptr;

	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idle;

};
#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Box2D/Box2D.h"
#include "Animation.h"
#include "Timer.h"

enum Direction {

	LEFT,
	RIGHT
};

enum State {
	IDLE = 0,
	WALK,
	JUMP,
	FALL,
	HIDE,
	CRAWL,
	UNHIDE,
	CLIMB,
	GLIDE,
	DEAD
};

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

	// L08 TODO 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void SetPosition(Vector2D pos);

	Vector2D GetPosition();

	int GetWax();
	void UpdateWaxToCandle();

	void StopMovement();
	void ResumeMovement();

public:

	//Declare player parameters
	float speed = 70.0f;
	SDL_Texture* texture = NULL;
	int texW, texH;

	//Audio fx
	int pickCoinFxId;

	// L08 TODO 5: Add physics to the player - declare a Physics body
	PhysBody* pbody;
	float jumpForce = 200.0f; // The force to apply when jumping
	bool isJumping = false; // Flag to check if the player is currently jumping
	bool isHiding = false; // Flag to check if the player is currently hiding
	bool isCrawling = false;
	bool canDoubleJump = false;
	int lastJump = 0;
	int glid_time = 0;
	int glid_reduce=0;
	float fallForce = 1.5f;

	bool isClimbing=false;
	bool takenDMG = false;
	bool pickedItem = true;

	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idle;
	Animation walk;
	Animation hide;
	Animation unhide;
	Animation crawl;
	Animation jump;
	Animation fall;
	Animation death;

	bool change_level = false;
	bool cleanup_pbody = false;

	int crouch = 0;
	int CntCrouch = 0;

	b2Vec2 velocity;
	Direction dir = LEFT;
	State playerState;

	Timer hideTimer;
	Timer deathTimer;

};
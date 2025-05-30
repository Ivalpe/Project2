#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Box2D/Box2D.h"
#include "Animation.h"
#include "Timer.h"
#include "Platform.h"

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
	DASH,
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

	void TeleportToTemporaryCheckpoint();

	void TakeDamage();

	 
	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void SetPosition(Vector2D pos);

	Vector2D GetPosition();

	int GetWax();

	void StopMovement();
	void ResumeMovement();

public:

	//Declare player parameters
	float speed = 70.0f;
	SDL_Texture* texture = NULL;
	int texW, texH;

	//Audio fx
	int pickCoinFxId,
		jumpFxId,
		glideFxId,
		hideFxId,
		climbFxId,
		climbingFxId,
		deathFxId,
		waterFxId,
		walkFxId,
		landFxId;
	
	PhysBody* pbody;
	float jumpForce = 200.0f; // The force to apply when jumping
	bool isJumping = false; // Flag to check if the player is currently jumping
	bool isHiding = false; // Flag to check if the player is currently hiding
	bool isCrawling = false;
	bool canDoubleJump = false;
	bool touched_wall = false;

	int lastJump = 0;
	int glid_time = 0;
	int glid_reduce=0;
	float fallForce = 1.5f;

	int dashDurantion = 10;
	bool isDashing = false;
	bool dashColdown = false;

	bool isClimbing = false;
	bool onGround = false;

	bool takenDMG = false;
	bool isInAttackSensor = false;
	bool pickedItem = true;

	bool exitingRope = false;

	bool playingsound = false;

	bool useTemporaryCheckpoint = false;
	bool setTempCheckTrue = false;
	Vector2D temporaryCheckpoint;

	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idle, walk, hide, unhide, crawl, jump, fall, land, turn2back, turn2front, climb, onrope, death, glide_start, glide, glide_stop;
	
	bool change_level = false;
	bool cleanup_pbody = false;

	int crouch = 0;
	int CntCrouch = 0;

	b2Vec2 velocity;
	Direction dir = LEFT;
	State playerState;
	State lastState = IDLE;

	Timer hideTimer;
	Timer turnTimer;
	Timer deathTimer;

	float climbableX;
	float climbOffset = 0;

	bool isOnPlatform=false;
	Platform* platform = nullptr;

	int lightDamage = 0;
	bool onLight = false;
};
#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
#include "Timer.h"
#include "Physics.h"

#define PAUSETIME 120

class Player;
struct SDL_Texture;



class Enemy : public Entity
{
public:

	enum eState {
		PATROL = 0,
		CHASING,
		ATTACK,
		DEAD
	};

	enum Direction {
		LEFT,
		RIGHT
	};

	Enemy();
	virtual ~Enemy();

	bool Awake();

	virtual bool Start();

	virtual bool Update(float dt);

	void MovementEnemy(float dt);

	void AttackEnemy(float dt);

	void IdleEnemy(float dt);

	bool TargetReachCheck(Vector2D target, Vector2D pos, Vector2D worldPos);

	void IdleReset();

	bool CleanUp();

	void SetParameters(pugi::xml_node _parameters) {
		this->parameters = _parameters;
	}

	void SetInstanceParameters(pugi::xml_node _instanceParameters) {
		this->instanceParameters = _instanceParameters;
	}

	void SetPlayer(Player* player);

	void SetPosition(Vector2D pos);

	void ResetTarget(Vector2D pos);

	Vector2D GetPosition();

	void ResetPath();

	void SetPath(pugi::xml_node pathNode);

	void OnCollision(PhysBody* physA, PhysBody* physB) override;

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB) override;

	void StopMovement();
	void ResumeMovement();

	b2Body* GetBody() {
		return pbody->body;
	}

	b2Body* GetSensorBody() {
		return sensor->body;
	}

	b2Body* GetAttackSensorBody() {
		return attackSensor->body;
	}

	bool CheckIfTwoPointsNear(Vector2D point1, Vector2D point2, float nearDistance);
public:
	int visible = true;
	bool idleDir = false;
	Vector2D targetTile;


	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;
	pugi::xml_node parameters;
	pugi::xml_node instanceParameters;
	Animation* currentAnimation = nullptr;
	Animation idle, walk, attack;

	PhysBody* pbody;
	PhysBody* sensor;
	PhysBody* attackSensor;

	Pathfinding* pathfinding;
	bool followPlayer = false;
	bool attackPlayer = false;
	bool pauseEnemyIdle = false;
	int pauseCounter = 0;
	float weaponOffset = 100;


	Player* player;
	eState state = PATROL;
	Direction dir = LEFT;

	//path: list of points the soldier moves across
	std::vector<Vector2D> route;
	Vector2D destPoint;
	Vector2D targetWorld;
	int destPointIndex;

	int speed;
	int lives;
	int chaseArea;
	int attackArea;

	Timer deathTimer;
	Timer attackTimer;
	float attackTime;
	float deathTime;
	bool dead;

	float velocity;
};

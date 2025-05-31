#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
#include "Timer.h"
#include "Physics.h"


class Player;
struct SDL_Texture;



class Boss : public Entity
{
public:

	enum eState {
		IDLE,
		RUNNING,
		STUNNED,
		WAITING,
		DEAD
	};

	enum Direction {
		LEFT,
		RIGHT
	};

	Boss();
	virtual ~Boss();

	bool Awake();

	virtual bool Start();

	virtual bool Update(float dt);

	void FixYAxis(float coord);

	bool CleanUp();

	void SetParameters(pugi::xml_node _parameters) {
		this->parameters = _parameters;
	}

	void SetInstanceParameters(pugi::xml_node _instanceParameters) {
		this->instanceParameters = _instanceParameters;
	}

	void SetPlayer(Player* player);

	void SetPosition(Vector2D pos);

	Vector2D GetPosition();

	void ResetPath();

	void OnCollision(PhysBody* physA, PhysBody* physB) override;

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB) override;

	void StopMovement();
	void ResumeMovement();


	bool CheckIfTwoPointsNear(Vector2D point1, Vector2D point2, float nearDistance);

	b2Body* GetBody() {
		return pbody->body;
	}

	b2Body* GetSensorLeftBody() {
		return sensorLeft->body;
	}

	b2Body* GetSensorLimitLeft() {
		return sensorLimitLeft->body;
	}

	b2Body* GetSensorRight() {
		return sensorRight->body;
	}
public:
	int visible = true;



	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;
	pugi::xml_node parameters;
	pugi::xml_node instanceParameters;
	Animation* currentAnimation = nullptr;
	Animation idle;
	Animation die;
	Animation charge;
	Animation loop;
	Animation stunned;
	PhysBody* pbody;
	PhysBody* sensorLeft;
	PhysBody* sensorLimitLeft;
	PhysBody* sensorRight;
	Pathfinding* pathfinding;
	bool followPlayer;
	int contColumn;
	float fixedY;
	bool hasFixedY = false;

	int idleFxId,
		dieFxId,
		gruntFxId,
		impactFxId,
		rockFxId,
		musicId;

	bool music = false;

	Player* player;
	eState state = IDLE;
	eState lastState = IDLE;
	Direction dir = LEFT;

	bool playingsound = false;

	//path: list of points the soldier moves across
	std::vector<Vector2D> route;
	Vector2D destPoint;
	int destPointIndex;

	int speed;
	int lives;
	int chaseArea;
	int attackArea;

	Timer deathTimer;
	float deathTime;
	bool dead;
	float velocity;
	int timer = 0;
	int delay;
};

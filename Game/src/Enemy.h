#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
#include "Timer.h"


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

	void SetPath(pugi::xml_node pathNode);

	void OnCollision(PhysBody* physA, PhysBody* physB) override;

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB) override;

	void StopMovement();
	void ResumeMovement();


	bool CheckIfTwoPointsNear(Vector2D point1, Vector2D point2, float nearDistance);
public:
	int visible = true;



	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;
	pugi::xml_node parameters;
	pugi::xml_node instanceParameters;
	Animation* currentAnimation = nullptr;
	Animation idle;
	PhysBody* pbody;
	Pathfinding* pathfinding;


	Player* player;
	eState state = PATROL;
	Direction dir = LEFT;

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

};

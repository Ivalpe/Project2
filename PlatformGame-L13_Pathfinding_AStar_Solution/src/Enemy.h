#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"

struct SDL_Texture;

enum eState {
	PATROL = 0,
	CHASING
};

class Enemy : public Entity
{
public:

	Enemy();
	virtual ~Enemy();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node _parameters) {
		this->parameters = _parameters;
	}

	void SetInstanceParameters(pugi::xml_node _instanceParameters) {
		this->instanceParameters = _instanceParameters;
	}

	void SetPlayer(Player* _player) {
		this->player = _player;
	}

	void SetPosition(Vector2D pos);

	Vector2D GetPosition();

	void ResetPath();

	void SetPath(pugi::xml_node pathNode);


	void StopMovement();
	void ResumeMovement();
public:
	int visible = true;

private:

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
	eState enemyState = PATROL;
	//path: list of points the soldier moves across
	std::vector<Vector2D> route;
	Vector2D destPoint;

};

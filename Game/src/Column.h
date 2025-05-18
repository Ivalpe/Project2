#pragma once

#include "Physics.h"
#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
#include "Timer.h"

struct SDL_Texture;

class Column : public Entity
{
public:

	enum eState {
		IDLE,
		BREAKING,
		BROKEN
	};

	Column();
	virtual ~Column();

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

	void SetPosition(Vector2D pos);

	Vector2D GetPosition();

	void OnCollision(PhysBody* physA, PhysBody* physB) override;

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB) override;

	b2Body* GetBody() {
		return pbody->body;
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
	PhysBody* pbody;
	int contColumn;
	bool openColumn;

	eState state = IDLE;

	bool dead;
	int timer = 0;
};

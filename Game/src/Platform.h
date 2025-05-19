#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include <vector>
#include "Physics.h"

struct SDL_Texture;

class Platform : public Entity
{
public:

	Platform();
	virtual ~Platform();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void SetPosition(Vector2D pos);

	Vector2D GetPosition();

	void StopMovement();
	void ResumeMovement();

	b2Body* GetBody() {
		return pbody->body;
	}
public:
	PhysBody* pbody;

	std::vector<std::pair<int, int>> PlatformLimits
	{
		{3864 + 300, 4870 + 30},
		{182 +140+70, 3018 + 60},

		{8228+50+20, 11295+60},
	};

private:
	//int startPositionX = 800;
	//int endPositionX = 1000;
	int movement = 5;

	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;
	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idle;
};

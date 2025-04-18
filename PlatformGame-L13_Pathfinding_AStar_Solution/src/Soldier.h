#pragma once

#include "Enemy.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
#include <vector>



struct SDL_Texture;

class Soldier : public Enemy
{
public:

	Soldier();
	~Soldier();


	bool Start() override;

	bool Update(float dt) override;

	void OnCollision(PhysBody* physA, PhysBody* physB) override;
	PhysBody* chaseSensor;

	void MoveToPoint();

private:


	float dist;

	float attackTime;
	Timer attackTimer;
	
};
#include "Soldier.h"
#include "Engine.h"
#include "Textures.h"
#include "Physics.h"
#include "Scene.h"
#include "Player.h"
#include "LOG.h"
#include "Audio.h"

Soldier::Soldier()
{

}

Soldier::~Soldier() {
	
}


bool Soldier::Start() {

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	currentAnimation = &idle;

	//Add a physics to an item - initialize the physics body
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);

	//Assign collider type
	pbody->ctype = ColliderType::ENEMY;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	// Initialize pathfinding
	pathfinding = new Pathfinding();
	ResetPath();

	return true;
}

bool Soldier::Update(float dt)
{
	if (Engine::GetInstance().scene.get()->showPauseMenu == true) return true;

	if (enemyState == PATROL) {

	}


	// Pathfinding testing inputs
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {
		Vector2D pos = GetPosition();
		Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
		pathfinding->ResetPath(tilePos);
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_J) == KEY_DOWN) {
		pathfinding->PropagateBFS();
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_J) == KEY_REPEAT &&
		Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		pathfinding->PropagateBFS();
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_K) == KEY_DOWN) {
		pathfinding->PropagateDijkstra();
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_K) == KEY_REPEAT &&
		Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		pathfinding->PropagateDijkstra();
	}


	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_B) == KEY_DOWN) {
		pathfinding->PropagateAStar(MANHATTAN);
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_B) == KEY_REPEAT &&
		Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		pathfinding->PropagateAStar(MANHATTAN);
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_N) == KEY_DOWN) {
		pathfinding->PropagateAStar(EUCLIDEAN);
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_N) == KEY_REPEAT &&
		Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		pathfinding->PropagateAStar(EUCLIDEAN);
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_M) == KEY_DOWN) {
		pathfinding->PropagateAStar(SQUARED);
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_M) == KEY_REPEAT &&
		Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {
		pathfinding->PropagateAStar(SQUARED);
	}

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	currentAnimation->Update();

	// Draw pathfinding 
	pathfinding->DrawPath();
	return true;
}

void Soldier::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype) {
	case ColliderType::PLAYER:
		break;
	}
}
#include "Enemy.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Map.h"
#include "Player.h"

Enemy::Enemy() : Entity(EntityType::ENEMY)
{

}

Enemy::~Enemy() {
	delete pathfinding;
}

bool Enemy::Awake() {
	return true;
}

bool Enemy::Start() {

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
	//pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);

	////Assign collider type
	//pbody->ctype = ColliderType::ENEMY;
	//pbody->listener = this;

	//// Set the gravity of the body
	//if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	//// Initialize pathfinding
	//pathfinding = new Pathfinding();
	//ResetPath();

	return true;
}

bool Enemy::Update(float dt)
{
	 return true;



	//b2Transform pbodyPos = pbody->body->GetTransform();
	//position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	//position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	//Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	//currentAnimation->Update();

	//// Draw pathfinding 
	//pathfinding->DrawPath();
	return true;
}

bool Enemy::CleanUp()
{
	if (pbody != nullptr) {
		Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
		
		pbody = nullptr;
	}

	pathfinding->CleanUp();
	delete pathfinding;
	return true;
}

void Enemy::SetPlayer(Player* _player)
{
	player = _player;
}

void Enemy::SetPosition(Vector2D pos) {
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Enemy::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void Enemy::ResetPath() {
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	pathfinding->ResetPath(tilePos);
}

void Enemy::StopMovement() {
	if (pbody != nullptr) {
		Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
		pbody = nullptr;
		pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() - 2 + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::STATIC);
		pbody->listener = this;
		pbody->ctype = ColliderType::ENEMY;

	}
}

void Enemy::ResumeMovement() {
	if (pbody == nullptr) {
		Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
		pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() - 2 + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);
		pbody->listener = this;
		pbody->ctype = ColliderType::ENEMY;

	}
}

void Enemy::SetPath(pugi::xml_node pathNode)
{
	route.clear();

	if (pathNode)
	{
		for (pugi::xml_node pointNode : pathNode.children("point")) {

			float x = pointNode.attribute("x").as_float();
			float y = pointNode.attribute("y").as_float();

			route.emplace_back(x, y);
		}

		if (!route.empty()) {
			destPointIndex = 0;
			destPoint = route[destPointIndex];
		}
	
	}
}


bool Enemy::CheckIfTwoPointsNear(Vector2D point1, Vector2D point2, float nearDistance)
{
	b2Vec2 vectorBetweenPoints = { point1.getX() - point2.getX(), point1.getY() - point2.getY() };
	return vectorBetweenPoints.Length() < nearDistance;
}

void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {


	switch (physB->ctype)
	{
	//case ColliderType::WEAPON:
	//	if (enemyState != DEAD) {
	//		
	//	}
	//	break;
	case ColliderType::UNKNOWN:
		break;

	default:
		break;
	}
}

void Enemy::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		break;

	case ColliderType::UNKNOWN:
		break;
	default:
		break;
	}
}
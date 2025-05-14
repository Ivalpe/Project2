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

Boss::Boss() : Entity(EntityType::ENEMY)
{

}

Boss::~Boss() {
	delete pathfinding;
}

bool Boss::Awake() {
	return true;
}

bool Boss::Start() {
	followPlayer = false;
	velocity = 0;
	speed = 4.f;

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();
	speed = parameters.child("properties").attribute("speed").as_int();

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	currentAnimation = &idle;

	//Add a physics to an item - initialize the physics body
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);

	//Sensor
	sensorLeft = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), 10, texH * 3, bodyType::KINEMATIC);
	sensorLeft->ctype = ColliderType::RANGELEFT;
	sensorLeft->listener = this;

	//Sensor
	sensorLimitLeft = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), 10, texH, bodyType::KINEMATIC);
	sensorLimitLeft->ctype = ColliderType::WALLBOSS;
	sensorLimitLeft->listener = this;

	//Sensor
	sensorLimitRight = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), texW / 2, texH, bodyType::KINEMATIC);
	sensorLimitRight->ctype = ColliderType::WALLBOSS;
	sensorLimitRight->listener = this;

	//Sensor
	sensorRight = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), texW * 5, texH * 3, bodyType::KINEMATIC);
	sensorRight->ctype = ColliderType::RANGERIGHT;
	sensorRight->listener = this;

	////Assign collider type
	pbody->ctype = ColliderType::ENEMY;
	pbody->listener = this;

	//// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	//// Initialize pathfinding
	pathfinding = new Pathfinding();
	ResetPath();

	return true;
}

bool Boss::Update(float dt)
{
	if (Engine::GetInstance().scene.get()->showPauseMenu == true || Engine::GetInstance().scene.get()->GameOverMenu == true || Engine::GetInstance().scene.get()->InitialScreenMenu == true) return true;

	velocity = 0;
	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	if (dir == RIGHT) {
		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	}
	else {
		Engine::GetInstance().render.get()->DrawTextureFlipped(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	}
	currentAnimation->Update();

	b2Vec2 enemyPos = pbody->body->GetPosition();
	sensorLeft->body->SetTransform({ enemyPos.x - PIXEL_TO_METERS(32 * 20), enemyPos.y }, 0);
	sensorLimitLeft->body->SetTransform({ PIXEL_TO_METERS(1921), PIXEL_TO_METERS(1828) }, 0);
	sensorRight->body->SetTransform({ enemyPos.x + PIXEL_TO_METERS(32 * 20), enemyPos.y }, 0);
	sensorLimitRight->body->SetTransform({ PIXEL_TO_METERS(6208), PIXEL_TO_METERS(1828) }, 0);



	if (state == RUNNING) {
		velocity = dir == LEFT ? -15.0f : 15.0f;
		timer++;
		if (timer == 180) {
			timer = 0;
			state = WAITING;
		}
	}
	else if (state == WAITING) {
		timer++;
		if (timer == 180) {
			timer = 0;
			state = IDLE;
		}
	}

	pbody->body->SetLinearVelocity({ velocity,0 });

	return true;
}

bool Boss::CleanUp()
{
	if (pbody != nullptr) {
		Engine::GetInstance().physics.get()->DeletePhysBody(pbody);

		pbody = nullptr;
	}

	return true;
}

void Boss::SetPlayer(Player* _player)
{
	player = _player;
}

void Boss::SetPosition(Vector2D pos) {
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Boss::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void Boss::ResetPath() {
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	pathfinding->ResetPath(tilePos);
}

void Boss::StopMovement() {
	if (pbody != nullptr) {
		Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
		pbody = nullptr;
		pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() - 2 + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::STATIC);
		pbody->listener = this;
		pbody->ctype = ColliderType::ENEMY;

	}
}

void Boss::ResumeMovement() {
	if (pbody == nullptr) {
		Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
		pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() - 2 + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);
		pbody->listener = this;
		pbody->ctype = ColliderType::ENEMY;

	}
}

bool Boss::CheckIfTwoPointsNear(Vector2D point1, Vector2D point2, float nearDistance)
{
	b2Vec2 vectorBetweenPoints = { point1.getX() - point2.getX(), point1.getY() - point2.getY() };
	return vectorBetweenPoints.Length() < nearDistance;
}

void Boss::OnCollision(PhysBody* physA, PhysBody* physB) {


	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		if (physA->ctype == ColliderType::RANGELEFT) {

			if (state == IDLE) {
				dir = Direction::LEFT;
				state = RUNNING;
			}
		}
		else if (physA->ctype == ColliderType::RANGERIGHT) {

			if (state == IDLE) {
				dir = Direction::RIGHT;
				state = RUNNING;
			}
		}
		break;
	case ColliderType::UNKNOWN:
		break;

	default:
		break;
	}
}

void Boss::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		break;
	case ColliderType::PLAYER:
		if (physA->ctype == ColliderType::CHASESENSOR) {
			followPlayer = false;
		}
		break;
	case ColliderType::UNKNOWN:
		break;
	default:
		break;
	}
}
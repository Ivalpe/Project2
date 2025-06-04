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
	followPlayer = false;
	velocity = 0;

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	//position.setX(parameters.attribute("x").as_int());
	//position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();
	speed = parameters.child("properties").attribute("speed").as_int();
	attackTime = parameters.child("properties").attribute("attackTime").as_float();

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	walk.LoadAnimations(parameters.child("animations").child("walk"));
	attack.LoadAnimations(parameters.child("animations").child("attack"));
	currentAnimation = &idle;

	//Add a physics to an item - initialize the physics body
	//pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 3, texH / 2, bodyType::DYNAMIC);
	pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX() + texH / 2, (int)position.getY() + texH / 3, texW / 2, texH, bodyType::DYNAMIC);

	//Sensor
	sensor = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY() + texH, texW * 4, texH, bodyType::KINEMATIC);
	sensor->ctype = ColliderType::CHASESENSOR;
	sensor->listener = this;

	//Attack Sensor
	attackSensor = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY() + texH, texW / 2, texH, bodyType::KINEMATIC);
	attackSensor->ctype = ColliderType::ATTACKSENSOR;
	attackSensor->listener = this;

	/*weapon = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY() + texH/2 , 150, 250, bodyType::KINEMATIC);
	weapon->ctype = ColliderType::DAMAGE;
	weapon->listener = this;
	weapon->body->SetEnabled(false);*/

	////Assign collider type
	pbody->ctype = ColliderType::ENEMY;
	pbody->listener = this;

	//// Set the gravity of the body
	pbody->body->SetGravityScale(1.0f);

	//// Initialize pathfinding
	pathfinding = new Pathfinding();
	ResetPath();

	// Initialize idle position
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	targetTile.setY(tilePos.getY());
	targetTile.setX(tilePos.getX() - 12);

	walkFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/soldado/walk.wav");
	attackFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/soldado/attack.wav");

	return true;
}

bool Enemy::Update(float dt)
{

	if (Engine::GetInstance().scene.get()->GameOverMenu == true) {
		//// Initialize pathfinding
		pathfinding = new Pathfinding();
		ResetPath();
	}

	if (Engine::GetInstance().scene.get()->showPauseMenu == true || Engine::GetInstance().scene.get()->GameOverMenu == true || Engine::GetInstance().scene.get()->InitialScreenMenu == true) return true;

	velocity = 0;

	b2Vec2 enemyPos = pbody->body->GetPosition();
	if (sensor and sensor->body) sensor->body->SetTransform({ enemyPos.x, enemyPos.y }, 0);
	if (attackSensor and attackSensor->body) attackSensor->body->SetTransform({ enemyPos.x, enemyPos.y }, 0);

	if (followPlayer) {
		MovementEnemy(dt);
		if (!playingsound) {
			Engine::GetInstance().audio.get()->PlayFx(walkFxId, 0, 3);
			playingsound = true;
		}
	}
	else if (attackPlayer) {
		AttackEnemy(dt);
		Engine::GetInstance().audio.get()->PlayFx(attackFxId);
	}
	else {
		IdleEnemy(dt);
		if (playingsound) {
			Engine::GetInstance().audio.get()->StopFxByChannel(3);
			playingsound = false;
		}
	}

	/*if (attackPlayer) weapon->body->SetEnabled(true);
	else weapon->body->SetEnabled(false);*/

	currentAnimation->Update();

	b2Vec2 vel = pbody->body->GetLinearVelocity();
	pbody->body->SetLinearVelocity({ velocity, vel.y });
	//weapon->body->SetLinearVelocity({ velocity,0 });

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);


	if (dir == RIGHT) {
		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
		weaponOffset = 200;
	}
	else {
		Engine::GetInstance().render.get()->DrawTextureFlipped(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
		weaponOffset = -50;
	}

	//weapon->body->SetTransform({ enemyPos.x + weaponOffset, enemyPos.y }, 0);

	const float maxAngle = 2.0f * (float)M_PI / 180.0f;
	float angle = pbody->body->GetAngle();

	if (angle > maxAngle) {
		pbody->body->SetTransform(pbody->body->GetPosition(), maxAngle);
		pbody->body->SetAngularVelocity(0);
	}
	else if (angle < -maxAngle) {
		pbody->body->SetTransform(pbody->body->GetPosition(), -maxAngle);
		pbody->body->SetAngularVelocity(0);
	}

	return true;
}

void Enemy::IdleReset() {
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap((int)pos.getX() + 32, (int)pos.getY() + 32);

	targetTile.setY(tilePos.getY());

	if (idleDir) {
		idleDir = false;
		targetTile.setX(targetTile.getX() - 10);
	}
	else {
		idleDir = true;
		targetTile.setX(targetTile.getX() + 10);
	}
}

bool Enemy::TargetReachCheck(Vector2D target, Vector2D pos, Vector2D worldPos) {

	if (targetTile.getX() == pos.getX()) {
		return true;
	}
	if (targetTile.getX() <= pos.getX() and (int)targetTile.getX() + 1 >= pos.getX()) {
		//SetPosition(targetWorld);
		return true;
	}
	if ((int)targetTile.getX() - 1 <= pos.getX() and targetTile.getX() >= pos.getX()) {
		//SetPosition(targetWorld);
		return true;
	}
	return false;
}

void Enemy::IdleEnemy(float dt) {
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	targetTile.setY(tilePos.getY());

	if (TargetReachCheck(targetTile, tilePos, pos)) {
		targetWorld = Engine::GetInstance().map.get()->MapToWorld(targetTile.getX(), targetTile.getY());
		IdleReset();
		pauseEnemyIdle = true;
	}

	if (pauseEnemyIdle) {
		if (pauseCounter >= PAUSETIME) {
			targetWorld.setY(pos.getY());
			SetPosition(targetWorld);
			pauseCounter = 0;
			pauseEnemyIdle = false;
		}
		else {
			if (currentAnimation != &idle) {
				currentAnimation = &idle;
			}
			pauseCounter++;
		}
	}
	else {
		pathfinding->ResetPath(tilePos);

		int max = 200;
		bool found = false;
		while (!found) {
			found = pathfinding->PropagateAStar(MANHATTAN, targetTile);
			max--;
			if (max == 0) break;
			if (Engine::GetInstance().physics.get()->GetDebug())
				pathfinding->DrawPath();
		}

		if (found) {

			int sizeBread = pathfinding->breadcrumbs.size();
			Vector2D posBread;
			if (sizeBread >= 2) posBread = pathfinding->breadcrumbs[pathfinding->breadcrumbs.size() - 2];
			else posBread = pathfinding->breadcrumbs[pathfinding->breadcrumbs.size() - 1];

			//Movement Enemy
			if (posBread.getX() <= tilePos.getX()) {
				velocity = -speed;
				dir = LEFT;
			}
			else {
				velocity = speed;
				dir = RIGHT;
			}

			currentAnimation = &walk;
		}

	}

}

void Enemy::MovementEnemy(float dt) {
	//Reset
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	pathfinding->ResetPath(tilePos);

	int max = 200;
	bool found = false;
	while (!found) {
		found = pathfinding->PropagateAStar(MANHATTAN);
		max--;
		if (max == 0) break;
		if (Engine::GetInstance().physics.get()->GetDebug())
			pathfinding->DrawPath();
	}

	if (found) {
		int sizeBread = pathfinding->breadcrumbs.size();
		Vector2D posBread;
		if (sizeBread >= 2) posBread = pathfinding->breadcrumbs[pathfinding->breadcrumbs.size() - 2];
		else posBread = pathfinding->breadcrumbs[pathfinding->breadcrumbs.size() - 1];

		//Movement Enemy
		if (posBread.getX() <= tilePos.getX()) {
			velocity = -speed;
			dir = LEFT;
		}
		else {
			velocity = speed;
			dir = RIGHT;
		}

		currentAnimation = &walk;
	}

}

void Enemy::AttackEnemy(float dt) {

	if (attack.HasFinished() && attackTimer.ReadSec() >= attackTime) {
		currentAnimation = &idle;
		attack.Reset();
		attackPlayer = false;
	}

	int frame = static_cast<int>(attack.currentFrame);
	if (frame == 6 or frame == 5) {
		if (Engine::GetInstance().scene.get()->GetPlayer()->isInAttackSensor) {
			Vector2D enemyPos = GetPosition();
			Engine::GetInstance().scene.get()->GetPlayer()->TakeDamage();
		}
	}


	/*if (currentAnimation != &attack) {
		attack.Reset();
		currentAnimation = &attack;
		attackTimer.Start();
		weapon->body->SetEnabled(true);
	}
	else {
		if (attackTimer.ReadSec() >= attackTime / 2) {
			weapon->body->SetEnabled(false);
		}

		if (attack.HasFinished() && attackTimer.ReadSec() >= attackTime) {
			currentAnimation = &idle;
			attackPlayer = false;
		}
	}*/



}

bool Enemy::CleanUp()
{
	if (pbody != nullptr) {
		Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
		Engine::GetInstance().physics.get()->DeletePhysBody(attackSensor);
		Engine::GetInstance().physics.get()->DeletePhysBody(sensor);
		pbody->listener = NULL;
		pbody->body = nullptr;
		pbody = nullptr;
		attackSensor = nullptr;
		sensor = nullptr;
	}

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

void Enemy::ResetTarget(Vector2D pos) {
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	targetTile.setY(tilePos.getY());
	targetTile.setX(tilePos.getX() - 12);
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
		pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texW / 2, texH, bodyType::KINEMATIC);
		pbody->listener = this;
		pbody->ctype = ColliderType::ENEMY;

	}
}

void Enemy::ResumeMovement() {
	if (pbody == nullptr) {
		Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
		pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texW / 2, texH, bodyType::KINEMATIC);
		pbody->listener = this;
		pbody->ctype = ColliderType::ENEMY;

	}
}

void Enemy::SetPath(pugi::xml_node pathNode)
{
	/*route.clear();

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

	}*/
}


bool Enemy::CheckIfTwoPointsNear(Vector2D point1, Vector2D point2, float nearDistance)
{
	b2Vec2 vectorBetweenPoints = { point1.getX() - point2.getX(), point1.getY() - point2.getY() };
	return vectorBetweenPoints.Length() < nearDistance;
}

void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {


	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		if (physA->ctype == ColliderType::CHASESENSOR and !attackPlayer) {
			followPlayer = true;
		}
		else if (physA->ctype == ColliderType::ATTACKSENSOR) {

			if (!attackPlayer) {
				attackPlayer = true;
				currentAnimation = &attack;
				followPlayer = false;
			}
		}
		break;
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
	case ColliderType::PLAYER:
		if (physA->ctype == ColliderType::CHASESENSOR) {
			followPlayer = false;
			pauseEnemyIdle = false;
			IdleReset();
		}
		if (physA->ctype == ColliderType::ATTACKSENSOR) {
			attackPlayer = false;
		}
		//IdleReset();
		break;
	case ColliderType::UNKNOWN:
		break;
	default:
		break;
	}
}
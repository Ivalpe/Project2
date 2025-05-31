#include "Boss.h"
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
	contColumn = 2;
	followPlayer = false;
	velocity = 0;
	state = IDLE;
	delay = 30;

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();
	speed = parameters.child("properties").attribute("speed").as_int();

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	die.LoadAnimations(parameters.child("animations").child("die"));
	charge.LoadAnimations(parameters.child("animations").child("charge"));
	loop.LoadAnimations(parameters.child("animations").child("loop"));
	stunned.LoadAnimations(parameters.child("animations").child("dizzy"));
	currentAnimation = &idle;

	//Add a physics to an item - initialize the physics body
	pbody = Engine::GetInstance().physics.get()->CreateCircleSensor((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);

	//Sensor
	sensorLeft = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), texW / 3, texH, bodyType::KINEMATIC);
	sensorLeft->ctype = ColliderType::RANGELEFT;
	sensorLeft->listener = this;

	//Sensor
	sensorLimitLeft = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), 10, texH, bodyType::KINEMATIC);
	sensorLimitLeft->ctype = ColliderType::WALLBOSS;
	sensorLimitLeft->listener = this;

	//Sensor
	sensorRight = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), texW / 3, texH, bodyType::KINEMATIC);
	sensorRight->ctype = ColliderType::RANGERIGHT;
	sensorRight->listener = this;

	////Assign collider type
	pbody->ctype = ColliderType::BOSS;
	pbody->listener = this;

	//// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	//// Initialize pathfinding
	pathfinding = new Pathfinding();
	ResetPath();

	dieFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/minotauro/death.wav");
	idleFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/minotauro/idle.wav");
	gruntFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/minotauro/grunt.wav");
	impactFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/minotauro/impacto.wav");
	musicId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/minotauro/boss.wav");
	

	return true;
}

void Boss::FixYAxis(float coord) {
	int y = (int)PIXEL_TO_METERS(coord) + PIXEL_TO_METERS(texH / 2) - 1;
	fixedY = PIXEL_TO_METERS(coord) + PIXEL_TO_METERS(texH / 2) - 2.2;
	hasFixedY = true;
}

bool Boss::Update(float dt)
{
	if (Engine::GetInstance().scene.get()->showPauseMenu == true || Engine::GetInstance().scene.get()->GameOverMenu == true || Engine::GetInstance().scene.get()->InitialScreenMenu == true) return true;

	if (!hasFixedY) {
		fixedY = pbody->body->GetPosition().y;
		hasFixedY = true;
	}

	velocity = 0;
	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	if (state != lastState)
	{
		lastState = state;
		playingsound = false;
		Engine::GetInstance().audio.get()->StopFxByChannel(4);

	}

	// --- Animación y lógica de estado ---
	switch (state)
	{
	case RUNNING:
		// Si la animación actual no es charge, la asignamos y la reiniciamos
		if (currentAnimation != &charge && currentAnimation != &loop) {
			currentAnimation = &charge;
			charge.Reset();
		}
		if (!playingsound)
		{
			
			Engine::GetInstance().audio.get()->PlayFx(gruntFxId, 0, 4);
			playingsound = true;
		}
		// Si está en charge y terminó, pasamos a loop
		else if (currentAnimation == &charge && charge.HasFinished()) {
			currentAnimation = &loop;
			loop.Reset();
		}
		velocity = dir == LEFT ? -15.0f : 15.0f;
		break;

	case WAITING:
		timer++;
		if (timer == 180) {
			timer = 0;
			state = IDLE;
		}
		currentAnimation = &idle;
		break;

	case DEAD:
		currentAnimation = &die;
		if (!playingsound)
		{
			Engine::GetInstance().audio.get()->PlayFx(dieFxId, 0, 4);
			playingsound = true;
		}

		Engine::GetInstance().audio.get()->StopFxByChannel(5);
		
		break;

	case IDLE:
		if (!playingsound)
		{
			Engine::GetInstance().audio.get()->PlayFx(idleFxId, 0, 4);
			playingsound = true;
		}
		currentAnimation = &idle;
		break;
	case STUNNED:
		if (!playingsound)
		{
			Engine::GetInstance().audio.get()->PlayFx(impactFxId, 0, 4);
			playingsound = true;
		}
		if (!music)
		{
			Engine::GetInstance().audio.get()->PlayFx(musicId, 0, 5);
			music = true;
		}
		currentAnimation = &stunned;
		if (stunned.HasFinished()) {
			state = WAITING;
			currentAnimation = &idle;
			stunned.Reset();
		}
		break;
	default:
		break;
	}

	// Dibujo y actualización de animación
	if (dir == RIGHT) {
		Engine::GetInstance().render.get()->DrawTextureFlipped(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	}
	else {
		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	}
	currentAnimation->Update();

	// Resto de la lógica de física y sensores
	b2Vec2 enemyPos = pbody->body->GetPosition();
	pbody->body->SetTransform(b2Vec2(enemyPos.x, fixedY), pbody->body->GetAngle());
	sensorLeft->body->SetTransform({ enemyPos.x - PIXEL_TO_METERS(32 * 27), enemyPos.y }, 0);
	sensorLimitLeft->body->SetTransform({ PIXEL_TO_METERS(1921), PIXEL_TO_METERS(1828) }, 0);
	sensorRight->body->SetTransform({ enemyPos.x + PIXEL_TO_METERS(32 * 27), enemyPos.y }, 0);

	if (contColumn <= 0) {
		state = DEAD;
		pbody->ctype = ColliderType::ENEMY_DEATH;
	}

	if (delay > 0) delay--;

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
		pbody->ctype = ColliderType::BOSS;

	}
}

void Boss::ResumeMovement() {
	if (pbody == nullptr) {
		Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
		pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() - 2 + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);
		pbody->listener = this;
		pbody->ctype = ColliderType::BOSS;

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
		if (delay <= 0 && state != DEAD) {
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
		}
		break;
	case ColliderType::UNKNOWN:
		break;
	case ColliderType::WALLBOSS:
		if (physA->ctype == ColliderType::BOSS) {
			state = STUNNED;
		}
		break;
	case ColliderType::WALLBOSSDES:
		if (physA->ctype == ColliderType::BOSS) {
			state = STUNNED;
			contColumn--;
		}
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
		break;
	case ColliderType::UNKNOWN:
		break;
	default:
		break;
	}
}
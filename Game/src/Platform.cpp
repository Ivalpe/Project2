#include "Platform.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Map.h"
#include "Entity.h"
Platform::Platform() : Entity(EntityType::PLATFORM)
{

}

Platform::~Platform() {
}

bool Platform::Awake() {
	return true;
}

bool Platform::Start() {

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());

	if (!texture) {
		LOG("Error al cargar la textura de la plataforma.");
		return false;  // Si no se carga la textura, no continuar
	}

	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	currentAnimation = &idle;

	//Add a physics to an item - initialize the physics body
	pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX() + texW / 2, (int)position.getY() + texH / 2, texW, texH, bodyType::KINEMATIC);
	pbody->listener = this;

	// Set the gravity of the body
	pbody->body->SetGravityScale(0);

	//Assign collider type
	pbody->ctype = ColliderType::M_PLATFORM;

	if (name == "platform0") {
		position.setY(PlatformY[0]);
		position.setX(PlatformLimits[0].first);
		b2Vec2 currentPos = pbody->body->GetPosition();
		pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(PlatformLimits[0].first), PIXEL_TO_METERS(PlatformY[0])), 0);
	}
	else if (name == "platform1") {
		position.setY(PlatformY[1]);
		position.setX(PlatformLimits[1].first);
		b2Vec2 currentPos = pbody->body->GetPosition();
		pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(PlatformLimits[1].first), PIXEL_TO_METERS(PlatformY[1])), 0);
	}
	else if (name == "platform2") {
		position.setY(PlatformY[2]);
		position.setX(PlatformLimits[2].first);
		b2Vec2 currentPos = pbody->body->GetPosition();
		pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(PlatformLimits[2].first), PIXEL_TO_METERS(PlatformY[2])), 0);
	}
	return true;
}


bool Platform::Update(float dt)
{
	if (Engine::GetInstance().scene.get()->showPauseMenu == true || Engine::GetInstance().scene.get()->GameOverMenu == true || Engine::GetInstance().scene.get()->InitialScreenMenu == true || Engine::GetInstance().scene.get()->level == 0) return true;


	int startPositionX;
	int endPositionX;
	if (name == "platform0") {
		startPositionX = PlatformLimits[0].first;
		endPositionX = PlatformLimits[0].second;
	}
	if (name == "platform1") {
		startPositionX = PlatformLimits[1].first;
		endPositionX = PlatformLimits[1].second;
	}
	if (name == "platform2") {
		startPositionX = PlatformLimits[2].first;
		endPositionX = PlatformLimits[2].second;
	}

	b2Vec2 velocity = b2Vec2(pbody->body->GetLinearVelocity().x, 0);

	if (position.getX() <= startPositionX && position.getX() != endPositionX) velocity.x = movement; //Movement to the right
	else if (position.getX() >= endPositionX) velocity.x = -movement; // Movement to the left  
	pbody->body->SetLinearVelocity(velocity);






	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	currentAnimation->Update();

	LOG("Platform position: %f, %f", position.getX(), position.getY());
	return true;
}


bool Platform::CleanUp()
{
	return true;
}

void Platform::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	if (pbody->body != nullptr) {
		pbody->body->SetTransform(bodyPos, 0);
	}
}

Vector2D Platform::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void Platform::StopMovement() {
	if (pbody != nullptr) {
		Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
		pbody = nullptr;
		pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX() + texW / 2, (int)position.getY() + texH / 2, texW / 2, texH, bodyType::STATIC);
		pbody->listener = this;
		pbody->ctype = ColliderType::M_PLATFORM;
	}
}

void Platform::ResumeMovement() {
	if (pbody != nullptr) {
		Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
		pbody = nullptr;
		pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX() + texW / 2, (int)position.getY() + texH / 2, texW / 2, texH, bodyType::KINEMATIC);
		pbody->listener = this;
		pbody->ctype = ColliderType::M_PLATFORM;
		b2Vec2 velocity = b2Vec2(movement, 0);
		pbody->body->SetLinearVelocity(velocity);
	}
}
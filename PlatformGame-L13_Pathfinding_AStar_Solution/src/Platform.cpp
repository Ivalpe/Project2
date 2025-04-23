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
	pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX(), (int)position.getY(), texW, texH, bodyType::KINEMATIC);
	pbody->listener = this;

	// Set the gravity of the body
	pbody->body->SetGravityScale(0);

	//Assign collider type
	pbody->ctype = ColliderType::M_PLATFORM;

	return true;
}

bool Platform::Update(float dt)
{
	if (Engine::GetInstance().scene.get()->showPauseMenu == true || Engine::GetInstance().scene.get()->GameOverMenu == true || Engine::GetInstance().scene.get()->InitialScreenMenu == true || Engine::GetInstance().scene.get()->level == 0) return true;

	//if(Engine::GetInstance().scene.get()->)
	//Platform moving

	for (int i = 0; i < PlatformLimits.size(); i++)
	{
		int startPositionX = PlatformLimits[i].first;
		int endPositionX = PlatformLimits[i].second;

		b2Vec2 velocity = b2Vec2(pbody->body->GetLinearVelocity().x, 0);

		if (position.getX() <= startPositionX && position.getX() != endPositionX) velocity.x = movement; //Movement to the right
		else if (position.getX() >= endPositionX) velocity.x = -movement; // Movement to the left  
		pbody->body->SetLinearVelocity(velocity);

	}

	

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	currentAnimation->Update();

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

#include "Item.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"


Item::Item() : Entity(EntityType::ITEM)
{
	name = "item";
}

Item::~Item() {}

bool Item::Awake() {
	return true;
}

bool Item::Start() {

	name = parameters.attribute("name").as_string();

	//initilize textures
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();
	isPicked = parameters.attribute("isPicked").as_bool(false);
	isWax = parameters.attribute("isWax").as_bool(false);
	isFeather = parameters.attribute("isFeather").as_bool(false);
	isCloth = parameters.attribute("isCloth").as_bool(false);
	isGloves = parameters.attribute("isGloves").as_bool(false);

	//Initialize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	Feather_texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture1").as_string());


	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	currentAnimation = &idle;

	idle_feather.LoadAnimations(parameters.child("animations").child("idle_feather"));
	currentAnimation_feather = &idle_feather;


	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2 - 5, texH / 2, bodyType::DYNAMIC);
	pbody->listener = this;

	pbody->ctype = ColliderType::ITEM;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);


	return true;
}

bool Item::Update(float dt)
{
	if (Engine::GetInstance().scene.get()->showPauseMenu == true || Engine::GetInstance().scene.get()->GameOverMenu == true || Engine::GetInstance().scene.get()->InitialScreenMenu == true) return true;

	if ((isWax || isCloth || isGloves) && !isPicked) {
		Engine::GetInstance().render.get()->DrawTextureFlipped(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());

		currentAnimation->Update();
	}

	if (isFeather && !isPicked) {
		Engine::GetInstance().render.get()->DrawTexture(Feather_texture, (int)position.getX(), (int)position.getY(), &currentAnimation_feather->GetCurrentFrame());
		currentAnimation_feather->Update();
	}

	if (!isPicked) {
		b2Transform pbodyPos = pbody->body->GetTransform();
		position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
		position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);
	}

	return true;
}

bool Item::CleanUp()
{
	if (pbody != nullptr) {
		//Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
		pbody = nullptr;
	}
	Engine::GetInstance().textures.get()->UnLoad(texture);
	Engine::GetInstance().textures.get()->UnLoad(Feather_texture);

	return true;
}

void Item::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	if (bodyB->ctype == ColliderType::PLAYER && !isPicked)
	{
		Player* player = Engine::GetInstance().scene.get()->GetPlayer();

		if (name == "wax") {
			isPicked = true;
			Engine::GetInstance().entityManager->wax++;
			Engine::GetInstance().scene.get()->shouldFillWaxy = true;

			if (pbody != nullptr) {
				Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
				pbody = nullptr;
			}
		}
		else if (name == "feather") {
			isPicked = true;
			Engine::GetInstance().entityManager->feather++;
			Engine::GetInstance().scene.get()->shouldFillWaxy = false;

			if (pbody != nullptr) {
				Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
				pbody = nullptr;
			}
		}
		else if (name == "cloth") {
			isPicked = true;
			player->UnlockSkill("Glide");
			if (pbody != nullptr) {
				Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
				pbody = nullptr;
			}
		}
		else if (name == "gloves") {
			isPicked = true;
			player->UnlockSkill("Climb");
			if (pbody != nullptr) {
				Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
				pbody = nullptr;
			}
		}
	}
}

void Item::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	if (pbody->body != nullptr) {
		pbody->body->SetTransform(bodyPos, 0);
	}
}
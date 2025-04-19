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
	

	//Initialize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	Feather_texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture1").as_string());
	

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	currentAnimation = &idle;
	
	idle_feather.LoadAnimations(parameters.child("animations").child("idle_feather"));
	currentAnimation_feather = &idle_feather;


	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);
	pbody->ctype = ColliderType::ITEM;
	
	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);
	
	
	return true;
}

bool Item::Update(float dt)
{
	if (Engine::GetInstance().scene.get()->showPauseMenu == true || Engine::GetInstance().scene.get()->GameOverMenu == true) return true;


	Player* player = Engine::GetInstance().scene.get()->GetPlayer();
	if (player != nullptr)
	{
		// Comprobar la distancia entre el �tem y el jugador
		float distance = sqrt(pow(position.getX() - player->position.getX(), 2) + pow(position.getY() - player->position.getY(), 2));
		if (isWax && distance < 224.5f && isPicked == 0) {
			LOG("%f", distance);

			isPicked = 1;
			Engine::GetInstance().entityManager->wax++;
			player->UpdateWaxToCandle();
			LOG("�Item recogido! Wax actual: %d", Engine::GetInstance().entityManager->wax);

		}
		if (isFeather && distance < 281.0f && isPicked == 0) {

			isPicked = 1;
			Engine::GetInstance().entityManager->feather++;
			LOG("�Item recogido! Wax actual: %d", Engine::GetInstance().entityManager->feather);

		}
	}
	if (isWax && isPicked == 0) {
		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
		currentAnimation->Update();
	}
	if (isFeather && isPicked == 0) {
		Engine::GetInstance().render.get()->DrawTexture(Feather_texture, (int)position.getX(), (int)position.getY(), &currentAnimation_feather->GetCurrentFrame());
		currentAnimation_feather->Update();
	}

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);
	

	return true;
}

bool Item::CleanUp()
{
	if (pbody != nullptr) {
		Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
		pbody = nullptr;
	}
	Engine::GetInstance().textures.get()->UnLoad(texture);
	Engine::GetInstance().textures.get()->UnLoad(Feather_texture);

	return true;
}


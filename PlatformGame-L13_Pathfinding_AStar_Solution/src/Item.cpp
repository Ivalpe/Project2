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
	//position.setX(parameters.attribute("x").as_int());
	//position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();
	isPicked = parameters.attribute("isPicked").as_bool(false);
	isWax = parameters.attribute("isWax").as_bool(false);
	isFeather = parameters.attribute("isFeather").as_bool(false);
	isStalactites = parameters.attribute("isStalactites").as_bool(false);

	//Initialize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	Feather_texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture1").as_string());
	Stalactites_texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture2").as_string());


	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	currentAnimation = &idle;
	
	idle_feather.LoadAnimations(parameters.child("animations").child("idle_feather"));
	currentAnimation_feather = &idle_feather;

	idle_Stalactites.LoadAnimations(parameters.child("animations").child("idle_stalactites"));
	currentAnimation_stalactities = &idle_Stalactites;

	idle_stalactites_falls.LoadAnimations(parameters.child("animations").child("idle_stalactites_falls"));

	// L08 TODO 4: Add a physics to an item - initialize the physics body
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);

	pbody->listener = this;

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::ITEM;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);
	
	
	return true;
}

bool Item::Update(float dt)
{
	if (Engine::GetInstance().scene.get()->showPauseMenu == true || Engine::GetInstance().scene.get()->GameOverMenu == true) return true;

	// L08 TODO 4: Add a physics to an item - update the position of the object from the physics.  

	Player* player = Engine::GetInstance().scene.get()->GetPlayer();
	if (player != nullptr)
	{
		// Comprobar la distancia entre el ítem y el jugador
		float distance = sqrt(pow(position.getX() - player->position.getX(), 2) + pow(position.getY() - player->position.getY(), 2));
		float drop = fabs(position.getX() - player->position.getX());
		if (isWax && distance < 120.0f && isPicked == 0) {
			isPicked = 1;
			Engine::GetInstance().entityManager->wax++;
			LOG("¡Item recogido! Wax actual: %d", Engine::GetInstance().entityManager->wax);

		}
		if (isFeather && distance < 130.0f && isPicked == 0) {
			isPicked = 1;
			Engine::GetInstance().entityManager->feather++;
			LOG("¡Item recogido! Wax actual: %d", Engine::GetInstance().entityManager->feather);

		}

		if (isStalactites && drop < 200 && isPicked == 0)
		{
			pbody->ctype = ColliderType::DAMAGE;
			pbody->body->SetGravityScale(1);			
			b2Vec2 force(0.0f, -10.0f);
			pbody->body->ApplyForceToCenter(force, true);
			isPicked = 1;
			changecolision = true;
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
	if (isStalactites) {

		if (changecolision && pbody->ctype == ColliderType::PLATFORM) {
			Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
			pbody = nullptr;

			pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX() + texH / 2, (int)position.getY() + texH,64,32, bodyType::STATIC);		
			pbody->listener = this;

			changecolision = false;
		}

		Engine::GetInstance().render.get()->DrawTexture(Stalactites_texture, (int)position.getX(), (int)position.getY()+16, &currentAnimation_stalactities->GetCurrentFrame());
		currentAnimation_stalactities->Update();
		
	}


	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
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
	Engine::GetInstance().textures.get()->UnLoad(Stalactites_texture);

	return true;
}
void Item::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	if (bodyA == pbody && bodyB->ctype == ColliderType::PLATFORM)
	{
		if (name == "stalactites") {			
			pbody->ctype = ColliderType::PLATFORM;
			currentAnimation_stalactities = &idle_stalactites_falls;
		}
		
	}
}

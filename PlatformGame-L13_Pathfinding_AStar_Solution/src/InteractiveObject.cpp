#include "InteractiveObject.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"


InteractiveObject::InteractiveObject() : Entity(EntityType::INTERACTIVEOBJECT)
{
	name = "interactiveObject";
}

InteractiveObject::~InteractiveObject() {}

bool InteractiveObject::Awake() {
	return true;
}

bool InteractiveObject::Start() {

	name = parameters.attribute("name").as_string();

	//initilize textures
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();
	isPicked = parameters.attribute("isPicked").as_bool(false);
	isStalactites = parameters.attribute("isStalactites").as_bool(false);
	isWall = parameters.attribute("isWall").as_bool(false);

	Stalactites_texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture2").as_string());
	Wall_texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture3").as_string());


	//Load animations

	idle_Stalactites.LoadAnimations(parameters.child("animations").child("idle_stalactites"));
	currentAnimation_stalactities = &idle_Stalactites;

	idle_stalactites_falls.LoadAnimations(parameters.child("animations").child("idle_stalactites_falls"));

	idle_wall.LoadAnimations(parameters.child("animations").child("idle_wall"));
	currentAnimation_wall = &idle_wall;

	idle_stalactites_falls.LoadAnimations(parameters.child("animations").child("idle_stalactites_falls"));
	idle_raise.LoadAnimations(parameters.child("animations").child("idle_raise"));


	if (name == "wall")
	{
		pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX() + texW / 2, (int)position.getY() - texH / 2, texW, texH, bodyType::STATIC);
		

	}
	else {
		pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::STATIC);

	}

	pbody->listener = this;
	pbody->ctype = ColliderType::PLATFORM;
	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);


	return true;
}

bool InteractiveObject::Update(float dt)
{
	if (Engine::GetInstance().scene.get()->showPauseMenu == true || Engine::GetInstance().scene.get()->GameOverMenu == true) return true;


	Player* player = Engine::GetInstance().scene.get()->GetPlayer();
	if (player != nullptr)
	{
		// Comprobar la distancia entre el ?tem y el jugador
		float distance = sqrt(pow(position.getX() - player->position.getX(), 2) + pow(position.getY() - player->position.getY(), 2));
		float drop = fabs(position.getX() - player->position.getX());

		if (isStalactites && drop < 200 && isPicked == 0)
		{
			pbody->body->SetType(b2_dynamicBody);
			pbody->ctype = ColliderType::DAMAGE;
			pbody->body->SetGravityScale(1);
			b2Vec2 force(0.0f, -10.0f);
			pbody->body->ApplyForceToCenter(force, true);
			isPicked = 1;
			changecolision = true;
		}

		if (isWall && distance < 370.0f && isPicked == 0)
		{
			LOG("%d", distance);
			blockText = true;

		}
		else {
			blockText = false;

		}
	}
	if (isStalactites) {

		if (changecolision && pbody->ctype == ColliderType::PLATFORM) {
			
			Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
			pbody = nullptr;

			pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX() + texW / 2, (int)position.getY() + texH, 64, 32, bodyType::DYNAMIC);

			pbody->listener = this;
			changecolision = false;

			if (!damageStalactite) {

				pbody->body->SetType(b2_staticBody);


			}
			
		}


		Engine::GetInstance().render.get()->DrawTexture(Stalactites_texture, (int)position.getX(), (int)position.getY() + 16, &currentAnimation_stalactities->GetCurrentFrame());

		currentAnimation_stalactities->Update();

	}

	if (isWall) {

		if (!Wallraise && Engine::GetInstance().entityManager->feather >= 2)
		{

			currentAnimation_wall = &idle_raise;

			Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
			pbody = nullptr;
			Wallraise = true;


		}

		Engine::GetInstance().render.get()->DrawTexture(Wall_texture, (int)position.getX(), (int)position.getY(), &currentAnimation_wall->GetCurrentFrame());
		currentAnimation_wall->Update();

		if (blockText == true)
		{
			int textWidthSentence, textHeightSentence;

			if (!Wallraise && Engine::GetInstance().entityManager->feather >= 2)
			{
				TTF_SizeText(Engine::GetInstance().render.get()->font, "Ikaros, tiene plumas", &textWidthSentence, &textHeightSentence);

			}
			else {
				TTF_SizeText(Engine::GetInstance().render.get()->font, "Ikaros, busca plumas", &textWidthSentence, &textHeightSentence);

			}

			float scale = 0.5;
			SDL_Rect Sentence = { 960 - 20, 850 + 10,  static_cast<int>(textWidthSentence * scale),  static_cast<int>(textHeightSentence * scale) };

			SDL_Rect Rect = { 960 - 20, 850 + 10, static_cast<int>(textWidthSentence * scale), static_cast<int>(textHeightSentence * scale) };
			SDL_SetRenderDrawColor(Engine::GetInstance().render.get()->renderer, 0, 0, 0, 150);
			SDL_RenderFillRect(Engine::GetInstance().render.get()->renderer, &Rect);


			if (Engine::GetInstance().entityManager->feather >= 2)
			{
				LOG("Tiene plumas");
				Engine::GetInstance().render.get()->DrawText("Ikaros, tiene plumas", Sentence.x, Sentence.y, Sentence.w, Sentence.h);

				isPicked = 1;

			}
			else {
				Engine::GetInstance().render.get()->DrawText("Ikaros, busca plumas", Sentence.x, Sentence.y, Sentence.w, Sentence.h);

			}
		}

	}

	if (pbody != NULL) {
		b2Transform pbodyPos = pbody->body->GetTransform();
		position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
		position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);
	}

	return true;
}

bool InteractiveObject::CleanUp()
{
	if (pbody != nullptr) {
		Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
		pbody = nullptr;
	}

	Engine::GetInstance().textures.get()->UnLoad(Stalactites_texture);
	Engine::GetInstance().textures.get()->UnLoad(Wall_texture);

	return true;
}
void InteractiveObject::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{

	if ((bodyA == pbody && bodyA->ctype == ColliderType::DAMAGE && bodyB->ctype == ColliderType::PLATFORM) ||
		(bodyB == pbody && bodyB->ctype == ColliderType::DAMAGE && bodyA->ctype == ColliderType::PLATFORM))
	{
		if (name == "stalactites") {
			pbody->ctype = ColliderType::PLATFORM;
			currentAnimation_stalactities = &idle_stalactites_falls;
			if (position.getY() > 1830)	damageStalactite = false;
		}

	}
	if ((bodyA == pbody && bodyA->ctype == ColliderType::DAMAGE && bodyB->ctype == ColliderType::PLAYER) ||
		(bodyB == pbody && bodyB->ctype == ColliderType::DAMAGE && bodyA->ctype == ColliderType::PLAYER))
	{
		damageStalactite = true;
	}
}

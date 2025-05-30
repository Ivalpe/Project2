#include "Column.h"
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

Column::Column() : Entity(EntityType::COLUMN)
{
}

Column::~Column() {
}

bool Column::Awake() {
	return true;
}

bool Column::Start() {

	contColumn = 2;
	openColumn = false;

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
	if(!parameters.attribute("damage").as_bool()){ 
		pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX(), (int)position.getY(), texW, texH, bodyType::STATIC);

		////Assign collider type
		pbody->ctype = ColliderType::WALLBOSSDES;
		pbody->listener = this;
	}
	else {
		pbody = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), texW, texH, bodyType::STATIC);

		////Assign collider type
		pbody->ctype = ColliderType::DAMAGE_LIGHT;
		pbody->listener = this;
	}
	//// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	return true;
}

bool Column::Update(float dt)
{
	if (Engine::GetInstance().scene.get()->showPauseMenu == true || Engine::GetInstance().scene.get()->GameOverMenu == true || Engine::GetInstance().scene.get()->InitialScreenMenu == true) return true;

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);


	if (!openColumn && contColumn <= 0) {
		openColumn = true;
		Engine::GetInstance().physics->DeletePhysBody(pbody);
		pbody = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)parameters.attribute("x").as_int(), (int)parameters.attribute("y").as_int(), texW, texH, bodyType::STATIC);
	}

	if (!openColumn)
	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX() + (texW * 1.7), (int)position.getY(), &currentAnimation->GetCurrentFrame());

	currentAnimation->Update();

	pbody->body->SetLinearVelocity({ 0,0 });

	return true;
}

bool Column::CleanUp()
{
	if (pbody != nullptr) {
		Engine::GetInstance().physics.get()->DeletePhysBody(pbody);

		pbody = nullptr;
	}

	return true;
}

void Column::SetPosition(Vector2D pos) {
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Column::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void Column::OnCollision(PhysBody* physA, PhysBody* physB) {


	switch (physB->ctype)
	{
	case ColliderType::BOSS:
		contColumn--;
		break;
	default:
		break;
	}
}

void Column::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::UNKNOWN:
		break;
	default:
		break;
	}
}
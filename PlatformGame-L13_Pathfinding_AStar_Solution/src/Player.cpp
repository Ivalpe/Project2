#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"
#include "Platform.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
}

Player::~Player() {

}

bool Player::Awake() {

	return true;
}

bool Player::Start() {

	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	currentAnimation = &idle;

	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);

	pbody->listener = this;

	pbody->ctype = ColliderType::PLAYER;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(1);

	//initialize audio effect
	pickCoinFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");
	

	return true;
}

bool Player::Update(float dt)
{
	velocity = b2Vec2(0, pbody->body->GetLinearVelocity().y);

	if (!parameters.attribute("gravity").as_bool()) {
		velocity = b2Vec2(0,0);
	}

	// Move left
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		velocity.x = -0.2 * 16;

	}
	// Move right
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		velocity.x = 0.2 * 16;
		
	}
	
	//Jump
	HandleJump(dt);

	// hide
	if (!isClimbing && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		
		isHiding = true;
		pbody->body->SetLinearVelocity(b2Vec2(pbody->body->GetLinearVelocity().x, 0));
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			velocity.x /= 4;
		}


	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_UP) {
		isHiding = false;
	}

	//To glide
	HandleGlide(dt);

	//Climbing
	HandleClimbing(dt);
	
	// Apply the velocity to the player
	pbody->body->SetLinearVelocity(velocity);

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	/*Engine::GetInstance().render.get()*/
	currentAnimation->Update();
	return true;
}

void Player::HandleJump(float dt)
{
	if (isJumping && lastJump <= 25)lastJump++;
	if (!isHiding && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		if (!isJumping) {
			// Apply an initial upward force
			pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
			isJumping = true;
			canDoubleJump = true;
		}
		else if (canDoubleJump && lastJump > 25) {
			pbody->body->SetLinearVelocity(b2Vec2(pbody->body->GetLinearVelocity().x, 0));
			pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
			canDoubleJump = false;
		}

	}

	// If the player is jumpling, we don't want to apply gravity, we use the current velocity prduced by the jump
	if (isJumping == true)
	{
		velocity.y = pbody->body->GetLinearVelocity().y;

	}
}

void Player::HandleGlide(float dt)
{
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
	{
		++glid_time;
		if (fallForce >= 1.0 && glid_time > glid_reduce) {
			fallForce -= 0.1;
			glid_reduce += glidDuration;
		}
		velocity.y = pbody->body->GetLinearVelocity().y / fallForce;
	}
}

void Player::HandleClimbing(float dt) 
{
	if (isClimbing) {
		velocity.y = 0;
		pbody->body->SetGravityScale(0);

		// Move Up
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			velocity.y = -0.3 * 16;
		}

		// Move down
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			velocity.y = 0.3 * 16;
		}
	}
	else {
		pbody->body->SetGravityScale(1);

	}

	// When on a m_platform, add platform velocity to player movement
	if (isOnPlatform){
		velocity.x+= platform->pbody->body->GetLinearVelocity().x;
	}


	// Apply the velocity to the player
	pbody->body->SetLinearVelocity(velocity);

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	currentAnimation->Update();
	

}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		isJumping = false;
		canDoubleJump = false;
		lastJump = 0;
		fallForce = 1.5;

		isClimbing = false;
		break;
	case ColliderType::M_PLATFORM:
		LOG("Collision M_PLATFORM");
		isJumping = false;
		canDoubleJump = false;
		lastJump = 0;
		fallForce = 1.5;
		isClimbing = false;
		isOnPlatform = true;

		// Assign the platform listener if valid.
		if (physB->listener != nullptr && dynamic_cast<Platform*>(physB->listener)) {
			platform = static_cast<Platform*>(physB->listener);
		}
	
		break;

	case ColliderType::ITEM:
		LOG("Collision ITEM");
		Engine::GetInstance().audio.get()->PlayFx(pickCoinFxId);
		Engine::GetInstance().physics.get()->DeletePhysBody(physB); // Deletes the body of the item from the physics world
		break;
	case ColliderType::CLIMBABLE:

		isClimbing = true;
		pbody->body->SetLinearVelocity(b2Vec2(0, 0));
		pbody->body->SetGravityScale(0);
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	default:
		break;
	}
}

void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("End Collision PLATFORM");
		break;
	case ColliderType::M_PLATFORM:
		LOG("End Collision M_PLATFORM");
		isOnPlatform = false;
		platform = nullptr;
		break;
	case ColliderType::ITEM:
		LOG("End Collision ITEM");
		break;
	case ColliderType::CLIMBABLE:
		isClimbing = false;
		pbody->body->SetGravityScale(1);

		LOG("End Collision CLIMABLE");
		break;
	case ColliderType::UNKNOWN:
		LOG("End Collision UNKNOWN");
		break;
	default:
		break;
	}
}

void Player::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos,0);
}

Vector2D Player::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}
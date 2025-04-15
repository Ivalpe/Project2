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

#define GRAVITY 2.0f

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


	//L03: TODO 2: Initialize Player parameters
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	if (Engine::GetInstance().scene.get()->level == 0) {
		position.setX(parameters.attribute("x").as_int());
		position.setY(parameters.attribute("y").as_int());
	}
	else if (Engine::GetInstance().scene.get()->level == 1)
	{
		position.setX(40);
		position.setY(70);
	}

	//Load animations
	hide.LoadAnimations(parameters.child("animations").child("hide"));
	getUp.LoadAnimations(parameters.child("animations").child("getUp"));
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	walk.LoadAnimations(parameters.child("animations").child("walk"));
	hide.LoadAnimations(parameters.child("animations").child("hide"));
	crawl.LoadAnimations(parameters.child("animations").child("crawl"));
	unhide.LoadAnimations(parameters.child("animations").child("unhide"));
	jump.LoadAnimations(parameters.child("animations").child("jump"));
	fall.LoadAnimations(parameters.child("animations").child("fall"));
	death.LoadAnimations(parameters.child("animations").child("death"));
	/*currentAnimation = &idle;*/
	playerState = IDLE;
	hide.Reset();

	// L08 TODO 5: Add physics to the player - initialize physics body
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() - texH / 2, (int)position.getY() - texH / 2, texW / 3, bodyType::DYNAMIC);

	// L08 TODO 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(GRAVITY);

	//initialize audio effect
	pickCoinFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");
	
	return true;
}

bool Player::Update(float dt)
{
	if (Engine::GetInstance().scene.get()->showPauseMenu == true || Engine::GetInstance().scene.get()->GameOverMenu == true) return true;
	velocity = b2Vec2(0, pbody->body->GetLinearVelocity().y);

	if (!parameters.attribute("gravity").as_bool()) velocity = b2Vec2(0,0);

	// press F to die for absolutely no reason lmao (akshually yes, debugging purposes)
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		playerState = DEAD;
	}

	if (playerState != DEAD) {

		
		// Move left
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT/* || Engine::GetInstance().input.get()->pads[0].l_x <= -0.1f*/) {

			velocity.x = -0.2 * speed;
			dir = RIGHT;
			if (playerState != FALL && playerState != JUMP) playerState = WALK;
		}
		// Move right
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT/* || Engine::GetInstance().input.get()->pads[0].l_x >= 0.1f*/) {
			velocity.x = 0.2 * speed;
			dir = LEFT;
			if (playerState != FALL && playerState != JUMP) playerState = WALK;
		}

		if (playerState != CRAWL && playerState != HIDE && playerState != UNHIDE) {
			if (velocity.x == 0) {
				playerState = IDLE;
			}
		}


		//Jump
		if (isJumping && lastJump <= 25) lastJump++;
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN /*|| Engine::GetInstance().input.get()->pads[0].b*/) {

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


		// hide

		if (!isClimbing && (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT /*|| Engine::GetInstance().input.get()->pads[0].zl*/)) {

			isJumping = false;
			if (playerState != CRAWL) {
				playerState = HIDE;
			}

			// crawl
			if (playerState == HIDE && hide.HasFinished()) {

				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT
					/*|| Engine::GetInstance().input.get()->pads[0].l_x <= -0.1f || Engine::GetInstance().input.get()->pads[0].l_x >= 0.1f*/) {
					velocity.x /= 4;
					playerState = CRAWL;

				}
				else {
					playerState = HIDE;
				}
			}

		}

		if (playerState == FALL && velocity.y == 0) {
			if (velocity.x != 0) playerState = WALK;
			else playerState = IDLE;
		}

		//Unhide
		if (/*playerState == CRAWL && */Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_UP /*|| !Engine::GetInstance().input.get()->pads[0].zl*/) {
			playerState = UNHIDE;
			isCrawling = false;
		}


		if (isJumping == true) {
			velocity.y = pbody->body->GetLinearVelocity().y;
			if (velocity.y < 0) playerState = JUMP;
			/*else playerState = FALL;*/
		}

		printf("%f\n", velocity.y);

		if (velocity.y > 1.0f) {

			playerState = FALL;
		}


		//To glide
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
		{
			playerState = GLIDE;
			++glid_time;
			if (fallForce >= 1.0 && glid_time > glid_reduce) {
				fallForce -= 0.1;
				glid_reduce += glidDuration;
			}
			velocity.y = pbody->body->GetLinearVelocity().y / fallForce;
		}

		if (isClimbing) {
			velocity.y = 0;
			pbody->body->SetGravityScale(0);

			// Move Up
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) velocity.y = -0.3 * 16;

			// Move down
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) velocity.y = 0.3 * 16;
		}
		else {
			pbody->body->SetGravityScale(GRAVITY);

		}
		// Apply the velocity to the player
		pbody->body->SetLinearVelocity(velocity);

	}


	switch (playerState) {
	case IDLE:
		currentAnimation = &idle;
		break;
	case WALK:
		currentAnimation = &walk;
		hide.Reset();
		break;
	case JUMP:
		if (currentAnimation != &jump) {
			jump.Reset();
			currentAnimation = &jump;
		}

		break;
	case FALL:
		if (currentAnimation != &fall) {
			fall.Reset();
			currentAnimation = &fall;
		}
		break;
	case CLIMB:
	//	currentAnimation = &hurt;
		break;
	case HIDE:
		if (currentAnimation != &hide) {
			hide.Reset();
			currentAnimation = &hide;
		}
		break;
	case CRAWL:
		currentAnimation = &crawl;
		unhide.Reset();
		break;
	case UNHIDE:
		if (currentAnimation != &unhide) {
			unhide.Reset();
			currentAnimation = &unhide;
		} 

		if (unhide.HasFinished()) {
			playerState = IDLE;
			/*unhide.Reset();
			hide.Reset();*/
		}
	
		break;
	case DEAD:
		if (currentAnimation != &death) {
			currentAnimation = &death;
			deathTimer.Start();
		}
		
		if (deathTimer.ReadSec() >= 2.0f) {
			Engine::GetInstance().scene.get()->reset_level = true;
			playerState = IDLE;
			
		}
		break;
	}

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH );
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH );

	if (dir == LEFT) {
		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX() + texH/2, (int)position.getY() + texH/3, &currentAnimation->GetCurrentFrame());
	}
	else {
		Engine::GetInstance().render.get()->DrawTextureFlipped(texture, (int)position.getX() + texH / 2, (int)position.getY() + texH / 3, &currentAnimation->GetCurrentFrame());
	}
	
	currentAnimation->Update();
	return true;
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

// L08 TODO 6: Define OnCollision function for the player. 
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		isJumping = false;
		canDoubleJump = false;
		lastJump = 0;
		fallForce = 1.5;

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
	case ColliderType::CHANGE_LEVEL:
		change_level = true;
		Engine::GetInstance().scene.get()->level++;
		Engine::GetInstance().scene.get()->reset_level = true;

		cleanup_pbody = true;
		break;
	case ColliderType::DAMAGE:
		LOG("Colisión con daño detectada");

		Engine::GetInstance().entityManager.get()->wax--;
		if (Engine::GetInstance().entityManager.get()->wax > 0) {
			//Engine::GetInstance().scene.get()->PreUpdate();
			Engine::GetInstance().scene.get()->reset_level = true;


		}

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
	case ColliderType::ITEM:
		LOG("End Collision ITEM");
		break;
	case ColliderType::CLIMBABLE:
		isClimbing = false;
		pbody->body->SetGravityScale(GRAVITY);

		LOG("End Collision CLIMABLE");
		break;
	case ColliderType::CHANGE_LEVEL:
		change_level = false;
		Engine::GetInstance().scene.get()->reset_level = true;

		if (cleanup_pbody) {

			Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
			pbody = nullptr;  // Eliminar el cuerpo físico del jugador

			Start();

			cleanup_pbody = false;
		}
	case ColliderType::DAMAGE:
		Engine::GetInstance().scene.get()->reset_level = true;

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

int Player::GetWax() {
	return Engine::GetInstance().entityManager.get()->wax;
}

void Player::StopMovement() {
	if (pbody != nullptr) {
		Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
		pbody = nullptr;
		pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() - texH / 2, (int)position.getY() - texH / 3, texW / 3, bodyType::STATIC);
		pbody->listener = this;
		pbody->ctype = ColliderType::PLAYER;

	}
}

void Player::ResumeMovement() {
	if (pbody != nullptr) {
		Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
		pbody = nullptr;
		pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() - texH / 2, (int)position.getY() - texH / 3, texW / 3, bodyType::DYNAMIC);
		pbody->listener = this;
		pbody->ctype = ColliderType::PLAYER;

	}
}
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

	useTemporaryCheckpoint = false;
	setTempCheckTrue = false;

	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	if (Engine::GetInstance().scene.get()->level == 0) {
		position.setX(parameters.attribute("x").as_int());
		position.setY(parameters.attribute("y").as_int());
	}
	else if (Engine::GetInstance().scene.get()->level == 1)
	{
		position.setX(300);
		position.setY(600);
	}

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	walk.LoadAnimations(parameters.child("animations").child("walk"));
	hide.LoadAnimations(parameters.child("animations").child("hide"));
	crawl.LoadAnimations(parameters.child("animations").child("crawl"));
	unhide.LoadAnimations(parameters.child("animations").child("unhide"));
	jump.LoadAnimations(parameters.child("animations").child("jump"));
	fall.LoadAnimations(parameters.child("animations").child("fall"));
	land.LoadAnimations(parameters.child("animations").child("land"));
	turn2back.LoadAnimations(parameters.child("animations").child("turn2back"));
	climb.LoadAnimations(parameters.child("animations").child("climb"));
	onrope.LoadAnimations(parameters.child("animations").child("onrope"));
	turn2front.LoadAnimations(parameters.child("animations").child("turn2front"));
	death.LoadAnimations(parameters.child("animations").child("death"));
	glide_start.LoadAnimations(parameters.child("animations").child("glide_start"));
	glide.LoadAnimations(parameters.child("animations").child("glide"));
	glide_stop.LoadAnimations(parameters.child("animations").child("glide_stop"));

	playerState = IDLE;
	hide.Reset();

	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() - texH / 2, (int)position.getY() - texH / 2, texW / 3, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(GRAVITY);

	//initialize audio effect
	pickCoinFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");

	return true;
}

bool Player::Update(float dt)
{
	if (Engine::GetInstance().scene.get()->showPauseMenu == true || Engine::GetInstance().scene.get()->GameOverMenu == true || Engine::GetInstance().scene.get()->InitialScreenMenu == true) return true;
	velocity = b2Vec2(0, pbody->body->GetLinearVelocity().y);

	if (takenDMG and useTemporaryCheckpoint) {
		TeleportToTemporaryCheckpoint();
		takenDMG = false;
	}

	if (!parameters.attribute("gravity").as_bool()) velocity = b2Vec2(0, 0);

	// press F to die for absolutely no reason lmao (akshually yes, debugging purposes)
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		playerState = DEAD;
	}

	if (playerState != DEAD) {
		// Move left
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT/* || Engine::GetInstance().input.get()->pads[0].l_x <= -0.1f*/) {

			velocity.x = -0.2 * speed;
			dir = RIGHT;
			if (playerState == CLIMB) {

			}
			if (playerState != FALL && playerState != JUMP && playerState != CLIMB) {
				playerState = WALK;
			}
		}
		// Move right
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT/* || Engine::GetInstance().input.get()->pads[0].l_x >= 0.1f*/) {
			velocity.x = 0.2 * speed;
			if (playerState == CLIMB) {
				LOG("MOVING LEFT");
			}
			dir = LEFT;
			if (playerState != FALL && playerState != JUMP && playerState != CLIMB) {
				playerState = WALK;
			}
		}
		if (playerState != CRAWL && playerState != HIDE && playerState != UNHIDE && playerState != CLIMB) {
			if (velocity.x >= -0.05 && velocity.x <= 0.05) {
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

		//Dash
		if ((playerState != CLIMB && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_E)== KEY_DOWN && !dashColdown) || isDashing) {
			
			playerState = DASH;
			isDashing = true;
			isJumping = false;
			dashColdown = true;

			if (dashDurantion < 12 ) {
				pbody->body->SetGravityScale(0);
				velocity.y = 0;

				if (dir == RIGHT) {
					velocity.x = -0.7 * speed;
					currentAnimation = &jump;

				}
				else {
					velocity.x = 0.7 * speed;
				}
				dashDurantion++;
			}
			else{
				pbody->body->SetGravityScale(GRAVITY);
				dashDurantion = 0;
				playerState = FALL;
				isDashing = false;

			}
		}

		// hide
		if (playerState != CLIMB && (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT /*|| Engine::GetInstance().input.get()->pads[0].zl*/)) {

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
				/*else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_UP || Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_UP) {
					playerState = HIDE;
				}*/

			}

		}

		if (playerState == FALL && velocity.y == 0 && playerState != DASH) {
			if (velocity.x != 0) playerState = WALK;
			else playerState = IDLE;
		}

		//Unhide
		if (playerState != CLIMB && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_UP /*|| !Engine::GetInstance().input.get()->pads[0].zl*/) {
			playerState = UNHIDE;
			isCrawling = false;
		}


		if (isJumping == true) {
			velocity.y = pbody->body->GetLinearVelocity().y;
			if (velocity.y < 0) playerState = JUMP;
			/*else playerState = FALL;*/
		}

		//printf("%f\n", velocity.y);

		if (velocity.y > 1.0f && playerState != CLIMB && !isClimbing) {

			playerState = FALL;
		}

		//To glide
		if (playerState != DASH && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT && velocity.y > 0.5f)
		{
			playerState = GLIDE;

			++glid_time;
			if (fallForce >= 1.0 && glid_time > glid_reduce) {
				fallForce -= 0.1;
				glid_reduce += glidDuration;
			}
			velocity.y = pbody->body->GetLinearVelocity().y / fallForce;
		}

		if (playerState == CLIMB) {
			pbody->body->SetGravityScale(0);
			velocity.y = 0;

			// If still auto-grabbing
			if (!isClimbing) {
				if (currentAnimation == &turn2back && turn2back.HasFinished()) {
					isClimbing = true;

				}

				pbody->body->SetTransform(b2Vec2(climbableX, pbody->body->GetPosition().y), pbody->body->GetAngle());

			}
			else {
				// Now allow climbing movement
				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
					velocity.y = -0.3f * 16;
					currentAnimation = &climb;
				}
				else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
					velocity.y = 0.3f * 16;
					currentAnimation = &climb;

				}
				else {
					currentAnimation = &onrope;
				}
				climbOffset = 40;

				// Press space to jump off 
				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
					LOG("Jumped off rope");

					isClimbing = false;
					exitingRope = true;

					playerState = JUMP;
					velocity.y = -6.0f; // or whatever your jump velocity is
					pbody->body->SetGravityScale(GRAVITY);
					climbOffset = 0;
					return true;
				}

				// press left/right to exit rope
				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT ||
					Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
					LOG("Walked off rope");
					isClimbing = false;
					exitingRope = true;

					playerState = FALL;
					pbody->body->SetGravityScale(GRAVITY);
					climbOffset = 0;
					return true;
				}

			}
		}
		else {
			pbody->body->SetGravityScale(GRAVITY);
		}

		// When on a m_platform, add platform velocity to player movement
		if (isOnPlatform) {
			velocity.x += platform->pbody->body->GetLinearVelocity().x;
		}
		// Apply the velocity to the player
		pbody->body->SetLinearVelocity(velocity);

	}


	switch (playerState) {
	case IDLE:
		if (currentAnimation == &turn2front) {

			if (!turn2front.HasFinished()) {
				currentAnimation = &turn2front;
				break;
			}
		}


		if (currentAnimation == &glide) {
			glide_stop.Reset();
			currentAnimation = &glide_stop;
			break;
		}
		if (currentAnimation == &glide_stop) {

			if (!glide_stop.HasFinished()) {
				currentAnimation = &glide_stop;
				break;
			}
		}

		if (currentAnimation == &fall && fall.HasFinished()) {
			land.Reset();
			currentAnimation = &land;
			exitingRope = false;
		}
		else if (currentAnimation == &land) {
			if (land.HasFinished()) {
				currentAnimation = &idle;

			}
		}
		else {
			currentAnimation = &idle;
			isClimbing = false;
			climbOffset = 0;
		}

		break;
	case WALK:
		currentAnimation = &walk;
		hide.Reset();
		break;
	case JUMP:
		if (currentAnimation != &jump) {

			if (!exitingRope) jump.Reset();
			currentAnimation = &jump;
		}

		break;
	case FALL:
		if (currentAnimation != &fall && !onGround) {
			if (!exitingRope) fall.Reset();
			currentAnimation = &fall;
		}
		break;
	case HIDE:

		if (currentAnimation != &hide) {
			if (currentAnimation != &crawl) hide.Reset();
			currentAnimation = &hide;
		}
		break;
	case CRAWL:
		if (velocity.x != 0) currentAnimation = &crawl;
		else currentAnimation = &hide;
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

	case GLIDE:
		if (currentAnimation != &glide && currentAnimation != &glide_stop && currentAnimation != &glide_start) {
			glide_start.Reset();
			currentAnimation = &glide_start;
			break;
		}

		if (currentAnimation == &glide_start && glide_start.HasFinished()) {
			glide_start.Reset();
			currentAnimation = &glide;

		}
		break;
	}

	b2Transform pbodyPos = pbody->body->GetTransform();


	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH);

	if (dir == LEFT) {
		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX() + texH / 2 - climbOffset, (int)position.getY() + texH / 3, &currentAnimation->GetCurrentFrame());
	}
	else {
		Engine::GetInstance().render.get()->DrawTextureFlipped(texture, (int)position.getX() + texH / 2 + climbOffset, (int)position.getY() + texH / 3, &currentAnimation->GetCurrentFrame());
	}

	currentAnimation->Update();
	/*LOG("playerstate: %i", playerState);*/
	return true;
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

void Player::TeleportToTemporaryCheckpoint() {
	SetPosition(temporaryCheckpoint);
	takenDMG = false;
}

void Player::TakeDamage() {
	if (!takenDMG) {
		Engine::GetInstance().entityManager.get()->candleNum--;
		if (Engine::GetInstance().entityManager.get()->candleNum <= 0) {
			//Engine::GetInstance().scene.get()->PreUpdate();
			Engine::GetInstance().scene.get()->reset_level = true;
		}
	}

	pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0.f, -250.0f), true);
	isInAttackSensor = false;
	takenDMG = true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::BOSS:
		TakeDamage();
		break;

	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		isJumping = false;
		canDoubleJump = false;
		dashColdown = false;
		onGround = true;
		lastJump = 0;
		fallForce = 1.5;

		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		Engine::GetInstance().audio.get()->PlayFx(pickCoinFxId);
		/*pickedItem = true;*/
		Engine::GetInstance().scene.get()->filledWaxy = false;
		Engine::GetInstance().physics.get()->DeletePhysBody(physB); // Deletes the body of the item from the physics world
		break;
	case ColliderType::CLIMBABLE:
		LOG("Collision CLIMBABLE");

		// Auto-grab triggered
		playerState = CLIMB;
		isClimbing = false;
		isJumping = false;
		pbody->body->SetGravityScale(0);
		pbody->body->SetLinearVelocity(b2Vec2(0, 0));

		// auto-grab animation
		if (currentAnimation != &turn2back) {
			turn2back.Reset();
			currentAnimation = &turn2back;
		}

		climbableX = physB->body->GetPosition().x;
		break;
	case ColliderType::CHANGE_LEVEL:
		change_level = true;
		Engine::GetInstance().scene.get()->level++;
		Engine::GetInstance().scene.get()->reset_level = true;

		cleanup_pbody = true;
		break;
	case ColliderType::ENEMY:
		if (useTemporaryCheckpoint) {
			setTempCheckTrue = true;
			useTemporaryCheckpoint = false;
		}
	case ColliderType::DAMAGE:
		LOG("Colisión con daño detectada");

		if (!takenDMG) {
			Engine::GetInstance().entityManager.get()->candleNum--;
			if (Engine::GetInstance().entityManager.get()->candleNum <= 0) {
				//Engine::GetInstance().scene.get()->PreUpdate();
				Engine::GetInstance().scene.get()->reset_level = true;
			}
		}

		takenDMG = true;

		break;
	case ColliderType::ATTACKSENSOR:
		isInAttackSensor = true;
		break;
		//Engine::GetInstance().scene.get()->drainedWaxy = false;
	case ColliderType::WALL:
		if (Engine::GetInstance().entityManager->feather >= 2) touched_wall = true;
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
	case ColliderType::DAMAGE_RESPAWN:
		LOG("Collision Damage Respawn");
		if (!useTemporaryCheckpoint) {
			useTemporaryCheckpoint = true;
			temporaryCheckpoint = GetPosition();
			int y = temporaryCheckpoint.getY();
			temporaryCheckpoint.setY(y - (5 * 32));
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
		onGround = false;
		break;
	case ColliderType::ITEM:
		LOG("End Collision ITEM");
		break;
	case ColliderType::CLIMBABLE:

		if (currentAnimation != &turn2front) {
			isClimbing = false;
			turn2front.Reset();
			currentAnimation = &turn2front;

			playerState = IDLE;
			pbody->body->SetGravityScale(GRAVITY);

		}

		LOG("End Collision CLIMABLE");
		break;

	case ColliderType::CHANGE_LEVEL:
		//change_level = false;
		//Engine::GetInstance().scene.get()->reset_level = true;

		//if (cleanup_pbody) {

		//	Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
		//	pbody = nullptr;  // Eliminar el cuerpo físico del jugador

		//	Start();

		//	cleanup_pbody = false;
		//}
	case ColliderType::ENEMY:
		if (setTempCheckTrue) {
			useTemporaryCheckpoint = true;
			setTempCheckTrue = false;
		}
	case ColliderType::DAMAGE:
		/*Engine::GetInstance().scene.get()->reset_level = true;*/
		takenDMG = false;

		break;
	case ColliderType::ATTACKSENSOR:
		isInAttackSensor = false;
		takenDMG = false;
		break;
	case ColliderType::M_PLATFORM:
		LOG("End Collision M_PLATFORM");
		isOnPlatform = false;
		platform = nullptr;
		break;
	case ColliderType::DAMAGE_RESPAWN:
		LOG("End Collision Damage Respawn");
		if (useTemporaryCheckpoint) {
			useTemporaryCheckpoint = false;
		}
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
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Player::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

int Player::GetWax() {

	return Engine::GetInstance().entityManager->wax;

}


void Player::StopMovement() {
	if (pbody != nullptr) {
		Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
		pbody = nullptr;
		pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH, (int)position.getY() + texH, (texW / 3), bodyType::STATIC);
		pbody->listener = this;
		pbody->ctype = ColliderType::PLAYER;

	}
}

void Player::ResumeMovement() {
	if (pbody != nullptr) {
		Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
		pbody = nullptr;
		pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH, (int)position.getY() + texH, (texW / 3), bodyType::DYNAMIC);
		pbody->listener = this;
		pbody->ctype = ColliderType::PLAYER;

	}
}
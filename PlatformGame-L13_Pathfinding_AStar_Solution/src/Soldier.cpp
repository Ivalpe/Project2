#include "Soldier.h"
#include "Engine.h"
#include "Textures.h"
#include "Physics.h"
#include "Scene.h"
#include "Player.h"
#include "LOG.h"
#include "Audio.h"
#include "Pathfinding.h"


Soldier::Soldier()
{

}

Soldier::~Soldier() {
	
}


bool Soldier::Start() {

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
	pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX() + texW / 6, (int)position.getY() + texH/2, texW/2, texH, bodyType::DYNAMIC);

	//Assign collider type
	pbody->ctype = ColliderType::ENEMY;
	pbody->body->SetFixedRotation(true);
	pbody->listener = this;
	//INIT ROUTE
	for (int i = 0; i < route.size(); i++)
	{
		route[i] = { route[i].getX(), route[i].getY() };
	}
	destPointIndex = 0;
	destPoint = route[destPointIndex];


	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	// Initialize pathfinding
	pathfinding = new Pathfinding();
	ResetPath();

	//init variables
	state = PATROL;
	speed = parameters.child("properties").attribute("speed").as_float();
	chaseArea = parameters.child("properties").attribute("chaseArea").as_float();
	attackArea = parameters.child("properties").attribute("attackArea").as_float();
	
	dir = LEFT;

	return true;
}

bool Soldier::Update(float dt)
{

	if (!dead) {
		////FRUSTUM
		//if (!Engine::GetInstance().render.get()->InCameraView(pbody->GetPosition().getX() - texW, pbody->GetPosition().getY() - texH, texW, texH))
		//{
		//	pbody->body->SetEnabled(false);
		//	return true;
		//}
		//else
		//{
		//	pbody->body->SetEnabled(true);
		//}

		if (!Engine::GetInstance().scene.get()->showPauseMenu) {

			/*printf("Position: %f, %f\n", player->pbody->GetPhysBodyWorldPosition().getX(), player->pbody->GetPhysBodyWorldPosition().getY());*/
			dist = pbody->GetPhysBodyWorldPosition().distanceEuclidean(player->pbody->GetPhysBodyWorldPosition());

			//STATES CHANGERS
			if (state != ATTACK && state != DEAD)
			{
				if (dist > chaseArea && state != PATROL)
				{
					state = PATROL;
					ResetPath();
					destPoint = route[destPointIndex];
				}
				else if (dist <= chaseArea/* && state != CHASING*/)
				{
					if (dist <= attackArea && state != ATTACK) {
						state = ATTACK;
						/*player->DMGPlayer(player->pbody, pbody);
						Engine::GetInstance().audio.get()->PlayFx(swordSlashSFX, 0, -1);*/
						/*attack.Reset();*/
						attackTimer.Start();
						pbody->body->SetLinearVelocity({ 0,0 });
						
					}
					else if (state != CHASING)
					{
						state = CHASING;
						ResetPath();
					}
				}
			}



			Vector2D playerPos = player->pbody->GetPhysBodyWorldPosition();
			/*Vector2D playerPosCenteredOnTile = Engine::GetInstance().map.get()->WorldToWorldCenteredOnTile(playerPos.getX(), playerPos.getY());*/

			//STATES CONTROLER
			if (state == DEAD) {

				if (deathTimer.ReadSec() > deathTime) {

					pbody->body->SetEnabled(false);
					dead = true;
				}

			}

			else if (state == PATROL) {

				Vector2D physPos = pbody->GetPhysBodyWorldPosition();
				if (CheckIfTwoPointsNear(destPoint, { physPos.getX(), physPos.getY() }, 3))
				{
					destPointIndex++;
					if (destPointIndex == route.size()) {
						destPointIndex = 0;
						
					}
					destPoint = route[destPointIndex];
					ResetPath();
					
					
				}
			}
			else if (state == CHASING) {

				if (destPoint.getX() != playerPos.getX() && destPoint.getY() != playerPos.getY())
				{
					destPoint = playerPos;
					ResetPath();
				}
			}
			else if (state == ATTACK) {


				if (attackTimer.ReadSec() > attackTime) {

					state = PATROL;
				}
			}

			//PATHFINDING CONTROLER
			if (state == PATROL || state == CHASING)
			{
				if (pathfinding->pathTiles.empty())
				{
					while (pathfinding->pathTiles.empty())
					{
						pathfinding->PropagateAStar(SQUARED, destPoint);

					}
					pathfinding->pathTiles.pop_back();
				}
				else
				{

					Vector2D nextTile = pathfinding->pathTiles.back();
					Vector2D nextTileWorld = Engine::GetInstance().map.get()->MapToWorld(nextTile.getX(), nextTile.getY());

					if (CheckIfTwoPointsNear(nextTileWorld, { (float)METERS_TO_PIXELS(pbody->body->GetPosition().x), (float)METERS_TO_PIXELS(pbody->body->GetPosition().y) }, 5)) {

						pathfinding->pathTiles.pop_back();
						if (pathfinding->pathTiles.empty()) {
							ResetPath();
						}
					}
					else {
						Vector2D nextTilePhysics = { PIXEL_TO_METERS(nextTileWorld.getX()),PIXEL_TO_METERS(nextTileWorld.getY()) };
						b2Vec2 direction = { nextTilePhysics.getX() - pbody->body->GetPosition().x, nextTilePhysics.getY() - pbody->body->GetPosition().y };
						direction.Normalize();
						
						pbody->body->SetLinearVelocity({ direction.x * speed, pbody->body->GetLinearVelocity().y });
						printf("velocity x = %f\n", pbody->body->GetLinearVelocity().x);
					}
				}
				/*Vector2D currentTile = Engine::GetInstance().map.get()->WorldToMap(pbody->GetPhysBodyWorldPosition().getX(), pbody->GetPhysBodyWorldPosition().getY());*/

				
			}

		}
		else
		{
			/*pbody->body->SetLinearVelocity({ 0,0 });*/

		}


		/*switch (state) {
			break;
		case CHASING:
			currentAnimation = &walk;
			break;
		case PATROL:
			currentAnimation = &walk;
			break;
		case ATTACK:
			currentAnimation = &attack;
			break;
		case DEAD:
			currentAnimation = &death;
			break;
		default:
			break;
		}*/

		//DIRECTION
		if (pbody->body->GetLinearVelocity().x > 0.2f) {
			dir = RIGHT;
		}
		else if (pbody->body->GetLinearVelocity().x < -0.2f) {
			dir = LEFT;
		}

		//DRAW

		if (pbody->body->IsEnabled() && !Engine::GetInstance().scene.get()->showPauseMenu) {

			if (Engine::GetInstance().physics.get()->GetDebug())
			{
				Engine::GetInstance().render.get()->DrawCircle(position.getX() + texW / 2, position.getY() + texH / 2, chaseArea, 255, 255, 255);
				Engine::GetInstance().render.get()->DrawCircle(destPoint.getX(), destPoint.getY(), 3, 255, 0, 0);
				pathfinding->DrawPath();
				
			}
			
			currentAnimation->Update();
			

			b2Transform pbodyPos = pbody->body->GetTransform();
			position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2 );
			position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2 );



			if (dir == RIGHT) {
				Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
			}
			else if (dir == LEFT) {
				Engine::GetInstance().render.get()->DrawTextureFlipped(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
			}
		}

	}

	return true;
}

void Soldier::OnCollision(PhysBody* physA, PhysBody* physB) 
{
	

	
}
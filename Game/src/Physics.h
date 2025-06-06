#pragma once
#include "Module.h"
#include "Entity.h"
#include "box2d/box2d.h"
#include <list>

#define GRAVITY_X 0.0f
#define GRAVITY_Y -20.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

// types of bodies
enum bodyType {
	DYNAMIC,
	STATIC,
	KINEMATIC
};

enum class ColliderType {
	PLAYER,
	ITEM,
	PLATFORM,
	M_PLATFORM,
	ENEMY,
	ENEMY_DEATH,
	CLIMBABLE,
	CHANGE_LEVEL,
	DAMAGE,
	DAMAGE_RESPAWN,
	DAMAGE_LIGHT,
	WALL,
	CHASESENSOR,
	RANGELEFT,
	RANGERIGHT,
	WALLBOSS,
	WALLBOSSDES,
	BOSS,
	ATTACKSENSOR,
	UNKNOWN
	// ..
};

// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL), ctype(ColliderType::UNKNOWN)
	{}

	~PhysBody() {}



	Vector2D GetPosition();
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;
	

public:
	int width = 0;
	int height = 0;
	b2Body* body;
	Entity* listener;
	ColliderType ctype;
	Vector2D GetPhysBodyWorldPosition();
};

// Module --------------------------------------
class Physics : public Module, public b2ContactListener
{
public:

	// Constructors & Destructors
	Physics();
	~Physics();

	// Main module steps
	bool Start();
	bool PreUpdate();
	bool PostUpdate();
	bool CleanUp();

	bool IsPendingToDelete(PhysBody* physBody);


	bool GetDebug();

	// Create basic physics objects
	PhysBody* CreateRectangle(int x, int y, int width, int height, bodyType type);
	PhysBody* CreateCircle(int x, int y, int radious, bodyType type);
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height, bodyType type);
	PhysBody* CreateChain(int x, int y, int* points, int size, bodyType type);
	PhysBody* CreateCircleSensor(int x, int y, int radious, bodyType type);
	
	
	// b2ContactListener ---
	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact);

	void DeletePhysBody(PhysBody* physBody);

	void DeleteBody(b2Body* body) {
		world->DestroyBody(body);
	}

	std::list<PhysBody*> listToDelete;

	// List of physics bodies
	std::list<PhysBody*> bodiesToDelete;

private:

	// Box2D World
	b2World* world;

	// debug mode
	bool debug;
};
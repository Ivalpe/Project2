#include "EntityManager.h"
#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Scene.h"
#include "Log.h"
#include "Item.h"
#include "Enemy.h"
#include "Platform.h"



EntityManager::EntityManager() : Module()
{
	name = "entitymanager";
}

// Destructor
EntityManager::~EntityManager()
{}

// Called before render is available
bool EntityManager::Awake()
{
	LOG("Loading Entity Manager");
	bool ret = true;

	//Iterates over the entities and calls the Awake
	for(const auto entity : entities)
	{
		if (entity->active == false) continue;
		ret = entity->Awake();
	}

	return ret;
}

bool EntityManager::Start() {

	bool ret = true; 

	//Iterates over the entities and calls Start
	for(const auto entity : entities)
	{
		if (entity->active == false) continue;
		ret = entity->Start();
	}

	return ret;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	bool ret = true;

	for(const auto entity : entities)
	{
		if (entity->active == false) continue;
		ret = entity->CleanUp();
	}

	entities.clear();

	return ret;
}

Entity* EntityManager::CreateEntity(EntityType type)
{
	Entity* entity = nullptr; 

	switch (type)
	{
	case EntityType::PLAYER:
		entity = new Player();
		break;
	case EntityType::ITEM:
		entity = new Item();
		break;
	case EntityType::INTERACTIVEOBJECT:
		entity = new InteractiveObject();
		break;
	case EntityType::ENEMY:
		entity = new Enemy();
		break;
	case EntityType::BOSS:
		entity = new Boss();
		break;
	case EntityType::PLATFORM:
		entity = new Platform();
		break;
	case EntityType::COLUMN:
		entity = new Column();
		break;
	default:
		break;
	}

	entities.push_back(entity);

	return entity;
}

void EntityManager::DestroyEntity(Entity* entity)
{
	for (auto it = entities.begin(); it != entities.end(); ++it)
	{
		if (*it == entity) {
			(*it)->CleanUp();
			delete* it; // Free the allocated memory
			entities.erase(it); // Remove the entity from the list
			break; // Exit the loop after removing the entity
		}
	}
}

void EntityManager::AddEntity(Entity* entity)
{
	if ( entity != nullptr) entities.push_back(entity);
}

bool EntityManager::Update(float dt)
{
	bool ret = true;
	if (!Engine::GetInstance().scene.get()->reset_level) {
		for (const auto entity : entities)
		{
			if (entity->active == false) continue;
			ret = entity->Update(dt);
		}
	}
	return ret;
}

void EntityManager::RemoveAllEnemies()
{
	for (auto it = entities.begin(); it != entities.end(); ) {
		if ((*it)->type == EntityType::ENEMY) {
			LOG("Destroying enemy entity at position: (%f, %f)", (*it)->position.getX(), (*it)->position.getY());
			(*it)->CleanUp();
			delete* it;
			it = entities.erase(it);
		}
		else {
			++it;
		}
	}
}

void EntityManager::RemoveAllItems()
{
	for (auto it = entities.begin(); it != entities.end(); ) {
		if ((*it)->type == EntityType::ITEM) {
			LOG("Destroying enemy entity at position: (%f, %f)", (*it)->position.getX(), (*it)->position.getY());
			(*it)->CleanUp();
			delete* it;
			it = entities.erase(it);
		}
		else {
			++it;
		}
	}
}
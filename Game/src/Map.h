#pragma once

#include "Module.h"
#include <list>
#include "Physics.h"
#include <vector>
#include <map>

enum MapOrientation
{
	ORTOGRAPHIC = 0,
	ISOMETRIC
};

struct Properties
{
	struct Property
	{
		std::string name;
		bool value; //We assume that we are going to work only with bool for the moment
	};

	std::list<Property*> propertyList;

	~Properties()
	{
		for (const auto& property : propertyList)
		{
			delete property;
		}

		propertyList.clear();
	}

	Property* GetProperty(const char* name);

};

struct MapLayer
{
	int id;
	std::string name;
	int width;
	int height;
	std::vector<int> tiles;
	Properties properties;

	int Get(int i, int j) const
	{
		return tiles[(j * width) + i];
	}
};

// Ignore Terrain Types and Tile Types for now, but we want the image!

struct TileSet
{
	int firstGid;
	std::string name;
	int tileWidth;
	int tileHeight;
	int spacing;
	int margin;
	int tileCount;
	int columns;
	SDL_Texture* texture;

	SDL_Rect GetRect(unsigned int gid) {
		SDL_Rect rect = { 0 };

		int relativeIndex = gid - firstGid;
		rect.w = tileWidth;
		rect.h = tileHeight;
		rect.x = margin + (tileWidth + spacing) * (relativeIndex % columns);
		rect.y = margin + (tileHeight + spacing) * (relativeIndex / columns);

		return rect;
	}

};

struct MapData
{
	int width;
	int height;
	int tileWidth;
	int tileHeight;
	std::list<TileSet*> tilesets;
	MapOrientation orientation;

	std::list<MapLayer*> layers;
};

class Map : public Module
{
public:

	Map();

	// Destructor
	virtual ~Map();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(std::string path, std::string mapFileName);

	Vector2D MapToWorld(int x, int y) const;

	Vector2D WorldToMap(int x, int y);

	Vector2D MapToWorldCentered(int x, int y);

	Vector2D WorldToWorldCenteredOnTile(int x, int y);



	TileSet* GetTilesetFromTileId(int gid) const;

	bool LoadProperties(pugi::xml_node& node, Properties& properties);

	int GetWidth() {
		return mapData.width;
	}

	int GetHeight() {
		return mapData.height;
	}

	int GetTileWidth() {
		return mapData.tileWidth;
	}

	int GetTileHeight() {
		return mapData.tileHeight;
	}

	std::vector<Vector2D> GetEnemyList() {
		return posEnemy;
	}

	std::vector<Vector2D> GetWaxList() {
		return posWaxys;
	}
	
	std::vector<Vector2D> GetFeathersList() {
		return posFeathers;
	}

	std::vector<Vector2D> GetGlovesList() {
		return posGloves;
	}

	std::vector<Vector2D> GetClothList() {
		return posCloth;
	}

	std::vector<Vector2D> GetBossList() {
		return posBoss;
	}

	std::vector<Vector2D> GetColumnBossList() {
		return posColumnBoss;
	}
	std::vector<Vector2D> GetLightList() {
		return posLight;
	}

	MapLayer* GetNavigationLayer();

	void DrawFront();
	void DrawAll();
	void DrawFrontAbove();
public:
	std::string mapFileName;
	std::string mapPath;

private:
	bool mapLoaded;
	MapData mapData;
	std::vector<Vector2D> posEnemy;
	std::vector<PhysBody*> collisions;
	std::vector<Vector2D> posWaxys;
	std::vector<Vector2D> posFeathers;
	std::vector<Vector2D> posColumnBoss;
	std::vector<Vector2D> posLight;
	std::vector<Vector2D> posBoss;
	std::vector<Vector2D> posGloves;
	std::vector<Vector2D> posCloth;

	std::vector<PhysBody*> colliders;
};
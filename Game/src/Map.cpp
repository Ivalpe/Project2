
#include "Engine.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Log.h"
#include "Physics.h"
#include "Scene.h"
#include <map>

#include <math.h>

Map::Map() : Module(), mapLoaded(false)
{
	name = "map";
}

// Destructor
Map::~Map()
{
}

// Called before render is available
bool Map::Awake()
{
	name = "map";
	LOG("Loading Map Parser");

	return true;
}

bool Map::Start() {

	return true;
}

bool Map::Update(float dt)
{
	bool ret = true;

	if (mapLoaded) {

		// iterate all tiles in a layer
		for (const auto& mapLayer : mapData.layers) {
			if (Engine::GetInstance().scene->level != 0) {
				//Check if the property Draw exist get the value, if it's true draw the lawyer
				if (mapLayer->name != "Front" && mapLayer->name != "Front0" && mapLayer->properties.GetProperty("Draw") != NULL && mapLayer->properties.GetProperty("Draw")->value == true) {
					for (int i = 0; i < mapData.width; i++) {



						for (int j = 0; j < mapData.height; j++) {

							Vector2D mapInWorld = MapToWorld(i, j);
							if (Engine::GetInstance().render.get()->InCameraView(mapInWorld.getX(), mapInWorld.getY(), mapData.tileWidth, mapData.tileHeight))
							{
								//Get the gid from tile
								int gid = mapLayer->Get(i, j);
								//Check if the gid is different from 0 - some tiles are empty
								if (gid != 0) {

									TileSet* tileSet = GetTilesetFromTileId(gid);
									if (tileSet != nullptr) {
										//Get the Rect from the tileSetTexture;
										SDL_Rect tileRect = tileSet->GetRect(gid);
										//Get the screen coordinates from the tile coordinates
										Vector2D mapCoord = MapToWorld(i, j);
										//Draw the texture
										Engine::GetInstance().render->DrawTexture(tileSet->texture, mapCoord.getX(), mapCoord.getY(), &tileRect);
									}
								}
							}
						}
					}
				}
			}
			else {
				if (mapLayer->name == "Background" && mapLayer->properties.GetProperty("Draw") != NULL && mapLayer->properties.GetProperty("Draw")->value == true) {
					for (int i = 0; i < mapData.width; i++) {



						for (int j = 0; j < mapData.height; j++) {

							Vector2D mapInWorld = MapToWorld(i, j);
							if (Engine::GetInstance().render.get()->InCameraView(mapInWorld.getX(), mapInWorld.getY(), mapData.tileWidth, mapData.tileHeight))
							{
								//Get the gid from tile
								int gid = mapLayer->Get(i, j);
								//Check if the gid is different from 0 - some tiles are empty
								if (gid != 0) {

									TileSet* tileSet = GetTilesetFromTileId(gid);
									if (tileSet != nullptr) {
										//Get the Rect from the tileSetTexture;
										SDL_Rect tileRect = tileSet->GetRect(gid);
										//Get the screen coordinates from the tile coordinates
										Vector2D mapCoord = MapToWorld(i, j);
										//Draw the texture
										Engine::GetInstance().render->DrawTexture(tileSet->texture, mapCoord.getX(), mapCoord.getY(), &tileRect);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return ret;
}

void Map::DrawAll() {
	for (const auto& mapLayer : mapData.layers) {
		//Check if the property Draw exist get the value, if it's true draw the lawyer
		if (mapLayer->name != "Front" && mapLayer->name != "Front0" && mapLayer->name != "Background" && mapLayer->properties.GetProperty("Draw") != NULL && mapLayer->properties.GetProperty("Draw")->value == true) {
			for (int i = 0; i < mapData.width; i++) {
				for (int j = 0; j < mapData.height; j++) {

					Vector2D mapInWorld = MapToWorld(i, j);
					if (Engine::GetInstance().render.get()->InCameraView(mapInWorld.getX(), mapInWorld.getY(), mapData.tileWidth, mapData.tileHeight))
					{
						//Get the gid from tile
						int gid = mapLayer->Get(i, j);
						//Check if the gid is different from 0 - some tiles are empty
						if (gid != 0) {

							TileSet* tileSet = GetTilesetFromTileId(gid);
							if (tileSet != nullptr) {
								//Get the Rect from the tileSetTexture;
								SDL_Rect tileRect = tileSet->GetRect(gid);
								//Get the screen coordinates from the tile coordinates
								Vector2D mapCoord = MapToWorld(i, j);
								//Draw the texture
								Engine::GetInstance().render->DrawTexture(tileSet->texture, mapCoord.getX(), mapCoord.getY(), &tileRect);
							}
						}
					}
				}
			}
		}
	}
}

void Map::DrawFront() {
	for (const auto& mapLayer : mapData.layers) {
		//Check if the property Draw exist get the value, if it's true draw the lawyer
		if (mapLayer->name == "Front" && mapLayer->name != "Front0" && mapLayer->properties.GetProperty("Draw") != NULL && mapLayer->properties.GetProperty("Draw")->value == true) {
			for (int i = 0; i < mapData.width; i++) {
				for (int j = 0; j < mapData.height; j++) {

					Vector2D mapInWorld = MapToWorld(i, j);
					if (Engine::GetInstance().render.get()->InCameraView(mapInWorld.getX(), mapInWorld.getY(), mapData.tileWidth, mapData.tileHeight))
					{
						//Get the gid from tile
						int gid = mapLayer->Get(i, j);
						//Check if the gid is different from 0 - some tiles are empty
						if (gid != 0) {

							TileSet* tileSet = GetTilesetFromTileId(gid);
							if (tileSet != nullptr) {
								//Get the Rect from the tileSetTexture;
								SDL_Rect tileRect = tileSet->GetRect(gid);
								//Get the screen coordinates from the tile coordinates
								Vector2D mapCoord = MapToWorld(i, j);
								//Draw the texture
								Engine::GetInstance().render->DrawTexture(tileSet->texture, mapCoord.getX(), mapCoord.getY(), &tileRect);
							}
						}
					}
				}
			}
		}
	}
}

void Map::DrawFrontAbove() {
	for (const auto& mapLayer : mapData.layers) {
		//Check if the property Draw exist get the value, if it's true draw the lawyer
		if (mapLayer->name != "Front" && mapLayer->name == "Front0" && mapLayer->properties.GetProperty("Draw") != NULL && mapLayer->properties.GetProperty("Draw")->value == true) {
			for (int i = 0; i < mapData.width; i++) {
				for (int j = 0; j < mapData.height; j++) {

					Vector2D mapInWorld = MapToWorld(i, j);
					if (Engine::GetInstance().render.get()->InCameraView(mapInWorld.getX(), mapInWorld.getY(), mapData.tileWidth, mapData.tileHeight))
					{
						//Get the gid from tile
						int gid = mapLayer->Get(i, j);
						//Check if the gid is different from 0 - some tiles are empty
						if (gid != 0) {

							TileSet* tileSet = GetTilesetFromTileId(gid);
							if (tileSet != nullptr) {
								//Get the Rect from the tileSetTexture;
								SDL_Rect tileRect = tileSet->GetRect(gid);
								//Get the screen coordinates from the tile coordinates
								Vector2D mapCoord = MapToWorld(i, j);
								//Draw the texture
								Engine::GetInstance().render->DrawTexture(tileSet->texture, mapCoord.getX(), mapCoord.getY(), &tileRect);
							}
						}
					}
				}
			}
		}
	}
}

TileSet* Map::GetTilesetFromTileId(int gid) const
{
	TileSet* set = nullptr;

	for (const auto& tileset : mapData.tilesets) {
		if (gid >= tileset->firstGid && gid < (tileset->firstGid + tileset->tileCount)) {
			set = tileset;
			break;
		}
	}

	return set;
}

// Called before quitting
bool Map::CleanUp()
{
	for (const auto& tileset : mapData.tilesets) {
		delete tileset;
	}
	mapData.tilesets.clear();

	for (const auto& layer : mapData.layers)
	{
		delete layer;
	}
	mapData.layers.clear();

	for (auto c : collisions) {
		Engine::GetInstance().physics->DeleteBody((c)->body);
		delete c;
	}
	collisions.clear();

	return true;
}

// Load new map
bool Map::Load(std::string path, std::string fileName)
{
	mapData.layers.clear();
	bool ret = false;
	posEnemy.clear();
	posWaxys.clear();
	posFeathers.clear();
	posColumnBoss.clear();
	posLight.clear();
	posBoss.clear();
	posCloth.clear();
	posGloves.clear();
	for (auto c : colliders) {
		Engine::GetInstance().physics->DeleteBody(c->body);
		delete c;
	}
	colliders.clear();

	// Assigns the name of the map file and the path
	mapFileName = fileName;
	mapPath = path;
	std::string mapPathName = mapPath + mapFileName;

	pugi::xml_document mapFileXML;
	pugi::xml_parse_result result = mapFileXML.load_file(mapPathName.c_str());

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", mapPathName.c_str(), result.description());
		ret = false;
	}
	else {

		// retrieve the paremeters of the <map> node and store the into the mapData struct
		mapData.width = mapFileXML.child("map").attribute("width").as_int();
		mapData.height = mapFileXML.child("map").attribute("height").as_int();
		mapData.tileWidth = mapFileXML.child("map").attribute("tilewidth").as_int();
		mapData.tileHeight = mapFileXML.child("map").attribute("tileheight").as_int();

		std::string orientationStr = mapFileXML.child("map").attribute("orientation").as_string();
		if (orientationStr == "orthogonal") {
			mapData.orientation = MapOrientation::ORTOGRAPHIC;
		}
		else if (orientationStr == "isometric") {
			mapData.orientation = MapOrientation::ISOMETRIC;
		}
		else {
			LOG("Map orientation not found");
			ret = false;
		}

		//Iterate the Tileset
		for (pugi::xml_node tilesetNode = mapFileXML.child("map").child("tileset"); tilesetNode != NULL; tilesetNode = tilesetNode.next_sibling("tileset"))
		{
			//Load Tileset attributes
			TileSet* tileSet = new TileSet();
			tileSet->firstGid = tilesetNode.attribute("firstgid").as_int();
			tileSet->name = tilesetNode.attribute("name").as_string();
			tileSet->tileWidth = tilesetNode.attribute("tilewidth").as_int();
			tileSet->tileHeight = tilesetNode.attribute("tileheight").as_int();
			tileSet->spacing = tilesetNode.attribute("spacing").as_int();
			tileSet->margin = tilesetNode.attribute("margin").as_int();
			tileSet->tileCount = tilesetNode.attribute("tilecount").as_int();
			tileSet->columns = tilesetNode.attribute("columns").as_int();

			//Load the tileset image
			std::string imgName = tilesetNode.child("image").attribute("source").as_string();
			tileSet->texture = Engine::GetInstance().textures->Load((mapPath + imgName).c_str());

			mapData.tilesets.push_back(tileSet);
		}

		for (pugi::xml_node layerNode = mapFileXML.child("map").child("layer"); layerNode != NULL; layerNode = layerNode.next_sibling("layer")) {

			//Load the attributes and saved in a new MapLayer
			MapLayer* mapLayer = new MapLayer();
			mapLayer->id = layerNode.attribute("id").as_int();
			mapLayer->name = layerNode.attribute("name").as_string();
			mapLayer->width = layerNode.attribute("width").as_int();
			mapLayer->height = layerNode.attribute("height").as_int();

			LoadProperties(layerNode, mapLayer->properties);

			//Iterate over all the tiles and assign the values in the data array
			for (pugi::xml_node tileNode = layerNode.child("data").child("tile"); tileNode != NULL; tileNode = tileNode.next_sibling("tile")) {
				mapLayer->tiles.push_back(tileNode.attribute("gid").as_int());
			}

			//add the layer to the map
			mapData.layers.push_back(mapLayer);
		}

		// Later you can create a function here to load and create the colliders from the map

		//Iterate the layer and create colliders
		for (const auto& mapLayer : mapData.layers) {
			if (mapLayer->name == "Collisions") {
				for (int i = 0; i < mapData.width; i++) {
					for (int j = 0; j < mapData.height; j++) {
						int gid = mapLayer->Get(i, j);
						if (gid == 1) {
							Vector2D mapCoord = MapToWorld(i, j);
							PhysBody* c1 = Engine::GetInstance().physics.get()->CreateRectangle(mapCoord.getX() + mapData.tileWidth / 2, mapCoord.getY() + mapData.tileHeight / 2, mapData.tileWidth, mapData.tileHeight, STATIC);
							c1->ctype = ColliderType::PLATFORM;
							collisions.push_back(c1);
						}
						else if (gid == 2) { // Soldier
							Vector2D mapCoord = { (float)i * 32, (float)j * 32 };
							posEnemy.push_back(mapCoord);
						}
						else if (gid == 3) { // Boss
							Vector2D mapCoord = { (float)i * 32, (float)j * 32 };
							posBoss.push_back(mapCoord);
						}
						else if (gid == 4) { // Items : Wax
							Vector2D mapCoord = { (float)i * 32, (float)j * 32 };
							posWaxys.push_back(mapCoord);
						}
						else if (gid == 5) { // Items : Feathers
							Vector2D mapCoord = { (float)i * 32, (float)j * 32 };
							posFeathers.push_back(mapCoord);
						}
						else if (gid == 6) { // Column Boss
							Vector2D mapCoord = { (float)i * 32, (float)j * 32 };
							posColumnBoss.push_back(mapCoord);
						}
						else if (gid == 7) { // Cloth
							Vector2D mapCoord = { (float)i * 32, (float)j * 32 };
							posCloth.push_back(mapCoord);
						}
						else if (gid == 8) { // Gloves
							Vector2D mapCoord = { (float)i * 32, (float)j * 32 };
							posGloves.push_back(mapCoord);
						}
						else if (gid == 16330) { // lIGHT
							Vector2D mapCoord = { (float)i * 32, (float)j * 32 };
							posLight.push_back(mapCoord);
						}
					}
				}
			}
		}


		float x = 0.0f;
		float y = 0.0f;
		float width = 0.0f;
		float height = 0.0f;

		for (pugi::xml_node layerNode = mapFileXML.child("map").child("objectgroup"); layerNode != NULL; layerNode = layerNode.next_sibling("objectgroup")) {

			//Get objet group name(PLATFORM OR SPIKE)
			std::string layerName = layerNode.attribute("name").as_string();

			for (pugi::xml_node tileNode = layerNode.child("object"); tileNode != NULL; tileNode = tileNode.next_sibling("object")) {

				// Asigna los valores correctos desde el XML
				x = tileNode.attribute("x").as_float();
				y = tileNode.attribute("y").as_float();
				width = tileNode.attribute("width").as_float();
				height = tileNode.attribute("height").as_float();

				ColliderType colliderType = ColliderType::PLATFORM; // Valor por defecto

				// If layer name is "spike" then asign type spike AQUI ES LO DE LAS CAPAS
				if (layerName == "Climb") {
					colliderType = ColliderType::CLIMBABLE;

					// Crear el objeto de colisi�n con el tipo determinado
					PhysBody* rect = Engine::GetInstance().physics.get()->CreateRectangleSensor(x + width / 2, y + height / 2, width, height, STATIC);
					rect->ctype = ColliderType::CLIMBABLE;
					Engine::GetInstance().physics->listToDelete.push_back(rect);
					colliders.push_back(rect);

				}

				if (layerName == "Damage") {
					colliderType = ColliderType::DAMAGE;

					// Crear el objeto de colisi�n con el tipo determinado
					PhysBody* damage = Engine::GetInstance().physics.get()->CreateRectangleSensor(x + width / 2, y + height / 2, width, height, STATIC);
					damage->ctype = ColliderType::DAMAGE;
					Engine::GetInstance().physics->listToDelete.push_back(damage);
					colliders.push_back(damage);

				}

				if (layerName == "Exit") {
					colliderType = ColliderType::CHANGE_LEVEL;

					// Crear el objeto de colisi�n con el tipo determinado
					PhysBody* Change_level = Engine::GetInstance().physics.get()->CreateRectangleSensor(x + width / 2, y + height / 2, width, height, STATIC);
					Change_level->ctype = ColliderType::CHANGE_LEVEL;
					Engine::GetInstance().physics->listToDelete.push_back(Change_level);
					colliders.push_back(Change_level);
				}

				if (layerName == "DamageRespawn") {
					colliderType = ColliderType::DAMAGE_RESPAWN;

					// Crear el objeto de colisi�n con el tipo determinado
					PhysBody* Damage_respawn = Engine::GetInstance().physics.get()->CreateRectangleSensor(x + width / 2, y + height / 2, width, height, STATIC);
					Damage_respawn->ctype = ColliderType::DAMAGE_RESPAWN;
					Engine::GetInstance().physics->listToDelete.push_back(Damage_respawn);
					colliders.push_back(Damage_respawn);
				}

			}
		}

		ret = true;

		if (ret == true)
		{
			LOG("Successfully parsed map XML file :%s", fileName.c_str());
			LOG("width : %d height : %d", mapData.width, mapData.height);
			LOG("tile_width : %d tile_height : %d", mapData.tileWidth, mapData.tileHeight);

			LOG("Tilesets----");

			//iterate the tilesets
			for (const auto& tileset : mapData.tilesets) {
				LOG("name : %s firstgid : %d", tileset->name.c_str(), tileset->firstGid);
				LOG("tile width : %d tile height : %d", tileset->tileWidth, tileset->tileHeight);
				LOG("spacing : %d margin : %d", tileset->spacing, tileset->margin);
			}

			LOG("Layers----");

			for (const auto& layer : mapData.layers) {
				LOG("id : %d name : %s", layer->id, layer->name.c_str());
				LOG("Layer width : %d Layer height : %d", layer->width, layer->height);
			}
		}
		else {
			LOG("Error while parsing map file: %s", mapPathName.c_str());
		}

		if (mapFileXML) mapFileXML.reset();

	}

	mapLoaded = ret;
	return ret;
}

Vector2D Map::MapToWorld(int x, int y) const
{
	Vector2D ret;

	ret.setX(x * mapData.tileWidth);
	ret.setY(y * mapData.tileHeight);

	return ret;
}

Vector2D Map::MapToWorldCentered(int x, int y)
{
	Vector2D ret;

	ret.setX(x * mapData.tileWidth + mapData.tileWidth / 2);
	ret.setY(y * mapData.tileHeight + mapData.tileWidth / 2);

	return ret;
}

Vector2D Map::WorldToWorldCenteredOnTile(int x, int y)
{
	Vector2D ret;

	int tileX = x / mapData.tileWidth;
	int tileY = y / mapData.tileHeight;

	ret.setX(tileX * mapData.tileWidth + mapData.tileWidth / 2);
	ret.setY(tileY * mapData.tileHeight + mapData.tileHeight / 2);

	return ret;
}




Vector2D Map::WorldToMap(int x, int y) {

	Vector2D ret(0, 0);

	if (mapData.orientation == MapOrientation::ORTOGRAPHIC) {
		ret.setX(x / mapData.tileWidth);
		ret.setY(y / mapData.tileHeight);
	}

	if (mapData.orientation == MapOrientation::ISOMETRIC) {
		float half_width = mapData.tileWidth / 2;
		float half_height = mapData.tileHeight / 2;
		ret.setX(int((x / half_width + y / half_height) / 2));
		ret.setY(int((y / half_height - (x / half_width)) / 2));
	}

	return ret;
}

bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = false;

	for (pugi::xml_node propertieNode = node.child("properties").child("property"); propertieNode; propertieNode = propertieNode.next_sibling("property"))
	{
		Properties::Property* p = new Properties::Property();
		p->name = propertieNode.attribute("name").as_string();
		p->value = propertieNode.attribute("value").as_bool(); // (!!) I'm assuming that all values are bool !!

		properties.propertyList.push_back(p);
	}

	return ret;
}

MapLayer* Map::GetNavigationLayer() {
	for (const auto& layer : mapData.layers) {
		if (layer->properties.GetProperty("Navigation") != NULL &&
			layer->properties.GetProperty("Navigation")->value) {
			return layer;
		}
	}

	return nullptr;
}

Properties::Property* Properties::GetProperty(const char* name)
{
	for (const auto& property : propertyList) {
		if (property->name == name) {
			return property;
		}
	}

	return nullptr;
}




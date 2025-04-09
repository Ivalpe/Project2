#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Log.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Player.h"
#include "Map.h"
#include "Item.h"
#include "Enemy.h"
#include "GuiControl.h"
#include "GuiManager.h"


Scene::Scene() : Module(), showPauseMenu(false), showSettingsMenu(false), GameOverMenu(false)
{
	name = "scene";
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	//L04: TODO 3b: Instantiate the player using the entity manager
	player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);
	player->SetParameters(configParameters.child("entities").child("player"));
	
	//L08 Create a new item using the entity manager and set the position to (200, 672) to test
	for (pugi::xml_node itemNode = configParameters.child("entities").child("items").child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		for (int i = 0; i < 3; i++)
		{
			Item* item = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
			item->SetParameters(itemNode);
			item->position = Vector2D(200 + (100 * i), 672);
		}

	}

	for (pugi::xml_node itemNode = configParameters.child("entities").child("items").child("feather_item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
	
		for (int i = 0; i < 3; i++)
		{
			Item* item = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
			item->SetParameters(itemNode);
			item->position = Vector2D(800 + (100 * i), 400);
		}

	}

	// Create a enemy using the entity manager 
	for (pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
	{
		Enemy* enemy = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY);
		enemy->SetParameters(enemyNode);
		enemyList.push_back(enemy);
	}

	return ret;
}

void Scene::CreateItems() 
{
	int WaxIndex = 0;
	std::vector<Vector2D> waxPositions;

	if (level == 0) {

		waxPositions = {
		  Vector2D(300, 672),
		  Vector2D(400, 672),
		  Vector2D(500, 672)
		};

		for (auto& it : itemList) {
			if (it->name == "wax" && WaxIndex < waxPositions.size()) {
				it->position = waxPositions[WaxIndex++];
			}
		}

		for (auto& it : itemList) {
			if (it->name == "feather" && WaxIndex < waxPositions.size()) {
				it->position = waxPositions[WaxIndex++];
			}
		}
	}
	else {
		for (auto& it : itemList) {
			if (it->name == "wax" && WaxIndex < waxPositions.size()) {
				it->CleanUp();
			}
		}
	}
	
}


// Called before the first frame
bool Scene::Start()
{
	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->Load(configParameters.child("map").attribute("path").as_string(), configParameters.child("map").attribute("name").as_string());

	// Texture to highligh mouse position 
	mouseTileTex = Engine::GetInstance().textures.get()->Load("Assets/Maps/MapMetadata.png");

	// Initalize the camera position
	int w, h;
	Engine::GetInstance().window.get()->GetWindowSize(w, h);
	Engine::GetInstance().render.get()->camera.x = 0;
	Engine::GetInstance().render.get()->camera.y = 0;


	Menu_Pause = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("Menu_Pause").attribute("path").as_string());
	Menu_Settings = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("Menu_Settings").attribute("path").as_string());
	GameOver = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("GameOver").attribute("path").as_string());
	Feather = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("Feather").attribute("path").as_string());
	FeatherTexture = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("FeatherUI").attribute("path").as_string());
	waxTexture = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("WaxUI").attribute("path").as_string());

	return true;
}

void Scene::Change_level(int level)
{

	if (level == 0)
	{
		Uint32 startTime = SDL_GetTicks();

		Engine::GetInstance().map.get()->CleanUp();
		Engine::GetInstance().entityManager.get()->RemoveAllEnemies();
		Engine::GetInstance().entityManager.get()->RemoveAllItems();
		Engine::GetInstance().map->Load(configParameters.child("map").attribute("path").as_string(), configParameters.child("map").attribute("name").as_string());
		CreateItems();
		//CreateEnemies();

	}

	if (level == 1)
	{
		Uint32 startTime = SDL_GetTicks();

		Engine::GetInstance().map.get()->CleanUp();
		//REMOVE // WHEN SECOND STAGE ENEMYS ADDED
		//Engine::GetInstance().entityManager.get()->RemoveAllEnemies();
		//Engine::GetInstance().entityManager.get()->RemoveAllItems();
		Engine::GetInstance().map->Load(configParameters.child("map1").attribute("path").as_string(), configParameters.child("map1").attribute("name").as_string());
		CreateItems();

	}
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	float camSpeed = 1;
	int Px = player->position.getX();
	int Py = player->position.getY();


	Engine::GetInstance().render.get()->camera.x = -(Px - 700);
	Engine::GetInstance().render.get()->camera.y = -(Py - 700);


	//if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	//	Engine::GetInstance().render.get()->camera.y -= ceil(camSpeed * dt);

	//if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	//	Engine::GetInstance().render.get()->camera.y += ceil(camSpeed * dt);

	//if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	//	Engine::GetInstance().render.get()->camera.x -= ceil(camSpeed * dt);

	//if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	//	Engine::GetInstance().render.get()->camera.x += ceil(camSpeed * dt);

	// L10 TODO 6: Implement a method that repositions the player in the map with a mouse click
	
	//Get mouse position and obtain the map coordinate
	Vector2D mousePos = Engine::GetInstance().input.get()->GetMousePosition();
	Vector2D mouseTile = Engine::GetInstance().map.get()->WorldToMap(mousePos.getX() - Engine::GetInstance().render.get()->camera.x,
																     mousePos.getY() - Engine::GetInstance().render.get()->camera.y);


	//Render a texture where the mouse is over to highlight the tile, use the texture 'mouseTileTex'
	Vector2D highlightTile = Engine::GetInstance().map.get()->MapToWorld(mouseTile.getX(),mouseTile.getY());
	SDL_Rect rect = { 0,0,32,32 };
	Engine::GetInstance().render.get()->DrawTexture(mouseTileTex,
													highlightTile.getX(),
													highlightTile.getY(),
													&rect);

	// saves the tile pos for debugging purposes
	if (mouseTile.getX() >= 0 && mouseTile.getY() >= 0 || once) {
		tilePosDebug = "[" + std::to_string((int)mouseTile.getX()) + "," + std::to_string((int)mouseTile.getY()) + "] ";
		once = true;
	}

	//If mouse button is pressed modify enemy position
	if (Engine::GetInstance().input.get()->GetMouseButtonDown(1) == KEY_DOWN) {
		enemyList[0]->SetPosition(Vector2D(highlightTile.getX(), highlightTile.getY()));
		enemyList[0]->ResetPath();
	}

	//Reset level
	if (reset_level) {
		Change_level(level);
		if(level==0) player->SetPosition(Vector2D{ 40,70 });

		reset_level = false;
	}

	//Pause menu
	Active_MenuPause();

	GameOver_State();
	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	//if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	//	ret = false;
	
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_0) == KEY_DOWN) {
		Change_level(0);
		level = 0;
		player->SetPosition(Vector2D{ 784, 600 });
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
		Change_level(1);
		level = 1;
		player->SetPosition(Vector2D{ 40, 70 });
	}

	if (!showPauseMenu && !showSettingsMenu) {
		//Life texture
		for (int i = 0; i < player->GetWax(); i++) {
			Engine::GetInstance().render.get()->DrawTexture(waxTexture, 10 + (i * 40), 10, nullptr, false);
		}

		//Wax texture
		Engine::GetInstance().render.get()->DrawTexture(FeatherTexture, 10, 50, nullptr, false);
		char FeatherText[64];
		sprintf_s(FeatherText, " x%d", Engine::GetInstance().entityManager->feather);
		Engine::GetInstance().render.get()->DrawText(FeatherText, 50, 55, 40, 30);
	}
	
	if (Engine::GetInstance().scene.get()->showPauseMenu == false && Engine::GetInstance().scene.get()->showSettingsMenu == false) Engine::GetInstance().map.get()->DrawFront();
	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");
	return true;
}

// Return the player position
Vector2D Scene::GetPlayerPosition()
{
	return player->GetPosition();
}

void Scene::GameOver_State()
{
	if (Engine::GetInstance().entityManager.get()->wax<=0){

		if (!GameOverMenu) {
			GameOverMenu = true;
		}
		if (player != nullptr) {
			player->CleanUp();
		}
		Engine::GetInstance().entityManager.get()->RemoveAllEnemies();
		Engine::GetInstance().entityManager.get()->RemoveAllItems();

		if (Engine::GetInstance().guiManager != nullptr)	Engine::GetInstance().guiManager->CleanUp();

		int cameraX = Engine::GetInstance().render.get()->camera.x;
		int cameraY = Engine::GetInstance().render.get()->camera.y;

		Engine::GetInstance().render.get()->DrawTexture(GameOver, -cameraX, -cameraY);

		SDL_Rect Continue = { 865, 760, 245, 75 };
		SDL_Rect Exit = { 940, 860, 95, 75 };

		guiBt = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 7, "Continue", Continue, this));
		guiBt1 = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 6, "Exit", Exit, this));
	}
		

		
	
	
}

void Scene::Active_MenuPause() {
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		showPauseMenu = !showPauseMenu;
		showSettingsMenu = false;
		if (showPauseMenu) {
			player->StopMovement();
			for (Enemy* enemy : enemyList) {
				if (enemy!=NULL) {
					enemy->visible = false;
					enemy->StopMovement();
				}
			}
			for (Item* item : itemList) {
				if (item != NULL) {
					item->apear = false;

				}
			}
		}
		else if(!showPauseMenu){
			player->ResumeMovement();
			for (Enemy* enemy : enemyList) {
				if (enemy) {
					enemy->visible = true;
					enemy->ResumeMovement();
				}
			}
			for (Item* item : itemList) {
				item->apear = true;
			}

			DisableGuiControlButtons();
		}
	}

	if (showPauseMenu) {

		MenuPause();
		if (showSettingsMenu) {
			MenuSettings();
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN) 
			{
				showSettingsMenu = false;
			}
		}
	}

}

void Scene::MenuPause()
{
	if (Engine::GetInstance().guiManager != nullptr)	Engine::GetInstance().guiManager->CleanUp();

	int cameraX = Engine::GetInstance().render.get()->camera.x;
	int cameraY = Engine::GetInstance().render.get()->camera.y;

	Engine::GetInstance().render.get()->DrawTexture(Menu_Pause, -cameraX, -cameraY);

	SDL_Rect ConitnuesButton = { 840, 520, 200, 45 };
	SDL_Rect Settings = { 860, 595, 150, 45 };
	SDL_Rect Exit = { 900-3, 670, 75, 35 };

	guiBt = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "Continue", ConitnuesButton, this));
	guiBt1 = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "Settings", Settings, this));
	guiBt2 = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, "Exit", Exit, this));
}

void Scene::MenuSettings()
{
	if (Engine::GetInstance().guiManager != nullptr)	Engine::GetInstance().guiManager->CleanUp();

	Vector2D mousePos = Engine::GetInstance().input.get()->GetMousePosition();


	int cameraX = Engine::GetInstance().render.get()->camera.x;
	int cameraY = Engine::GetInstance().render.get()->camera.y;

	Engine::GetInstance().render.get()->DrawTexture(Menu_Settings, -cameraX, -cameraY);

	Engine::GetInstance().render.get()->DrawText("Settings", 860, 325, 250, 65);
	Engine::GetInstance().render.get()->DrawText("Music Volumen", 700, 485, 220, 35);
	Engine::GetInstance().render.get()->DrawText("Ambient Sounds", 700, 555, 220, 35);
	Engine::GetInstance().render.get()->DrawText("Lenguage", 700, 630, 150, 35);
	Engine::GetInstance().render.get()->DrawText("English", 1085, 625, 100, 35);


	SDL_Rect MusicPosition = { musicPosX, 10, 485, 35 };
	guiBt->bounds.x = musicPosX; // Reposition the music volume button
	SDL_Rect FxPosition = { ambient_soundsPosX, 10, 555, 35 };
	guiBt1->bounds.x = ambient_soundsPosX; // Reposition the ambient sounds volume button

	guiBt = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, "  ", MusicPosition, this));
	guiBt1 = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, "  ", FxPosition, this));

	// Prevent both ambient sounds and Music Volume from moving at the same time. Check the mouse position on the Y axis
	if (mousePos.getX() >= musicPosX && mousePos.getX() <= musicPosX + 6 &&	mousePos.getY() >= 511 - 5 && mousePos.getY() <= 511 + 10)
	{
		mouseOverMusicControl = true; // Mouse is over the music volume control
	}
	else {
		mouseOverMusicControl = false; // Mouse is not over the music control
	}

	// Check if the mouse is over the Ambient sounds control
	if (mousePos.getX() >= ambient_soundsPosX && mousePos.getX() <= ambient_soundsPosX + 6 && mousePos.getY() >= 571 - 5 && mousePos.getY() <= 571 + 10) {
		mouseOverAmbientControl = true;  // Mouse is over the ambient sounds volume control
	}
	else {
		mouseOverAmbientControl = false; // Mouse is not over the ambient sounds volume control
	}

	if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
		if (mouseOverMusicControl) {
			musicButtonHeld = true;  // Enable movement only if mouse is over the music button
		}
		if (mouseOverAmbientControl) {
			Ambient_Sounds_ButtonHeld = true;  // Enable movement only if mouse is over the ambient sounds button
		}
	}

	if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
		Ambient_Sounds_ButtonHeld = false;   // Disable movement when mouse button is released - ambient sounds    
		musicButtonHeld = false;  // // Disable movement when mouse button is released - Music volume
	}

	if (musicButtonHeld) {

		if (mousePos.getX() < 1034) {
			musicPosX = 1034;
		}
		else if (mousePos.getX() > 1225) {
			musicPosX = 1225;
		}
		else {
			musicPosX = mousePos.getX();
		}
	}

	if (Ambient_Sounds_ButtonHeld) {

		if (mousePos.getX() < 1034) {
			ambient_soundsPosX = 1034;
		}
		else if (mousePos.getX() > 1225) {
			ambient_soundsPosX = 1225;
		}
		else {
			ambient_soundsPosX = mousePos.getX();
		}
	}

	SDL_Rect newMusicPos = { musicPosX, 511-5, 6, 15 }; // New music volume position 
	guiBt->bounds = newMusicPos;

	SDL_Rect newFxPos = { ambient_soundsPosX, 571-5, 6, 15 }; // New music ambient sounds position
	guiBt1->bounds = newFxPos;

	// Adjust music volume
	float volume = (float)(musicPosX - 1034) / (1225 - 1034);
	volume = std::max(0.0f, std::min(1.0f, volume));
	sdlVolume = (int)(volume * MIX_MAX_VOLUME);
	Mix_VolumeMusic(sdlVolume);

	// Adjust Ambient sounds
	float volumeFx = (float)(ambient_soundsPosX - 1034) / (1225 - 1034);
	int sdlVolumeFx = (int)(volumeFx * MIX_MAX_VOLUME);
	Mix_Volume(-1, sdlVolumeFx);

	// Music volume background bar
	Engine::GetInstance().render.get()->DrawRectangle({ 1034, 511, 195, 6 }, 0, 0, 0, 255, true, false);
	//Ambient sounds background bar
	Engine::GetInstance().render.get()->DrawRectangle({ 1034, 570 + 1, 195, 6 }, 0, 0, 0, 255, true, false);

	// Music volume filled portion
	Engine::GetInstance().render.get()->DrawRectangle({ 1034, 511, musicPosX - 1034, 6 }, 255, 255, 255, 255, true, false);
	//Ambient sounds filled portion
	Engine::GetInstance().render.get()->DrawRectangle({ 1034, 570 + 1, ambient_soundsPosX - 1034, 6 }, 255, 255, 255, 255, true, false);
}

void Scene::DisableGuiControlButtons() 
{
	guiBt->state = GuiControlState::DISABLED;
	guiBt1->state = GuiControlState::DISABLED;
	guiBt2->state = GuiControlState::DISABLED;
}
bool Scene::OnGuiMouseClickEvent(GuiControl* control)
{
	switch (control->id) {
	case 1:
		showPauseMenu = false;

		player->ResumeMovement();
		for (Enemy* enemy : enemyList) {
			if (enemy) {
				enemy->visible = true;
				enemy->ResumeMovement();
			}
		}
		for (Item* item : itemList) {
			item->apear = true;
		}
		DisableGuiControlButtons();
		break;
	case 2:
		showSettingsMenu = true;
		break;
	case 3:
		exit(0);
		DisableGuiControlButtons();
		break;
	case 4:	// Settings: Music Volume
		musicButtonHeld = true;
		break;
	case 5:// Settings: Fx Volume
		Ambient_Sounds_ButtonHeld = true;
		break;
	case 6:	// Game Over: Continue
		CreateItems();
		guiBt->state = GuiControlState::DISABLED;
		guiBt1->state = GuiControlState::DISABLED;
		player->Start();
		Engine::GetInstance().entityManager->wax = 3;
		Engine::GetInstance().entityManager->feather = 0;
		break;
	case 7:// Game Over: Exit
		exit(0);
		DisableGuiControlButtons();
		break;
	}
	return true;
}
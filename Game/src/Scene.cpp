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
#include "InteractiveObject.h"
#include "Enemy.h"
#include "Soldier.h"
#include "GuiControl.h"
#include "GuiManager.h"
#include "Physics.h"
#include "Platform.h"



Scene::Scene() : Module(), showPauseMenu(false), showSettingsMenu(false), GameOverMenu(false)
{
	name = "scene";
}

// Destructor
Scene::~Scene()
{
}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);
	player->SetParameters(configParameters.child("entities").child("player"));

	for (pugi::xml_node InteractiveObjectNode = configParameters.child("entities").child("interactiveObject").child("stalactites_item"); InteractiveObjectNode; InteractiveObjectNode = InteractiveObjectNode.next_sibling("interactiveObject"))
	{

		InteractiveObject* interactiveObject = (InteractiveObject*)Engine::GetInstance().entityManager->CreateEntity(EntityType::INTERACTIVEOBJECT);
		interactiveObject->SetParameters(InteractiveObjectNode);
		interactiveObjectList.push_back(interactiveObject);
		interactiveObject->name = "stalactites";
	}

	for (pugi::xml_node InteractiveObjectNode = configParameters.child("entities").child("interactiveObject").child("blocked_wall"); InteractiveObjectNode; InteractiveObjectNode = InteractiveObjectNode.next_sibling("interactiveObject"))

	{

		InteractiveObject* interactiveObject = (InteractiveObject*)Engine::GetInstance().entityManager->CreateEntity(EntityType::INTERACTIVEOBJECT);
		interactiveObject->SetParameters(InteractiveObjectNode);
		interactiveObjectList.push_back(interactiveObject);
		interactiveObject->name = "wall";
	}

	for (pugi::xml_node PlatformObjectNode = configParameters.child("entities").child("platforms").child("platform"); PlatformObjectNode; PlatformObjectNode = PlatformObjectNode.next_sibling("platforms"))

	{
		for (int i = 0; i < 3; i++)
		{
			Platform* PlatformObject = (Platform*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLATFORM);
			PlatformObject->SetParameters(PlatformObjectNode);
			platformList.push_back(PlatformObject);
			PlatformObject->name = "platform" + std::to_string(i);
		}

	}

	CreateEnemies(level);
	CreateItems(level);

	return ret;
}

void Scene::CreateEnemies(int level)
{
	for (auto e : enemyList) {
		Engine::GetInstance().physics->DeleteBody((e)->GetAttackSensorBody());
		Engine::GetInstance().physics->DeleteBody((e)->GetSensorBody());
		Engine::GetInstance().entityManager->DestroyEntity(e);
	}
	enemyList.clear();

	for (auto e : bossList) {
		Engine::GetInstance().physics->DeleteBody((e)->GetSensorLeftBody());
		Engine::GetInstance().physics->DeleteBody((e)->GetSensorLimitLeft());
		Engine::GetInstance().physics->DeleteBody((e)->GetSensorRight());
		Engine::GetInstance().entityManager->DestroyEntity(e);
	}
	bossList.clear();

	for (auto e : columnList) {
		Engine::GetInstance().entityManager->DestroyEntity(e);
	}
	columnList.clear();

	std::vector<Vector2D> listEnemy;

	//Enemies
	listEnemy = Engine::GetInstance().map->GetEnemyList();
	for (auto enemy : listEnemy) {
		Enemy* en;
		en = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY);
		en->SetParameters(configParameters.child("entities").child("enemies").child("soldier"));
		en->Start();
		en->SetPosition({ enemy.getX(), enemy.getY() });
		enemyList.push_back(en);
	}

	//Column Boss
	listEnemy = Engine::GetInstance().map->GetColumnBossList();
	for (auto columnBoss : listEnemy) {
		Column* col = (Column*)Engine::GetInstance().entityManager->CreateEntity(EntityType::COLUMN);
		col->SetParameters(configParameters.child("entities").child("columns").child("minotaur"));
		col->Start();
		columnList.push_back(col);
	}

	//Boss
	listEnemy = Engine::GetInstance().map->GetBossList();
	for (auto bosses : listEnemy) {
		Boss* boss = (Boss*)Engine::GetInstance().entityManager->CreateEntity(EntityType::BOSS);
		boss->SetParameters(configParameters.child("entities").child("enemies").child("minotaur"));
		boss->Start();
		boss->SetPosition({ bosses.getX(), bosses.getY() });
		bossList.push_back(boss);
	}
}
void Scene::CreateItems(int level)
{
	for (auto e : itemList) {
		//Engine::GetInstance().physics->DeleteBody((e)->GetBody());
		//Engine::GetInstance().entityManager->DestroyEntity(e);
	}
	itemList.clear();

	std::vector<Vector2D> listItems;

	listItems = Engine::GetInstance().map->GetWaxList();
	for (auto wax : listItems) {
		Item* item = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
		item->SetParameters(configParameters.child("entities").child("items").child("item"));
		item->name = "wax";
		item->Start();
		item->SetPosition({ wax.getX(), wax.getY() });
		itemList.push_back(item);
	}

	listItems = Engine::GetInstance().map->GetFeathersList();
	for (auto feather : listItems) {
		Item* item = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
		item->SetParameters(configParameters.child("entities").child("items").child("feather_item"));
		item->name = "feather";
		item->Start();
		item->SetPosition({ feather.getX(), feather.getY() });
		itemList.push_back(item);
	}
}

// Called before the first frame
bool Scene::Start()
{
	Engine::GetInstance().map->Load(configParameters.child("map").attribute("path").as_string(), configParameters.child("map").attribute("name").as_string());

	// Texture to highligh mouse position 
	mouseTileTex = Engine::GetInstance().textures.get()->Load("Assets/Maps/MapMetadata.png");

	// Initalize the camera position
	int w, h;
	Engine::GetInstance().window.get()->GetWindowSize(w, h);
	Engine::GetInstance().render.get()->camera.x = 0;
	Engine::GetInstance().render.get()->camera.y = 0;


	InitialScreen = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("Menu_initial").attribute("path").as_string());

	WaxiFloatingTexture = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("WaxiFloating").attribute("texture").as_string());

	WaxiFloating_idle.LoadAnimations(configParameters.child("textures").child("WaxiFloating").child("animations").child("idle"));
	WaxiFloating_currentAnimation = &WaxiFloating_idle;
	WaxiFloatingPos.setX(configParameters.child("textures").child("WaxiFloating").attribute("x").as_int());
	WaxiFloatingPos.setY(configParameters.child("textures").child("WaxiFloating").attribute("y").as_int());


	Menu_Pause = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("Menu_Pause").attribute("path").as_string());
	Menu_Pause_Back = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("Menu_Pause_back").attribute("path").as_string());
	Menu_Settings = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("Menu_Settings").attribute("path").as_string());
	Menu_Settings_Back = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("Menu_Settings_back").attribute("path").as_string());
	GameOver = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("GameOver").attribute("path").as_string());
	GameOver_Back = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("GameOver_back").attribute("path").as_string());
	Feather = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("Feather").attribute("path").as_string());
	FeatherTexture = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("FeatherUI").attribute("path").as_string());
	waxTexture = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("WaxUI").attribute("path").as_string());


	empty.LoadAnimations(configParameters.child("textures").child("WaxUI").child("animations").child("empty"));
	fill2lvl1.LoadAnimations(configParameters.child("textures").child("WaxUI").child("animations").child("fill_to_lvl1"));
	staticlvl1.LoadAnimations(configParameters.child("textures").child("WaxUI").child("animations").child("still_lvl1"));
	fill2lvl2.LoadAnimations(configParameters.child("textures").child("WaxUI").child("animations").child("fill_to_lvl2"));
	staticlvl2.LoadAnimations(configParameters.child("textures").child("WaxUI").child("animations").child("still_lvl2"));
	fill2lvl3.LoadAnimations(configParameters.child("textures").child("WaxUI").child("animations").child("fill_to_lvl3"));
	staticlvl3.LoadAnimations(configParameters.child("textures").child("WaxUI").child("animations").child("still_lvl3"));
	fill2full.LoadAnimations(configParameters.child("textures").child("WaxUI").child("animations").child("fill_to_full"));
	full.LoadAnimations(configParameters.child("textures").child("WaxUI").child("animations").child("full"));

	candle = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("Candle").attribute("path").as_string());

	MoonTexture = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("moon").attribute("texture").as_string());

	idle.LoadAnimations(configParameters.child("textures").child("moon").child("animations").child("idle"));
	currentAnimation = &idle;
	MoonPos.setX(configParameters.child("textures").child("moon").attribute("x").as_int());
	MoonPos.setY(configParameters.child("textures").child("moon").attribute("y").as_int());

	return true;
}

void Scene::Change_level(int level)
{

	for (auto e : itemList) {
		//Engine::GetInstance().physics->DeleteBody((e)->GetBody());
		Engine::GetInstance().entityManager->DestroyEntity(e);
	}
	itemList.clear();

	for (auto e : interactiveObjectList) {
		Engine::GetInstance().physics->DeleteBody((e)->GetBody());
		Engine::GetInstance().entityManager->DestroyEntity(e);
	}
	interactiveObjectList.clear();

	for (auto e : platformList) {
		Engine::GetInstance().physics->DeleteBody((e)->GetBody());
		Engine::GetInstance().entityManager->DestroyEntity(e);
	}
	platformList.clear();

	if (level == 0)
	{
		Engine::GetInstance().map.get()->CleanUp();
		Engine::GetInstance().map->Load(configParameters.child("map").attribute("path").as_string(), configParameters.child("map").attribute("name").as_string());
		CreateItems(level);
		CreateEnemies(level);
	}

	else if (level == 1) {
		Engine::GetInstance().map.get()->CleanUp();
		LOG("Current Level: %d", level);
		Engine::GetInstance().map->Load(configParameters.child("map1").attribute("path").as_string(), configParameters.child("map1").attribute("name").as_string());

		CreateItems(level);
		CreateEnemies(level);

		showBlackTransition = true;
		blackTransitionStart = SDL_GetTicks();


	}

	else if (level == 2) {
		Engine::GetInstance().map.get()->CleanUp();
		Engine::GetInstance().map->Load(configParameters.child("map2").attribute("path").as_string(), configParameters.child("map2").attribute("name").as_string());
		CreateItems(level);
		CreateEnemies(level);
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

	int camX = -(Px - 700);
	int camY = -(Py - 600);

	// Limitar la cámara principio
	if (camX > 0) camX = 0;
	if (camY > 0) camY = 0;

	// Limitar la cámara final
	if (camX < -11520)camX = -11520;

	Engine::GetInstance().render.get()->camera.x = (camX);
	Engine::GetInstance().render.get()->camera.y = (camY  /*+ player->crouch*/);

	//Reset levels
	if (reset_level) {
		Change_level(level);
		if (level == 1) player->SetPosition(Vector2D{ 40,70 });
		reset_level = false;
	}

	

	//Moon animation
	if (level == 0) {
		Engine::GetInstance().render.get()->DrawTexture(MoonTexture, (int)MoonPos.getX(), (int)MoonPos.getY(), &currentAnimation->GetCurrentFrame());
		currentAnimation->Update();
	}

	// Wax animation
	animationWaxy();

	//Pause menu
	Active_MenuPause();

	GameOver_State();

	MenuInitialScreen();

	if (showSettingsMenu) 	MenuSettings();

	////Waxi floating
	//if (level == 0) {
	//	Engine::GetInstance().render.get()->DrawTexture(WaxiFloatingTexture, (int)WaxiFloatingPos.getX(), (int)WaxiFloatingPos.getY(), &WaxiFloating_currentAnimation->GetCurrentFrame());
	//	WaxiFloating_currentAnimation->Update();
	//}

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
		level = 1;
		Change_level(level);
		player->SetPosition(Vector2D{ 64, 64 });
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_2) == KEY_DOWN) {
		level = 2;
		Change_level(level);
		player->SetPosition(Vector2D{ 64, 64 });
	}

	show_UI();

	if (showBlackTransition) {
		Uint32 now = SDL_GetTicks();
		Uint32 elapsed = now - blackTransitionStart;

		if (elapsed < blackTransitionDuration) {

			Uint8 alpha = 255 * (1 - (float(elapsed) / blackTransitionDuration));  // Reduce la opacidad gradualmente

			SDL_Rect BlackTransition = { 0, 0, 1920, 1080 };
			SDL_SetRenderDrawColor(Engine::GetInstance().render.get()->renderer, 0, 0, 0, alpha);
			SDL_RenderFillRect(Engine::GetInstance().render.get()->renderer, &BlackTransition);
		}
		else {
			showBlackTransition = false; // Termina la transici�n
		}
	}

	if (Engine::GetInstance().scene.get()->showPauseMenu == false && Engine::GetInstance().scene.get()->showSettingsMenu == false && Engine::GetInstance().scene.get()->GameOverMenu == false) {
		Engine::GetInstance().map.get()->DrawFront();
	}

	return ret;
}

void Scene::show_UI() {
	//Show UI when pressing E or velocity player 0,0
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_E) == KEY_DOWN || player->velocity.x == 0 && player->velocity.y == 0)
	{
		UI = true;
		current_time = 0;
	}


	if (UI) {
		current_time++;

		if (current_time > 180) UI = false; //3 seconds
	}
	if (!showPauseMenu && !showSettingsMenu && !GameOverMenu && !InitialScreenMenu && UI) {

		//Crea

		Engine::GetInstance().render.get()->DrawTexture(waxTexture, 10, 10, &currentWaxAnim->GetCurrentFrame(), false);

		//vela
		for (int i = 0; i < Engine::GetInstance().entityManager.get()->candleNum; i++) {
			Engine::GetInstance().render.get()->DrawTexture(candle, 150 + (i * 40), 50, nullptr, false);
		}

		//Wax texture
		Engine::GetInstance().render.get()->DrawTexture(FeatherTexture, 20, 150, nullptr, false);
		char FeatherText[64];
		sprintf_s(FeatherText, " x%d", Engine::GetInstance().entityManager->feather);
		Engine::GetInstance().render.get()->DrawText(FeatherText, 90, 165, 40, 30);
	}
}

void Scene::animationWaxy()
{
	if (!filledWaxy) {
		if (waxState == FULL) resetWax.Start();
		if (shouldFillWaxy) {
			FillWaxy();
		}
	}

	if (!drainedWaxy) {
		if (waxState == EMPTY) resetWax.Start();
		DrainWaxy();
	}

	//Wax animation
	switch (waxState)
	{
	case EMPTY:
		currentWaxAnim = &empty;
		break;
	case FILL_TO_LOW:
		if (currentWaxAnim != &fill2lvl1) {
			fill2lvl1.Reset();
			currentWaxAnim = &fill2lvl1;
		}
		break;
	case QUARTER_FULL:
		currentWaxAnim = &staticlvl1;
		break;
	case FILL_TO_HALF:
		if (currentWaxAnim != &fill2lvl2) {
			fill2lvl2.Reset();
			currentWaxAnim = &fill2lvl2;
		}
		break;
	case HALF_FULL:
		currentWaxAnim = &staticlvl2;
		break;
	case FILL_TO_HIGH:
		if (currentWaxAnim != &fill2lvl3) {
			fill2lvl3.Reset();
			currentWaxAnim = &fill2lvl3;
		}
		break;
	case ALMOST_FULL:
		currentWaxAnim = &staticlvl3;
		break;
	case FILL_TO_FULL:
		if (currentWaxAnim != &fill2full) {
			fill2full.Reset();
			currentWaxAnim = &fill2full;
		}
		break;
	case FULL:
		currentWaxAnim = &full;
		break;
	}

	currentWaxAnim->Update();
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

void Scene::MenuInitialScreen()
{
	if (InitialScreenMenu) {
		if (Engine::GetInstance().guiManager != nullptr)	Engine::GetInstance().guiManager->CleanUp();

		int cameraX = Engine::GetInstance().render.get()->camera.x;
		int cameraY = Engine::GetInstance().render.get()->camera.y;


		Engine::GetInstance().render.get()->DrawTexture(InitialScreen, -cameraX, -cameraY);


		int textWidthNewGame, textHeightNewGame;
		int textWidthContinue, textHeightContinue;
		int textWidthSettings, textHeightSettings;
		int textWidthExit, textHeightExit;

		TTF_SizeText(Engine::GetInstance().render.get()->font, "Start Game", &textWidthNewGame, &textHeightNewGame);
		TTF_SizeText(Engine::GetInstance().render.get()->font, "Continue", &textWidthContinue, &textHeightContinue);
		TTF_SizeText(Engine::GetInstance().render.get()->font, "Settings", &textWidthSettings, &textHeightSettings);
		TTF_SizeText(Engine::GetInstance().render.get()->font, "Quit", &textWidthExit, &textHeightExit);

		float scaleFactor = 0.9f;

		SDL_Rect NewGameButton = { 300 - 60 - 7-100+7, 445 + 50+10-5, static_cast<int>(textWidthNewGame * scaleFactor)+10, static_cast<int>(textHeightNewGame * scaleFactor)+10 };
		SDL_Rect ConitnuesButton = { 320 - 50 - 7-130+8, 520 + 50-5-5-3, static_cast<int>(textWidthContinue * scaleFactor)+10, static_cast<int>(textHeightContinue * scaleFactor)+10 };
		SDL_Rect Settings = { 330 - 45 - 7-130-5, 595 + 50-25-5, static_cast<int>(textWidthSettings * scaleFactor)+10, static_cast<int>(textHeightSettings * scaleFactor)+10 };
		SDL_Rect Exit = { 350 - 25 - 7-150-30+3, 670+10-5-3 , static_cast<int>(textWidthExit * scaleFactor)+10, static_cast<int>(textHeightExit * scaleFactor)+10 };


		guiBt0 = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 8, "", NewGameButton, this));
		guiBt = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 9, "", ConitnuesButton, this));
		guiBt1 = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 10, "", Settings, this));
		guiBt2 = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 11, "", Exit, this));

		Engine::GetInstance().render.get()->DrawTexture(WaxiFloatingTexture, (int)WaxiFloatingPos.getX(), (int)WaxiFloatingPos.getY(), &WaxiFloating_currentAnimation->GetCurrentFrame());
		WaxiFloating_currentAnimation->Update();
	}
}

void Scene::GameOver_State()
{
	if (Engine::GetInstance().entityManager->candleNum <= 0) {

		if (!GameOverMenu) {
			GameOverMenu = true;
		}

		if (Engine::GetInstance().guiManager != nullptr)	Engine::GetInstance().guiManager->CleanUp();

		if (player->pbody != nullptr) {
			Engine::GetInstance().physics.get()->DeletePhysBody(player->pbody);
			player->pbody = nullptr;

			player->pbody = Engine::GetInstance().physics.get()->CreateCircle(player->position.getX(), player->position.getY(), 32, bodyType::DYNAMIC);
			player->pbody->ctype = ColliderType::PLAYER;
		}
		int cameraX = Engine::GetInstance().render.get()->camera.x;
		int cameraY = Engine::GetInstance().render.get()->camera.y;

		SDL_SetTextureAlphaMod(GameOver_Back, 200);

		Engine::GetInstance().render.get()->DrawTexture(GameOver_Back, -cameraX, -cameraY);

		Engine::GetInstance().render.get()->DrawTexture(GameOver, -cameraX, -cameraY);

		int textWidthContinue, textHeightContinue;
		int textWidthExit, textHeightExit;

		TTF_SizeText(Engine::GetInstance().render.get()->font, "Continue", &textWidthContinue, &textHeightContinue);
		TTF_SizeText(Engine::GetInstance().render.get()->font, "Exit", &textWidthExit, &textHeightExit);

		SDL_Rect Continue = { 865+20, 760+10-5, textWidthContinue + 10, textHeightContinue + 10 };
		SDL_Rect Exit = { 940-10, 860-15-5, textWidthExit + 10, textHeightExit + 10 };

		guiBt = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 6, "Continue", Continue, this));
		guiBt1 = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 7, "Exit", Exit, this));
	}
}

void Scene::Active_MenuPause() {

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		showPauseMenu = !showPauseMenu;
		showSettingsMenu = false;
		if (showPauseMenu) {
			player->StopMovement();
			for (Enemy* enemy : enemyList) {
				if (enemy != NULL) {
					enemy->visible = false;
					enemy->StopMovement();
				}
			}
			for (Platform* platform : platformList) {
				if (platform != NULL) {
					platform->StopMovement();
				}
			}
		}
		else if (!showPauseMenu) {
			player->ResumeMovement();
			for (Enemy* enemy : enemyList) {
				if (enemy) {
					enemy->visible = true;
					enemy->ResumeMovement();
				}
			}
			for (Platform* platform : platformList) {
				if (platform != NULL) {
					platform->ResumeMovement();
				}
			}
			DisableGuiControlButtons();
		}
	}
	if (showPauseMenu) {
		if (!showSettingsMenu) {
			MenuPause(); 
		}

		if (showSettingsMenu) {
			MenuSettings();

			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN) {
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


	SDL_SetTextureAlphaMod(Menu_Pause_Back, 128);

	Engine::GetInstance().render.get()->DrawTexture(Menu_Pause_Back, -cameraX, -cameraY);

	Engine::GetInstance().render.get()->DrawTexture(Menu_Pause, -cameraX, -cameraY);


	int textWidthContinue, textHeightContinue;
	int textWidthSettings, textHeightSettings;
	int textWidthExit, textHeightExit;

	TTF_SizeText(Engine::GetInstance().render.get()->font, "Continue", &textWidthContinue, &textHeightContinue);
	TTF_SizeText(Engine::GetInstance().render.get()->font, "Settings", &textWidthSettings, &textHeightSettings);
	TTF_SizeText(Engine::GetInstance().render.get()->font, "Exit", &textWidthExit, &textHeightExit);


	SDL_Rect ConitnuesButton = { 862 +7+2, 520 - 15-30+10, textWidthContinue + 20, textHeightContinue + 10 };
	SDL_Rect Settings = { 882 +7, 595 - 10-30+10, textWidthSettings + 20, textHeightSettings + 10 };
	SDL_Rect Exit = { 919 +7, 670 - 5-30, textWidthExit + 20, textHeightExit + 10 };


	guiBt = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "", ConitnuesButton, this));
	guiBt1 = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "", Settings, this));
	guiBt2 = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, "", Exit, this));
}

void Scene::MenuSettings()
{
	if (Engine::GetInstance().guiManager != nullptr)	Engine::GetInstance().guiManager->CleanUp();

	Vector2D mousePos = Engine::GetInstance().input.get()->GetMousePosition();


	int cameraX = Engine::GetInstance().render.get()->camera.x;
	int cameraY = Engine::GetInstance().render.get()->camera.y;

	SDL_SetTextureAlphaMod(Menu_Settings_Back, 128);

	Engine::GetInstance().render.get()->DrawTexture(Menu_Settings_Back, -cameraX, -cameraY);
	Engine::GetInstance().render.get()->DrawTexture(Menu_Settings, -cameraX, -cameraY);


	SDL_Rect MusicPosition = { musicPosX, 10, 485, 35 };
	guiBt->bounds.x = musicPosX; // Reposition the music volume button
	SDL_Rect FxPosition = { ambient_soundsPosX, 10, 555, 35 };
	guiBt1->bounds.x = ambient_soundsPosX; // Reposition the ambient sounds volume button

	guiBt = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, "  ", MusicPosition, this));
	guiBt1 = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, "  ", FxPosition, this));

	// Prevent both ambient sounds and Music Volume from moving at the same time. Check the mouse position on the Y axis
	if (mousePos.getX() >= musicPosX && mousePos.getX() <= musicPosX + 6 && mousePos.getY() >= 511 - 5 && mousePos.getY() <= 511 + 10)
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

	SDL_Rect newMusicPos = { musicPosX, 511 - 5-7, 6, 15 }; // New music volume position 
	guiBt->bounds = newMusicPos;

	SDL_Rect newFxPos = { ambient_soundsPosX, 571 - 5+7, 6, 15 }; // New music ambient sounds position
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
	Engine::GetInstance().render.get()->DrawRectangle({ 1034, 511-7, 195, 6 }, 0, 0, 0, 255, true, false);
	//Ambient sounds background bar
	Engine::GetInstance().render.get()->DrawRectangle({ 1034, 570 + 1+7, 195, 6 }, 0, 0, 0, 255, true, false);

	// Music volume filled portion
	Engine::GetInstance().render.get()->DrawRectangle({ 1034, 511-7, musicPosX - 1034, 6 }, 255, 255, 255, 255, true, false);
	//Ambient sounds filled portion
	Engine::GetInstance().render.get()->DrawRectangle({ 1034, 570 + 1+7, ambient_soundsPosX - 1034, 6 }, 255, 255, 255, 255, true, false);
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
	case 1: //Menu pause: Resume
		showPauseMenu = false;

		player->ResumeMovement();
		Engine::GetInstance().entityManager->candleNum = 3;
		Engine::GetInstance().entityManager->feather = 0;
		/*for (Enemy* enemy : enemyList) {
			if (enemy) {
				enemy->visible = true;
				enemy->ResumeMovement();
			}
		}*/

		DisableGuiControlButtons();
		break;
	case 2://Menu pause: Settings
		showSettingsMenu = true;
		break;
	case 3: //Menu pause: Exit
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
		GameOverMenu = false;

		// Restablecer la posición del jugador según el nivel
		if (player != nullptr) {
			if (Engine::GetInstance().scene.get()->level == 0) {
				player->position.setX(784);
				player->position.setY(600);
			}
			else if (Engine::GetInstance().scene.get()->level == 1) {
				player->position.setX(300);
				player->position.setY(600);
			}

			player->ResumeMovement();
		}

		//player->Start(); // Reiniciar cualquier estado del jugador

		// Reiniciar recursos del jugador
		Engine::GetInstance().entityManager->candleNum = 3;
		Engine::GetInstance().entityManager->feather = 0;

		guiBt->state = GuiControlState::DISABLED;
		guiBt1->state = GuiControlState::DISABLED;

		Change_level(level);
		break;
	case 7:// Game Over: Exit

		exit(0);
		guiBt->state = GuiControlState::DISABLED;
		guiBt1->state = GuiControlState::DISABLED;
		break;

	case 8:// Initial Screen: New Game
		showPauseMenu = false;
		player->ResumeMovement();

		InitialScreenMenu = false;
		guiBt0->state = GuiControlState::DISABLED;
		DisableGuiControlButtons();
		CreateEnemies(level);
		CreateItems(level);

		break;
	case 9:// Initial Screen: Conitnue

		break;
	case 10:// Initial Screen: Settings

		//InitialScreenMenu = false;
		guiBt0->state = GuiControlState::DISABLED;
		DisableGuiControlButtons();
		showSettingsMenu = true;

		break;
	case 11:// Initial Screen: Exit
		InitialScreenMenu = false;
		exit(0);
		guiBt0->state = GuiControlState::DISABLED;
		DisableGuiControlButtons();
		break;
	}

	return true;
}


void Scene::FillWaxy() {

	currentWaxAnim->playReverse = false;
	switch (waxState)
	{
	case EMPTY:
		waxState = FILL_TO_LOW;
	case FILL_TO_LOW:

		if (currentWaxAnim == &fill2lvl1 && fill2lvl1.HasFinished()) {
			waxState = QUARTER_FULL;
			filledWaxy = true;
		}
		break;
	case QUARTER_FULL:
		waxState = FILL_TO_HALF;
	case FILL_TO_HALF:
		if (fill2lvl2.HasFinished()) {
			waxState = HALF_FULL;
			filledWaxy = true;
		}
		break;
	case HALF_FULL:
		waxState = FILL_TO_HIGH;
	case FILL_TO_HIGH:

		if (fill2lvl3.HasFinished()) {
			waxState = ALMOST_FULL;
			filledWaxy = true;
		}
		break;
	case ALMOST_FULL:
		waxState = FILL_TO_FULL;
	case FILL_TO_FULL:
		if (fill2full.HasFinished()) {
			waxState = FULL;
			filledWaxy = true;
		}
		break;
	case FULL:

		/*if (resetWax.ReadSec() >= 1.0f) {*/
		waxState = EMPTY;
		Engine::GetInstance().entityManager.get()->candleNum++;
		filledWaxy = true;

		break;
	}
}

void Scene::DrainWaxy() {

	currentWaxAnim->playReverse = true;

	switch (waxState) {
	case FULL:
		waxState = FILL_TO_FULL;
	case FILL_TO_FULL:
		if (fill2full.HasFinished()) {
			waxState = ALMOST_FULL;
			drainedWaxy = true;
		}
		break;
	case ALMOST_FULL:
		waxState = FILL_TO_HIGH;
	case FILL_TO_HIGH:
		if (fill2lvl3.HasFinished()) {
			waxState = HALF_FULL;
			drainedWaxy = true;
		}
		break;
	case FILL_TO_HALF:
		waxState = FILL_TO_HALF;
	case HALF_FULL:

		if (fill2lvl2.HasFinished()) {
			waxState = QUARTER_FULL;
			drainedWaxy = true;
		}
		break;
	case QUARTER_FULL:
		waxState = FILL_TO_LOW;
	case FILL_TO_LOW:
		if (fill2lvl1.HasFinished()) {
			waxState = EMPTY;

			drainedWaxy = true;
		}
		break;

	case EMPTY:
		if (resetWax.ReadSec() >= 1.0f) {
			Engine::GetInstance().entityManager.get()->candleNum--;
			waxState = FULL;
			drainedWaxy = true;
		}
		break;
	}
}


std::string Scene::GetCurrentLevelName() {
	return ("lvl" + std::to_string(level));
}
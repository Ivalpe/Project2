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


	//L08 Create a new item using the entity manager and set the position to (200, 672) to test
	for (pugi::xml_node itemNode = configParameters.child("entities").child("items").child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		for (int i = 0; i < 4; i++)
		{
			Item* item = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
			item->SetParameters(itemNode);
			item->name = "wax";
			//item->position = Vector2D(200 + (300 * i), 700);
			itemList.push_back(item);

		}

	}

	for (pugi::xml_node itemNode = configParameters.child("entities").child("items").child("feather_item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{

		for (int i = 0; i < 3; i++)
		{
			Item* item = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
			item->SetParameters(itemNode);
			item->name = "feather";

			//item->position = Vector2D(800 + (100 * i), 700);
			itemList.push_back(item);

		}

	}

	for (pugi::xml_node InteractiveObjectNode = configParameters.child("entities").child("interactiveObject").child("stalactites_item"); InteractiveObjectNode; InteractiveObjectNode = InteractiveObjectNode.next_sibling("interactiveObject"))
	{

		InteractiveObject* interactiveObject = (InteractiveObject*)Engine::GetInstance().entityManager->CreateEntity(EntityType::INTERACTIVEOBJECT);
		interactiveObject->SetParameters(InteractiveObjectNode);
		//interactiveObject->position = Vector2D(2500, 1500);
		interactiveObjectList.push_back(interactiveObject);
		interactiveObject->name = "stalactites";
	}

	for (pugi::xml_node InteractiveObjectNode = configParameters.child("entities").child("interactiveObject").child("blocked_wall"); InteractiveObjectNode; InteractiveObjectNode = InteractiveObjectNode.next_sibling("interactiveObject"))

	{

		InteractiveObject* interactiveObject = (InteractiveObject*)Engine::GetInstance().entityManager->CreateEntity(EntityType::INTERACTIVEOBJECT);
		interactiveObject->SetParameters(InteractiveObjectNode);
		//item->position = Vector2D(4840, 2761);
		//interactiveObject->position = Vector2D(1500, 2000);
		interactiveObjectList.push_back(interactiveObject);
		interactiveObject->name = "wall";

	}

	 //Create a enemy using the entity manager 
	/*for (pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
	{
		Enemy* enemy = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY);
		enemy->SetParameters(enemyNode);
		enemyList.push_back(enemy);
	}*/

	for (pugi::xml_node instanceNode = configParameters.child("entities").child("enemies").child("instances").child(GetCurrentLevelName().c_str()).child("instance"); instanceNode; instanceNode = instanceNode.next_sibling("instance")) {
		Enemy* enemy = (Soldier*)Engine::GetInstance().entityManager->CreateEntity((EntityType)instanceNode.attribute("entityType").as_int());
		pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child(instanceNode.attribute("enemyType").as_string());
		enemy->SetParameters(enemyNode);
		enemy->SetPlayer(player);
		enemy->SetInstanceParameters(instanceNode);
		enemy->SetPath(instanceNode);
		enemyList.push_back(enemy);
	}

	CreateItems(level);

	return ret;
}

void Scene::CreateItems(int level) 

{
	/*int currentLvl = level;*/
	LOG("Current Level: %d", level);
	int WaxIndex = 0;
	int fatherIndex = 0;
	std::vector<Vector2D> waxPositions{
		  Vector2D(300, 672),
		  Vector2D(400, 672),
		  Vector2D(500, 672)
	};
	std::vector<Vector2D> factherPositions{
		  Vector2D(900, 400),
		  Vector2D(110, 400),
		  Vector2D(130, 400)
	};

	if (level == 0) {

		for (auto& it : itemList) {
			if (it->name == "wax" && WaxIndex < waxPositions.size()) {
				it->position = waxPositions[WaxIndex++];
			}
			if (it->pbody != nullptr) {
				it->pbody->body->SetTransform(
					b2Vec2(PIXEL_TO_METERS(it->position.getX() + it->texW / 2),
						PIXEL_TO_METERS(it->position.getY() + it->texH / 2)),
					0);
			}
		}

		for (auto& it : itemList) {
			if (it->name == "feather" && fatherIndex < factherPositions.size()) {
				it->position = factherPositions[fatherIndex++];
			}
			if (it->pbody != nullptr) {
				it->pbody->body->SetTransform(
					b2Vec2(PIXEL_TO_METERS(it->position.getX() + it->texW / 2),
						PIXEL_TO_METERS(it->position.getY() + it->texH / 2)),
					0);
			}
		}

		for (auto& it : interactiveObjectList) 
		{
			if (it->name == "wall") {
				it->position = Vector2D(2500, 1500);

			}
			else {
				it->position = Vector2D(1500, 200);
			}

			if (it->pbody != nullptr) {
				it->pbody->body->SetTransform(
					b2Vec2(PIXEL_TO_METERS(it->position.getX() + it->texW / 2),
						PIXEL_TO_METERS(it->position.getY() + it->texH / 2)),
					0);
			}
		}
	}
	else if(level==1){

		for (auto& it : itemList) {
			if (it->name == "wax" && WaxIndex < waxPositions.size()) {
				it->position = Vector2D(-10000, -10000);

			}
			if (it->name == "feather" && fatherIndex < factherPositions.size()) {
				it->position = Vector2D(-10000, -10000);
			}

			if (it->pbody != nullptr) {
				it->pbody->body->SetTransform(
					b2Vec2(PIXEL_TO_METERS(it->position.getX() + it->texW / 2),
						PIXEL_TO_METERS(it->position.getY() + it->texH / 2)),
					0);
			}
		}

		for (auto& it : interactiveObjectList) {
			it->position = Vector2D(-10000, -10000);

			if (it->pbody != nullptr) {
				it->pbody->body->SetTransform(
					b2Vec2(PIXEL_TO_METERS(it->position.getX() + it->texW / 2),
						PIXEL_TO_METERS(it->position.getY() + it->texH / 2)),
					0);
			}
		}
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
	Menu_Pause = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("Menu_Pause").attribute("path").as_string());
	Menu_Settings = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("Menu_Settings").attribute("path").as_string());
	GameOver = Engine::GetInstance().textures.get()->Load(configParameters.child("textures").child("GameOver").attribute("path").as_string());
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

	if (level == 0)
	{
		
		Engine::GetInstance().map.get()->CleanUp();
		//Engine::GetInstance().entityManager.get()->RemoveAllEnemies();
		//Engine::GetInstance().entityManager.get()->RemoveAllItems();
		Engine::GetInstance().map->Load(configParameters.child("map").attribute("path").as_string(), configParameters.child("map").attribute("name").as_string());
		CreateItems(level);
		//CreateEnemies();
	}

	if (level == 1)
	{
		Engine::GetInstance().map.get()->CleanUp();
		LOG("Current Level: %d", level);
		//REMOVE // WHEN SECOND STAGE ENEMYS ADDED
		//Engine::GetInstance().entityManager.get()->RemoveAllEnemies();
		//Engine::GetInstance().entityManager.get()->RemoveAllItems();
		Engine::GetInstance().map->Load(configParameters.child("map1").attribute("path").as_string(), configParameters.child("map1").attribute("name").as_string());

		CreateItems(level);


		showBlackTransition = true;
		blackTransitionStart = SDL_GetTicks();
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
	if(camX< -11520)camX = -11520;
	
	Engine::GetInstance().render.get()->camera.x = (camX);
	Engine::GetInstance().render.get()->camera.y = (camY /*+ player->crouch*/);
	
	//Reset levels
	if (reset_level) {
		Change_level(level);
		if (level == 0) player->SetPosition(Vector2D{ 40,70 });

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

	if(showSettingsMenu) 	MenuSettings();

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


	show_UI();

	if (Engine::GetInstance().scene.get()->showPauseMenu == false && Engine::GetInstance().scene.get()->showSettingsMenu == false && Engine::GetInstance().scene.get()->GameOverMenu == false) Engine::GetInstance().map.get()->DrawFront();

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
	if (!showPauseMenu && !showSettingsMenu && !GameOverMenu && !InitialScreenMenu&& UI) {

		//Crea
		
		Engine::GetInstance().render.get()->DrawTexture(waxTexture, 10, 10, &currentWaxAnim->GetCurrentFrame(), false);

		//vela
		for (int i = 0; i < Engine::GetInstance().entityManager.get()->candleNum; i++) {
			Engine::GetInstance().render.get()->DrawTexture(candle, 150 + (i * 40), 50, nullptr, false);
		}

		//Wax texture
		Engine::GetInstance().render.get()->DrawTexture(FeatherTexture, 10, 150, nullptr, false);
		char FeatherText[64];
		sprintf_s(FeatherText, " x%d", Engine::GetInstance().entityManager->feather);
		Engine::GetInstance().render.get()->DrawText(FeatherText, 50, 155, 40, 30);
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

		TTF_SizeText(Engine::GetInstance().render.get()->font, "New Game", &textWidthNewGame, &textHeightNewGame);
		TTF_SizeText(Engine::GetInstance().render.get()->font, "Continue", &textWidthContinue, &textHeightContinue);
		TTF_SizeText(Engine::GetInstance().render.get()->font, "Settings", &textWidthSettings, &textHeightSettings);
		TTF_SizeText(Engine::GetInstance().render.get()->font, "Exit", &textWidthExit, &textHeightExit);

		float scaleFactor = 0.8f;

		SDL_Rect NewGameButton = {300, 475 - 15, static_cast<int>(textWidthNewGame* scaleFactor), static_cast<int>(textHeightNewGame * scaleFactor) };
		SDL_Rect ConitnuesButton = { 320, 520 - 15, static_cast<int>(textWidthContinue * scaleFactor), static_cast<int>(textHeightContinue * scaleFactor) };
		SDL_Rect Settings = { 330, 595 - 10, static_cast<int>(textWidthSettings * scaleFactor), static_cast<int>(textHeightSettings * scaleFactor) };
		SDL_Rect Exit = {350, 670 - 5, static_cast<int>(textWidthExit * scaleFactor), static_cast<int>(textHeightExit * scaleFactor) };


		guiBt0 = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 8, "New Game", NewGameButton, this));
		guiBt = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 9, "Continue", ConitnuesButton, this));
		guiBt1 = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 10, "Settings", Settings, this));
		guiBt2 = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 11, "Exit", Exit, this));
	}
	
}

void Scene::GameOver_State()
{
	if (Engine::GetInstance().entityManager->candleNum <= 0) {

		if (!GameOverMenu) {
			GameOverMenu = true;
		}

		if (Engine::GetInstance().guiManager != nullptr)	Engine::GetInstance().guiManager->CleanUp();

		int cameraX = Engine::GetInstance().render.get()->camera.x;
		int cameraY = Engine::GetInstance().render.get()->camera.y;

		Engine::GetInstance().render.get()->DrawTexture(GameOver, -cameraX, -cameraY);

		int textWidthContinue, textHeightContinue;
		int textWidthExit, textHeightExit;
		int textWidthSentence, textHeightSentence;

		TTF_SizeText(Engine::GetInstance().render.get()->font, "Continue", &textWidthContinue, &textHeightContinue);
		TTF_SizeText(Engine::GetInstance().render.get()->font, "Exit", &textWidthExit, &textHeightExit);
		TTF_SizeText(Engine::GetInstance().render.get()->font, "Ikaros, don't seek the strength int the light, seek it in the shades", &textWidthSentence, &textHeightSentence);

		SDL_Rect Continue = { 865, 760, textWidthContinue + 20, textHeightContinue + 10 };
		SDL_Rect Exit = { 940, 860, textWidthExit + 20, textHeightExit + 10 };
		SDL_Rect Sentence = { 260 - 85, 600, textWidthSentence + 20, textHeightSentence + 10 };

		guiBt = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 6, "Continue", Continue, this));
		guiBt1 = static_cast<GuiControlButton*>(Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 7, "Exit", Exit, this));

		Engine::GetInstance().render.get()->DrawText("Ikaros, don't seek the strength int the light, seek it in the shades", Sentence.x, Sentence.y, Sentence.w, Sentence.h);

	}





}

void Scene::Active_MenuPause() {

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		showPauseMenu = !showPauseMenu;
		showSettingsMenu = false;
		if (showPauseMenu) {
			player->StopMovement();
			/*	for (Enemy* enemy : enemyList) {
					if (enemy!=NULL) {
						enemy->visible = false;
						enemy->StopMovement();
					}
				}*/
		}
		else if (!showPauseMenu) {
			player->ResumeMovement();
			/*for (Enemy* enemy : enemyList) {
				if (enemy) {
					enemy->visible = true;
					enemy->ResumeMovement();
				}
			}*/
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

	SDL_Rect backgroundRect = { 0, 0, 1920, 1080 };
	SDL_SetRenderDrawColor(Engine::GetInstance().render.get()->renderer, 10, 10, 50, 200);
	SDL_RenderFillRect(Engine::GetInstance().render.get()->renderer, &backgroundRect);

	Engine::GetInstance().render.get()->DrawTexture(Menu_Pause, -cameraX, -cameraY);


	int textWidthContinue, textHeightContinue;
	int textWidthSettings, textHeightSettings;
	int textWidthExit, textHeightExit;

	TTF_SizeText(Engine::GetInstance().render.get()->font, "Continue", &textWidthContinue, &textHeightContinue);
	TTF_SizeText(Engine::GetInstance().render.get()->font, "Settings", &textWidthSettings, &textHeightSettings);
	TTF_SizeText(Engine::GetInstance().render.get()->font, "Exit", &textWidthExit, &textHeightExit);


	SDL_Rect ConitnuesButton = { 862 - 15, 520 - 15, textWidthContinue + 20, textHeightContinue + 10 };
	SDL_Rect Settings = { 882 - 15, 595 - 10, textWidthSettings + 20, textHeightSettings + 10 };
	SDL_Rect Exit = { 919 - 15, 670 - 5, textWidthExit + 20, textHeightExit + 10 };


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

	float scaleFactor = 0.5f; // Reducir tama�o al 50%

	int textWidthSettingsTitle, textHeightSettingsTitle;
	int textWidthMusicVolume, textHeightMusicVolume;
	int textWidthAmbientSounds, textHeightAmbientSounds;
	int textWidthLanguage, textHeightLanguage;
	int textWidthEnglish, textHeightEnglish;

	TTF_SizeText(Engine::GetInstance().render.get()->font, "Settings", &textWidthSettingsTitle, &textHeightSettingsTitle);
	TTF_SizeText(Engine::GetInstance().render.get()->font, "Music Volume", &textWidthMusicVolume, &textHeightMusicVolume);
	TTF_SizeText(Engine::GetInstance().render.get()->font, "Ambient Sounds", &textWidthAmbientSounds, &textHeightAmbientSounds);
	TTF_SizeText(Engine::GetInstance().render.get()->font, "Language", &textWidthLanguage, &textHeightLanguage);
	TTF_SizeText(Engine::GetInstance().render.get()->font, "English", &textWidthEnglish, &textHeightEnglish);


	SDL_Rect SettingsTitle = { 860 - 15, 325 - 15, textWidthSettingsTitle + 20, textHeightSettingsTitle + 10 };
	SDL_Rect MusicVolume = { 700 - 15, 485 - 15, static_cast<int>(textWidthMusicVolume * scaleFactor), static_cast<int>(textHeightMusicVolume * scaleFactor) };
	SDL_Rect AmbientSounds = { 700 - 15, 555 - 15, static_cast<int>(textWidthAmbientSounds * scaleFactor), static_cast<int>(textHeightAmbientSounds * scaleFactor) };
	SDL_Rect Language = { 700 - 15, 630 - 15, static_cast<int>(textWidthLanguage * scaleFactor), static_cast<int>(textHeightLanguage * scaleFactor) };
	SDL_Rect English = { 1085, 625, static_cast<int>(textWidthEnglish * scaleFactor), static_cast<int>(textHeightEnglish * scaleFactor) };


	Engine::GetInstance().render.get()->DrawText("Settings", SettingsTitle.x, SettingsTitle.y, SettingsTitle.w, SettingsTitle.h);
	Engine::GetInstance().render.get()->DrawText("Music Volume", MusicVolume.x, MusicVolume.y, MusicVolume.w, MusicVolume.h);
	Engine::GetInstance().render.get()->DrawText("Ambient Sounds", AmbientSounds.x, AmbientSounds.y, AmbientSounds.w, AmbientSounds.h);
	Engine::GetInstance().render.get()->DrawText("Language", Language.x, Language.y, Language.w, Language.h);
	Engine::GetInstance().render.get()->DrawText("English", English.x, English.y, English.w, English.h);


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

	SDL_Rect newMusicPos = { musicPosX, 511 - 5, 6, 15 }; // New music volume position 
	guiBt->bounds = newMusicPos;

	SDL_Rect newFxPos = { ambient_soundsPosX, 571 - 5, 6, 15 }; // New music ambient sounds position
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
	case 1: //Menu pause: Resume
		showPauseMenu = false;

		player->ResumeMovement();
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
		CreateItems(level);
		GameOverMenu = false;
		guiBt->state = GuiControlState::DISABLED;
		guiBt1->state = GuiControlState::DISABLED;
		Engine::GetInstance().scene.get()->reset_level = true;

		Engine::GetInstance().entityManager->candleNum = 3;
		Engine::GetInstance().entityManager->feather = 0;
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


void Scene::FillWaxy(){

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
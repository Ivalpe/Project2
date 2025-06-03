#pragma once

#include "Module.h"
#include "Player.h"
#include "Enemy.h"
#include "Boss.h"
#include "Item.h"
#include "InteractiveObject.h"
#include <vector>
#include "GuiControlButton.h"
#include "Animation.h"
#include "Platform.h"
#include "Column.h"

struct SDL_Texture;
enum WaxState {
	EMPTY,
	FILL_TO_LOW,
	QUARTER_FULL,
	FILL_TO_HALF,
	HALF_FULL,
	FILL_TO_HIGH,
	ALMOST_FULL,
	FILL_TO_FULL,
	FULL
};



class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Return the player position
	Vector2D GetPlayerPosition();

	void Change_level(int level);

	Player* GetPlayer() const { return player; }

	void CreateItems(int level);
	void CreateEnemies(int level);


	bool OnGuiMouseClickEvent(GuiControl* control);
	void DisableGuiControlButtons();
	
	//UI items
	void show_UI();
	void animationWaxy();

	//Menus
	void MenuSettings();
	void MenuPause();
	void Active_MenuPause();
	void GameOver_State();
	void MenuInitialScreen();

	void FillWaxy();
	void DrainWaxy();

	std::string GetCurrentLevelName();



public:
	


	// Get tilePosDebug value
	std::string GetTilePosDebug() {
		return tilePosDebug;

	}


	int level = 0;
	bool reset_level = false;
	float cameraY = 0.0f;

	bool showPauseMenu = false;
	bool showSettingsMenu = false;
	bool GameOverMenu = false;
	bool InitialScreenMenu = true;


	SDL_Texture* Menu_Pause = NULL;
	SDL_Texture* Menu_Pause_Back = NULL;
	SDL_Texture* Menu_Settings = NULL;
	SDL_Texture* Menu_Settings_Back = NULL;
	SDL_Texture* GameOver = NULL;
	SDL_Texture* GameOver_Back = NULL;
	SDL_Texture* InitialScreen = NULL;
	SDL_Texture* Feather = NULL;
	SDL_Texture* Feather_UI = NULL;
	SDL_Texture* Wax = NULL;
	SDL_Texture* candle = NULL;
	SDL_Texture* candleBase = NULL;

	GuiControlButton* guiBt = nullptr;
	GuiControlButton* guiBt1 = nullptr;
	GuiControlButton* guiBt2 = nullptr;
	GuiControlButton* guiBt0 = nullptr;


	//WAXY
	bool filledWaxy = true, drainedWaxy = true, shouldFillWaxy = false;
	Timer resetWax;

	//Sounds
	bool musicButtonHeld = false;
	bool Ambient_Sounds_ButtonHeld = false;
	bool mouseOverMusicControl = false;
	bool mouseOverAmbientControl = false;
	bool isMusicPlaying = false;

	int sdlVolume;
	int musicPosX = 1225;
	int ambient_soundsPosX = 1225;

private:
	SDL_Texture* mouseTileTex = nullptr;
	std::string tilePosDebug = "[0,0]";
	bool once = false;
	Player* player;
	Enemy* enemy;
	Platform* platform;
	InteractiveObject* interactiveObject;

	std::vector<Enemy*> enemyList;
	std::vector<Boss*> bossList;
	std::vector<Item*> itemList;
	std::vector<Column*> columnList;
	std::vector<InteractiveObject*> interactiveObjectList;
	std::vector<Platform*> platformList;

	SDL_Texture* FeatherTexture;
	SDL_Texture* waxTexture;
	SDL_Texture* glovesTexture;
	SDL_Texture* clothTexture;

	Vector2D mousePos;

	bool UI = false;
	bool UI_pause = false;
	int current_time;
	int current_time_pause;

	SDL_Texture* MoonTexture = nullptr;
	Animation* currentAnimation = nullptr;
	Animation idle;
	Vector2D MoonPos;



	SDL_Texture* WaxiFloatingTexture = nullptr;
	Animation* WaxiFloating_currentAnimation = nullptr;
	Animation WaxiFloating_idle;
	Vector2D WaxiFloatingPos;

	//waxy
	WaxState waxState = EMPTY;


	SDL_Texture* WaxTex = nullptr;
	Animation* currentWaxAnim = nullptr;
	Animation empty, fill2lvl1, staticlvl1, fill2lvl2, staticlvl2, fill2lvl3, staticlvl3, fill2full, full;


	bool showBlackTransition = false;
	float blackTransitionStart;
	int blackTransitionDuration = 4000;
};
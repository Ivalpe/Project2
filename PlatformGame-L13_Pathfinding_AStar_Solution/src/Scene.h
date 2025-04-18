#pragma once

#include "Module.h"
#include "Player.h"
#include "Enemy.h"
#include "Item.h"
#include "InteractiveObject.h"
#include <vector>
#include "GuiControlButton.h"
#include "Animation.h"

struct SDL_Texture;


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

	void CreateItems();


	bool OnGuiMouseClickEvent(GuiControl* control);
	void DisableGuiControlButtons();
	
	//UI items
	void items_UI();
	void show_UI();
	//Menus
	void MenuSettings();
	void MenuPause();
	void Active_MenuPause();
	void GameOver_State();

	std::string GetCurrentLevelName();


public:
	// Get tilePosDebug value
	std::string GetTilePosDebug() {
		return tilePosDebug;

	}


	int level = 0;
	bool reset_level = false;

	bool showPauseMenu = false;
	bool showSettingsMenu = false;
	bool GameOverMenu = false;


	SDL_Texture* Menu_Pause = NULL;
	SDL_Texture* Menu_Settings = NULL;
	SDL_Texture* GameOver = NULL;
	SDL_Texture* Feather = NULL;
	SDL_Texture* Feather_UI = NULL;
	SDL_Texture* Wax = NULL;

	GuiControlButton* guiBt = nullptr;
	GuiControlButton* guiBt1 = nullptr;
	GuiControlButton* guiBt2 = nullptr;


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
	std::vector<Enemy*> enemyList;
	std::vector<Item*> itemList;

	SDL_Texture* FeatherTexture;
	SDL_Texture* waxTexture;

	Vector2D mousePos;

	bool UI = false;
	bool UI_pause = false;
	int current_time;
	int current_time_pause;

	SDL_Texture* MoonTexture = nullptr;
	Animation* currentAnimation = nullptr;
	Animation idle;
	Vector2D MoonPos;

	bool showBlackTransition = false;
	float blackTransitionStart;
	int blackTransitionDuration = 4000;
};
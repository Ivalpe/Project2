#pragma once

#include "GuiControl.h"
#include "Vector2D.h"

class GuiControlButton : public GuiControl
{

public:

	GuiControlButton(int id, SDL_Rect bounds, const char* text);
	virtual ~GuiControlButton();

	// Called each loop iteration
	bool Update(float dt);
	bool gamepadFocused;
	bool gamepadPressed;

private:

	bool canClick = true;
	bool drawBasic = false;
	

	SDL_Texture* feather;
};

#pragma once
#include "GuiControlButton.h"
#include "Render.h"
#include "Engine.h"
#include "Audio.h"
#include "Scene.h"
#include "GuiManager.h"
#include "Textures.h"

GuiControlButton::GuiControlButton(int id, SDL_Rect bounds, const char* text) : GuiControl(GuiControlType::BUTTON, id)
{
	this->bounds = bounds;
	this->text = text;

	canClick = true;
	drawBasic = false;
}

GuiControlButton::~GuiControlButton()
{	
}

bool GuiControlButton::Update(float dt)
{

	if (state == GuiControlState::DISABLED)
	{
		return false;
	}

	if (state != GuiControlState::DISABLED)
	{
		
			Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();


		
			//If the position of the mouse if inside the bounds of the button 
			if (mousePos.getX() > bounds.x && mousePos.getX() < bounds.x + bounds.w && mousePos.getY() > bounds.y && mousePos.getY() < bounds.y + bounds.h) {

				state = GuiControlState::FOCUSED;

				if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
					state = GuiControlState::PRESSED;
				}

				if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
					NotifyObserver();
				}
			}
			else {
				state = GuiControlState::NORMAL;
			}
			



			if (id == 4 || id == 5)
			{
				switch (state)
				{
				case GuiControlState::NORMAL:
					Engine::GetInstance().render->DrawRectangle(bounds, 255, 255, 255, 255, true, false);
					break;
				case GuiControlState::FOCUSED:
					Engine::GetInstance().render->DrawRectangle(bounds, 255, 255, 255, 255, true, false);
					break;
				case GuiControlState::PRESSED:
					Engine::GetInstance().render->DrawRectangle(bounds, 255, 255, 255, 255, true, false);
					break;
				}
			}
			else {

				switch (state)
				{
				case GuiControlState::DISABLED:
					Engine::GetInstance().render->DrawRectangle(bounds, 200, 200, 200, 0, true, false);
					break;
				case GuiControlState::NORMAL:
					Engine::GetInstance().render->DrawRectangle(bounds, 0, 0, 255, 0, true, false);
					break;
				case GuiControlState::FOCUSED:

					if (!Engine::GetInstance().scene.get()->pad->enabled) {
						if (id == 1)Engine::GetInstance().render.get()->DrawTexture(Engine::GetInstance().scene.get()->Feather, 788 - 15, 500 , nullptr, false);
						if (id == 2)Engine::GetInstance().render.get()->DrawTexture(Engine::GetInstance().scene.get()->Feather, 788 - 15, 575 , nullptr, false);
						if (id == 3)Engine::GetInstance().render.get()->DrawTexture(Engine::GetInstance().scene.get()->Feather, 788 - 15, 650 , nullptr, false);
						if (id >= 7 && id <= 11 && !Engine::GetInstance().scene.get()->GameOverMenu) {
							Engine::GetInstance().render.get()->DrawTexture(Engine::GetInstance().scene.get()->Feather, 30, bounds.y + bounds.h / 2 - 10, nullptr, false);
						}
					}
					

					Engine::GetInstance().render->DrawRectangle(bounds, 66, 90, 131, 0, true, false);

					break;
				case GuiControlState::PRESSED:


					if (!Engine::GetInstance().scene.get()->pad->enabled) {
						if (id == 1)Engine::GetInstance().render.get()->DrawTexture(Engine::GetInstance().scene.get()->Feather, 788 - 15, 510 , nullptr, false);
						if (id == 2)Engine::GetInstance().render.get()->DrawTexture(Engine::GetInstance().scene.get()->Feather, 788 - 15, 585, nullptr, false);
						if (id == 3)Engine::GetInstance().render.get()->DrawTexture(Engine::GetInstance().scene.get()->Feather, 788 - 15, 660, nullptr, false);


						if (id >= 7 && id <= 11 && !Engine::GetInstance().scene.get()->GameOverMenu && !Engine::GetInstance().scene.get()->pad->enabled) {
							Engine::GetInstance().render.get()->DrawTexture(Engine::GetInstance().scene.get()->Feather, 160, bounds.y + bounds.h / 2 - 5, nullptr, false);
						}
						
					}

					Engine::GetInstance().render->DrawRectangle(bounds, 66, 90, 131, 255, true, false);
					

					break;
				}
			}
			Engine::GetInstance().render->DrawText(text.c_str(), bounds.x, bounds.y, bounds.w, bounds.h);
	}
	return false;
}


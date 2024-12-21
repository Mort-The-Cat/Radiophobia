#ifndef RADIOPHOBIA_MENUS_VISION_ENGINE_GL
#define RADIOPHOBIA_MENUS_VISION_ENGINE_GL

#include "..\OpenGL_Declarations.h"
#include "..\UI_Renderer_Declarations.h"
#include "..\Engine_Loop.h"

void Set_Language_English(UI_Element* Element)
{
	Current_Language_Setting = "English";
	Language_Settings_Update_Flag = !Language_Settings_Update_Flag;
}

void Set_Language_Deutsch(UI_Element* Element)
{
	Current_Language_Setting = "Deutsch";
	Language_Settings_Update_Flag = !Language_Settings_Update_Flag;
}

void Close_Game(UI_Element* Element)
{
	UI_Continue_Looping = false;
	glfwSetWindowShouldClose(Window, 1);
}

void Place_Language_Buttons()
{
	UI_Elements.push_back(new Button_UI_Element(0.6, 0.65, 0.9, 0.9, Set_Language_English, Pull_Texture("Assets/UI/English.png").Texture));
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = true;
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;

	UI_Elements.push_back(new Button_UI_Element(0.2, 0.65, 0.5, 0.9, Set_Language_Deutsch, Pull_Texture("Assets/UI/German.png").Texture));
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = true;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
}

void Open_Pause_Menu(UI_Element* Element) // There doesn't need to be any UI element given to this
{
	Delete_All_UI();

	Cursor_Reset = false;

	UI_Elements.push_back(new UI_Element(-1.0f, -1.0f, 1.0f, 1.0f));
	UI_Elements.back()->Flags[UF_FILL_SCREEN] = true;
	UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;

	UI_Elements.push_back(new Text_UI_Element(-1.0f, -0.9f, 3.0f, -0.5f, "Paused.txt", true, glm::vec4(1.0f), &Font_Georgia, 0.225f, 0.025f));
	UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;
	UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Shadow_Distance *= 0.25f;

	UI_Elements.push_back(new Button_Text_UI_Element(-0.75f, -0.4f, 0.5f, -0.1f, Return_To_Game_Loop, "Resume.txt", true, glm::vec4(1.0f), &Font_Console, 0.1f));
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;

	UI_Elements.push_back(new Button_Text_UI_Element(-0.75f, -0.0f, 0.5f, 0.3f, Open_Settings_Menu, "Settings.txt", true, glm::vec4(1.0f), &Font_Console, 0.1f));
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;

	UI_Elements.push_back(new Button_Text_UI_Element(-0.75f, 0.4f, 0.5f, 0.7f, Close_Game, "Close_Game.txt", true, glm::vec4(1.0f), &Font_Console, 0.1f));
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;

	//

	Place_Language_Buttons();

	UI_Loop();

	Delete_All_UI();
}

void Open_Settings_Menu(UI_Element* Element)
{
	// Delete_All_UI();

	UI_Elements.push_back(new Text_UI_Element(-1.0f, -0.9f, 3.0f, -0.5f, "Paused.txt", true, glm::vec4(1.0f), &Font_Georgia, 0.225f, 0.025f));
	UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;
	UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Shadow_Distance *= 0.25f;

	Place_Language_Buttons();

	UI_Loop();
}

#endif
#ifndef RADIOPHOBIA_MENUS_VISION_ENGINE_GL
#define RADIOPHOBIA_MENUS_VISION_ENGINE_GL

#include "..\OpenGL_Declarations.h"
#include "..\UI_Renderer_Declarations.h"
#include "..\Engine_Loop.h"

#include "Menus_Declarations.h"

#define FADE_TO_COLOUR true
#define FADE_FROM_COLOUR false

#define SHOULD_INCLUDE_FADE_EFFECT true

void Fade_From_Colour(glm::vec4 Colour, float Speed)
{
#if(!SHOULD_INCLUDE_FADE_EFFECT) // If we don't want the fade effect,
	return;						// just don't bother
#endif

	// Speed = 30.0f;

	UI_Elements.push_back(new UI_Element(-1.0f, -1.0f, 1.0f, 1.0f, Texture(), new UI_Fade_Out_Effect_Controller(Speed)));
	UI_Elements.back()->Flags[UF_FILL_SCREEN] = true;
	UI_Elements.back()->Colour = Colour;
}

void Fade_To_Colour(glm::vec4 Colour, float Speed)
{
	UI_Elements.push_back(new UI_Element(-1.0f, -1.0f, 1.0f, 1.0f, Texture(), new UI_Fade_In_Effect_Controller(0.0f, Speed)));
	UI_Elements.back()->Flags[UF_FILL_SCREEN] = true;
	UI_Elements.back()->Colour = Colour;
}

template<const bool To_Colour_Flag = FADE_TO_COLOUR>
void Fade_Colour(glm::vec4 Colour, float Speed)
{
#if(!SHOULD_INCLUDE_FADE_EFFECT) // If we don't want the fade effect,
	return;						// just don't bother
#endif

	// Speed = 30.0f;

	UI_Elements.push_back(new UI_Element(-1.0f, -1.0f, 1.0f, 1.0f, Texture(), new UI_Fade_Effect_Controller(0.0f)));
	UI_Elements.back()->Flags[UF_FILL_SCREEN] = true;
	UI_Elements.back()->Colour = Colour;

	float* Opacity = &reinterpret_cast<UI_Fade_Effect_Controller*>(UI_Elements.back()->Controller)->Opacity;

	if constexpr (!To_Colour_Flag)
		*Opacity = 1.0f;

	Receive_Inputs();

	bool Condition = false;

	do
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0u);
		
		glClear(GL_COLOR_BUFFER_BIT);

		Handle_Tick();

		if constexpr(To_Colour_Flag)
		{
			*Opacity += Tick * Speed;
			Condition = *Opacity < 1.0f;
		}
		else
		{
			*Opacity -= Tick * Speed;
			Condition = *Opacity > 0.0f;
		}

		Handle_UI();

		Audio::Handle_Audio(Player_Camera);

		Handle_Deletions();

		End_Of_Frame();
	} while (Condition);
}

void Set_Difficulty_Easy(UI_Element* Element)
{
	Selected_Difficulty = Difficulty::Easy;
}

const float Max_Master_Volume = 1.3f;

void Master_Volume_Up(UI_Element* Element) { Sound_Effect_Volume = std::min(1.3f, Sound_Effect_Volume + 0.2f); }
void Master_Volume_Down(UI_Element* Element) { Sound_Effect_Volume = std::max(0.1f, Sound_Effect_Volume - 0.2f); }

void Music_Volume_Up(UI_Element* Element) { Music_Volume = std::min(1.0f, Music_Volume + 0.1f); }
void Music_Volume_Down(UI_Element* Element) { Music_Volume = std::max(0.1f, Music_Volume - 0.1f); }

bool Is_Difficulty_Easy() { return Selected_Difficulty == Difficulty::Easy; }
bool Is_Difficulty_Medium() { return Selected_Difficulty == Difficulty::Medium; }
bool Is_Difficulty_Hard() { return Selected_Difficulty == Difficulty::Hard; }

void Set_Difficulty_Medium(UI_Element* Element)
{
	Selected_Difficulty = Difficulty::Medium;
}

void Set_Difficulty_Hard(UI_Element* Element)
{
	Selected_Difficulty = Difficulty::Hard;
}

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

void Set_Using_Controller(UI_Element* Element)
{
	Using_Controller = true;
}

void Close_Game(UI_Element* Element)
{
	UI_Continue_Looping = false;
	glfwSetWindowShouldClose(Window, 1);
}

void Place_Language_Buttons(glm::vec4 Colour = glm::vec4(1.0f))
{
	UI_Elements.push_back(new Button_UI_Element(0.6, 0.65, 0.9, 0.9, Set_Language_English, Pull_Texture("Assets/UI/English.png").Texture));
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = true;
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Colour = Colour;

	UI_Elements.push_back(new Button_UI_Element(0.2, 0.65, 0.5, 0.9, Set_Language_Deutsch, Pull_Texture("Assets/UI/German.png").Texture));
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = true;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Colour = Colour;

	/*UI_Elements.push_back(new Button_UI_Element(-0.2f, 0.65f, 0.1f, 0.9f, Set_Using_Controller, Pull_Texture("Assets/UI/Controller.png").Texture));
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = true;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Colour = Colour;*/
}

void Open_Pause_Menu(UI_Element* Element);

void Open_Pause_Menu_Close_Game_Prompt(UI_Element* Element)
{
	Fade_Colour(glm::vec4(0.0f), 10.0f);

	Delete_All_UI();

	Cursor_Reset = false;

	UI_Elements.push_back(new UI_Element(-1.0f, -1.0f, 1.0f, 1.0f));
	UI_Elements.back()->Flags[UF_FILL_SCREEN] = true;
	UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;

	UI_Elements.push_back(new Text_UI_Element(-1.0f, -0.9f, 3.0f, -0.5f, "Close_Game_Prompt.txt", true, glm::vec4(1.0f), &Font_Georgia, 0.225f, 0.025f));
	UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;
	UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Shadow_Distance *= 0.25f;

	//

	UI_Elements.push_back(new Button_Text_UI_Element(-0.75f, -0.4f, 0.5f, -0.1f, Close_Game, "Close_Game.txt", true, glm::vec4(1.0f), &Font_Console, 0.1f));
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;

	Place_Language_Buttons();

	UI_Elements.push_back(new Button_Text_UI_Element(-0.75f, 0.0f, 0.5f, 0.3f, Open_Pause_Menu, "Back.txt", true, glm::vec4(1.0f), &Font_Console, 0.1f));
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;

	Fade_From_Colour(glm::vec4(0.0f), 10.0f);

	UI_Loop();
}

void Open_Pause_Menu(UI_Element* Element) // There doesn't need to be any UI element given to this
{
	Fade_Colour(glm::vec4(0.0f), 10.0f);

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

	UI_Elements.push_back(new Button_Text_UI_Element(-0.75f, 0.4f, 0.5f, 0.7f, Open_Pause_Menu_Close_Game_Prompt, "Close_Game.txt", true, glm::vec4(1.0f), &Font_Console, 0.1f));
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;

	//

	Place_Language_Buttons();

	Fade_From_Colour(glm::vec4(0.0f), 10.0f);

	UI_Loop();

	Delete_All_UI();
}

void Open_Settings_Menu(UI_Element* Element);

void Open_Audio_Settings_Menu(UI_Element* Element)
{
	Fade_Colour<FADE_TO_COLOUR>(glm::vec4(0.0f), 10.0f);

	Delete_All_UI();

	UI_Elements.push_back(new UI_Element(-1.0f, -1.0f, 1.0f, 1.0f));
	UI_Elements.back()->Flags[UF_FILL_SCREEN] = true;
	UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;

	UI_Elements.push_back(new Text_UI_Element(-1.0f, -0.9f, 5.0f, -0.5f, "Audio.txt", true, glm::vec4(1.0f), &Font_Georgia, 0.225f, 0.025f));
	UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;
	UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Shadow_Distance *= 0.25f;

	//

	UI_Elements.push_back(new Text_UI_Element(-0.75f, -0.4f, 0.45f, -0.1f, "Master_Volume.txt", true, glm::vec4(1.0f), &Font_Console, 0.1f, 0.025f));
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;

	UI_Elements.push_back(new UI_Element(0.45f, -0.4f, 1.35f, -0.1f));
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;

	UI_Elements.push_back(new UI_Element(0.45f, -0.4f, 1.35f, -0.1f, Pull_Texture("Assets/Textures/White.png").Texture, new UI_Filling_Bar_Controller(0.9f, &Sound_Effect_Volume, &Max_Master_Volume)));
	UI_Elements.back()->Colour = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f);
	UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;

	UI_Elements.push_back(new Button_UI_Element(1.35f, -0.4f, 1.65f, -0.1f, Master_Volume_Down, Pull_Texture("Assets/UI/Audio_Down.png").Texture));
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;

	UI_Elements.push_back(new Button_UI_Element(1.65f, -0.4f, 1.95f, -0.1f, Master_Volume_Up, Pull_Texture("Assets/UI/Audio_Up.png").Texture));
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;

	//

	UI_Elements.push_back(new Text_UI_Element(-0.75f, 0.0f, 0.45f, 0.3f, "Music_Volume.txt", true, glm::vec4(1.0f), &Font_Console, 0.1f, 0.025f));
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;

	UI_Elements.push_back(new UI_Element(0.45f, 0.0f, 1.35f, 0.3f));
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;

	UI_Elements.push_back(new UI_Element(0.45f, 0.0f, 1.35f, 0.3f, Pull_Texture("Assets/Textures/White.png").Texture, new UI_Filling_Bar_Controller(0.9f, &Music_Volume, &One)));
	UI_Elements.back()->Colour = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f);
	UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;

	UI_Elements.push_back(new Button_UI_Element(1.35f, 0.0f, 1.65f, 0.3f, Music_Volume_Down, Pull_Texture("Assets/UI/Audio_Down.png").Texture));
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;

	UI_Elements.push_back(new Button_UI_Element(1.65f, 0.0f, 1.95f, 0.3f, Music_Volume_Up, Pull_Texture("Assets/UI/Audio_Up.png").Texture));
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;

	//

	Place_Language_Buttons();

	UI_Elements.push_back(new Button_Text_UI_Element(-0.75f, 0.4f, 0.725f, 0.7f, Open_Settings_Menu, "Back.txt", true, glm::vec4(1.0f), &Font_Console, 0.1f));
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;

	Fade_From_Colour(glm::vec4(0.0f), 10.0f);

	UI_Loop();
}

void Open_Controls_Menu(UI_Element* Element)
{
	Fade_Colour<FADE_TO_COLOUR>(glm::vec4(0.0f), 10.0f);

	Delete_All_UI();

	UI_Elements.push_back(new UI_Element(-1.0f, -1.0f, 1.0f, 1.0f));
	UI_Elements.back()->Flags[UF_FILL_SCREEN] = true;
	UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;

	UI_Elements.push_back(new Text_UI_Element(-1.0f, -0.9f, 5.0f, -0.5f, "Controls.txt", true, glm::vec4(1.0f), &Font_Georgia, 0.225f, 0.025f));
	UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;
	UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Shadow_Distance *= 0.25f;

	Place_Language_Buttons();

	UI_Elements.push_back(new Button_Text_UI_Element(-0.75f, 0.4f, 0.725f, 0.7f, Open_Settings_Menu, "Back.txt", true, glm::vec4(1.0f), &Font_Console, 0.1f));
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;

	Fade_From_Colour(glm::vec4(0.0f), 10.0f);

	UI_Loop();
}

void Open_Settings_Menu(UI_Element* Element)
{
	Fade_Colour<FADE_TO_COLOUR>(glm::vec4(0.0f), 10.0f); // Fades to black

	Delete_All_UI();

	UI_Elements.push_back(new UI_Element(-1.0f, -1.0f, 1.0f, 1.0f));
	UI_Elements.back()->Flags[UF_FILL_SCREEN] = true;
	UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;

	UI_Elements.push_back(new Text_UI_Element(-1.0f, -0.9f, 3.0f, -0.5f, "Settings.txt", true, glm::vec4(1.0f), &Font_Georgia, 0.225f, 0.025f));
	UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;
	UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Shadow_Distance *= 0.25f;

	//

	/*UI_Elements.push_back(new Text_UI_Element(-0.75f, -0.4f, 0.3f, -0.1f, "Difficulty.txt", true, glm::vec4(1.0f), &Font_Console, 0.1f, 0.025f));
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;

	UI_Elements.push_back(new Button_Text_UI_Element(0.35f, -0.4f, 0.95f, -0.1f, Set_Difficulty_Easy, "Easy.txt", true, glm::vec4(1.0f), &Font_Console, 0.1f));
	UI_Elements.back()->Controller = new UI_Settings_Button(Is_Difficulty_Easy);
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;

	UI_Elements.push_back(new Button_Text_UI_Element(0.95f, -0.4f, 1.55f, -0.1f, Set_Difficulty_Medium, "Medium.txt", true, glm::vec4(1.0f), &Font_Console, 0.1f));
	UI_Elements.back()->Controller = new UI_Settings_Button(Is_Difficulty_Medium);
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;

	UI_Elements.push_back(new Button_Text_UI_Element(1.55f, -0.4f, 2.15f, -0.1f, Set_Difficulty_Hard, "Hard.txt", true, glm::vec4(1.0f), &Font_Console, 0.1f));
	UI_Elements.back()->Controller = new UI_Settings_Button(Is_Difficulty_Hard);
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;*/

	// We only want the difficulty to come up during the game-creation menu

	UI_Elements.push_back(new Button_Text_UI_Element(-0.75f, -0.4f, 0.725f, -0.1f, Open_Audio_Settings_Menu, "Audio.txt", true, glm::vec4(1.0f), &Font_Console, 0.1f, 0.025f));
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;

	//

	UI_Elements.push_back(new Button_Text_UI_Element(-0.75f, 0.0f, 0.725f, 0.3f, Open_Controls_Menu, "Controls.txt", true, glm::vec4(1.0f), &Font_Console, 0.1f, 0.025f));
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;

	//

	UI_Elements.push_back(new Button_Text_UI_Element(-0.75f, 0.4f, 0.725f, 0.7f, Open_Pause_Menu, "Back.txt", true, glm::vec4(1.0f), &Font_Console, 0.1f));
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;

	Place_Language_Buttons();

	Fade_From_Colour(glm::vec4(0.0f), 10.0f);

	// Fade_Colour<FADE_FROM_COLOUR>(glm::vec4(0.0f), 6.0f); // Fades to black

	UI_Loop();
}

#endif
#ifndef VISION_ENGINE_TITLE_SCREEN_HANDLING
#define VISION_ENGINE_TITLE_SCREEN_HANDLING

#include "../Engine_Loop_Includes.h"
#include "../Engine_Loop.h"
#include "../UI_Textbox_Definitions.h"

#include "Menus.h"

#include "Enemy_Declarations.h"

#include "../Light_BVH_Tree_Handler.h"

#include "Interactable_Declarations.h"

#include "../Hitdetection_Blockmap.h"

#include "Level_Init.h"

void Run_Engine_Loop(UI_Element* Element)
{ 
	Delete_All_UI();

	//UI_Elements.push_back(new UI_Element(-1.0f, -1.0f, 1.0f, 1.0f));
	//UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;
	//UI_Elements.back()->Flags[UF_FILL_SCREEN] = true;

	Galaxy_Particles.Delete_All_Particles();

	//

	// Load_Test_Scene_Assets();

	bool Finished_Loading = false;

	Context_Interface::Loading_Progress = 1.0f;
	Context_Interface::Loading_Progress_Total = 1.0f;

	Job_System::Submit_Job(Job_System::Job(Loading_Function, new Loading_Function_Parameters(Load_Test_Scene_Assets, &Finished_Loading)));

	Loading_Screen_Loop(Finished_Loading);

	Delete_All_UI();

	//

	Setup_Intro_Tunnel();

	Delete_All_UI();

	Cursor_Reset = true;

	//Player_Camera.Position = glm::vec3(
	//	-1, -9.2, -4
	//);

	Engine_Loop(); 
}

void Title_Screen_Loop();

void Create_Title_Screen_Page()
{
	Pull_Audio("Assets/Audio/UI/Beep.wav");

	Sound_Engine->play2D(Pull_Audio("Assets/Audio/UI/Beep.wav").Source);

	UI_Elements.push_back(new UI_Element(-1.0f, -1.0f, 1.0f, 1.0f, Pull_Texture("Assets/UI/Title_Screen.png").Texture));
	UI_Elements.back()->Colour = glm::vec4(0.75f, 0.75f, 0.75f, 1.0f);
	UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;
	UI_Elements.back()->Flags[UF_FILL_SCREEN] = true;

	for (float Radius = 0.1f; Radius < 10.0f; Radius += 0.15f)
	{
		float Total = 64 * Radius;

		for (size_t W = 0; W < Total; W++)
			Galaxy_Particles.Particles.Spawn_Particle(Radius + RNG() * 0.3f, (W + RNG()) * 3.14159f * 2.0f / Total);

		for (size_t W = 0; W < 256 * Radius; W++)
			Galaxy_Particles.Particles.Spawn_Particle(Radius + RNG() * 0.1f, 6.28318f + (W + RNG() * 4.0f) * 3.14159f * 2.0f / (64 * Radius));
	}


	UI_Elements.push_back(new UI_Element(-1.0f, -1.0f, 0.4f, 1.0f, Pull_Texture("Assets/UI/UI_Line.png").Texture, nullptr));
	UI_Elements.back()->Colour = glm::vec4(0.8f, 0.8f, 0.8f, 0.6f);
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;

	// UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;

	UI_Elements.push_back(new UI_Element(-0.9f, -0.98f, 2.1f, -0.48f, Pull_Texture("Assets/UI/Radiophobia_Title.png").Texture));
	UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Shadow_Distance *= 0.25f;

	UI_Elements.push_back(new Button_Text_UI_Element(-0.9f, -0.5f, 0.7f, -0.1f, Run_Engine_Loop, "Start_Game.txt" /*"Drücken Sie diesen Knopf, um anzufangen"*/, true, glm::vec4(1.0f), &Font_Console, 0.15f, 0.01f));
	// UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;
	UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Shadow_Distance *= 0.25f;

	UI_Elements.push_back(new Button_Text_UI_Element(-0.9f, 0.3f, 0.7f, 0.7f, Close_Game, "Close_Game.txt" /*"Drücken Sie diesen Knopf, um anzufangen"*/, true, glm::vec4(1.0f), &Font_Console, 0.15f, 0.01f));
	// UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;
	UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;
	UI_Elements.back()->Flags[UF_SHADOW_BACKDROP] = true;
	UI_Elements.back()->Shadow_Distance *= 0.25f;

	Place_Language_Buttons(glm::vec4(1.0f));

	// UI_Elements.push_back(new Textbox_UI_Element(-1.0f, -0.25f, 1.0f, 0.4f, 0.1f, 0.0f, &Font_Console));

	Cursor_Reset = false;

	Player_Camera.Position = glm::vec3(
		2.872747f, -1.759029f, 9.186198f
	);

	Player_Camera.Orientation = glm::vec3(
		616.330139f, -40.649986f, 0.000000f
	);

	Title_Screen_Loop();
}

void Create_Old_Title_Screen_Page()
{
	Pull_Audio("Assets/Audio/UI/Beep.wav");

	Sound_Engine->play2D(Pull_Audio("Assets/Audio/UI/Beep.wav").Source); 
	
	// I just call this function because irrklang's preload doesn't work properly unless a sound has actually been played

	for (float Radius = 0.1f; Radius < 10.0f; Radius += 0.15f)
	{
		float Total = 64 * Radius;

		for (size_t W = 0; W < Total; W++)
			Galaxy_Particles.Particles.Spawn_Particle(Radius + RNG() * 0.3f, (W + RNG()) * 3.14159f * 2.0f / Total);

		for (size_t W = 0; W < 256 * Radius; W++)
			Galaxy_Particles.Particles.Spawn_Particle(Radius + RNG() * 0.1f, 6.28318f + (W + RNG() * 4.0f) * 3.14159f * 2.0f / (64 * Radius));
	}

	UI_Elements.push_back(new UI_Element(-1.0f, -1.0f, 1.0f, 1.0f)); // , Pull_Texture("Assets/UI/Title_Screen.png").Texture));
	UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;
	UI_Elements.back()->Flags[UF_FILL_SCREEN] = true;

	UI_Elements.push_back(new UI_Element(-0.9f, -0.98f, 2.1f, -0.48f, Pull_Texture("Assets/UI/Radiophobia_Title.png").Texture));
	UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;

	UI_Elements.push_back(new Button_Text_UI_Element(-0.9f, 0.5, 1.4f, 0.9f, Run_Engine_Loop, "Test.txt" /*"Drücken Sie diesen Knopf, um anzufangen"*/, true, glm::vec4(1.0f), &Font_Console, 0.1f, 0.01f));
	// UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	// UI_Elements.back()->Flags[UF_FILL_SCREEN] = true;
	
	// UI_Elements.back()->Image = Pull_Texture("Assets/Textures/Floor_Tiles.png").Texture;

	// UI_Elements.back()->Flags[UF_IMAGE] = true;

	//

	Place_Language_Buttons();

	// UI_Elements.push_back(new Textbox_UI_Element(-1.0f, -0.25f, 1.0f, 0.4f, 0.1f, 0.0f, &Font_Console));

	Cursor_Reset = false;

	Player_Camera.Position = glm::vec3(
		2.872747f, -1.759029f, 9.186198f
	);

	Player_Camera.Orientation = glm::vec3(
		616.330139f, -40.649986f, 0.000000f
	);

	//if (Music)
	//	Music->stop();
	//Music = Sound_Engine->play2D(Pull_Audio("Assets/Audio/Music/Intense_Bassloop.wav").Source, true, false, false, true);

	Title_Screen_Loop();
}

void Title_Screen_Loop()
{
	while (!glfwWindowShouldClose(Window))
	{
		double Current_Time = glfwGetTime();
		Tick = Current_Time - Last_Time;
		Last_Time = Current_Time;

		glBindFramebuffer(GL_FRAMEBUFFER, 0u);


		glDepthMask(GL_TRUE);

		glClearColor(0.05f, 0.05f, 0.05f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Receive_Inputs();

		Audio::Handle_Audio(Player_Camera);

		Player_Camera.Set_Projection_Matrix();
		Player_Camera.Set_Audio_Observer();

		{
			Galaxy_Particles.Update();
			
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE); // We'll be using purely additive blending for the galaxy particles
			Galaxy_Particles.Shader.Activate();
			Galaxy_Particles.Render();
		}

		Handle_UI();

		Handle_Deletions();

		End_Of_Frame();
	}
}

#endif

#ifndef ENGINE_LOOP_VISION
#define ENGINE_LOOP_VISION

#include "Engine_Loop_Includes.h"

void Render_All()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	glDisable(GL_BLEND);

	glEnable(GL_LINEAR_MIPMAP_LINEAR);

	//

	Update_Lighting_Buffer();

	Test_Cubemap.Parse_Texture(Scene_Object_Shader, "Cubemap", 0);
	Test_Cubemap.Bind_Texture();

	// Scene_Lights[0]->Position = Player_Camera.Position;

	// Scene_Lights[0]->Direction = Camera_Direction;

	for (size_t W = 0; W < Scene_Models.size(); W++)
	{
		Scene_Models[W]->Render(Scene_Object_Shader);
	}

	Makarov_Pistol.Render_Viewmodel();

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glDepthMask(GL_FALSE);
	Bubble_Particles.Shader.Activate();
	Bubble_Particles.Render();

	glDisable(GL_BLEND);

	if (Shadow_Mapper::Shadow_Mapping)
		Shadow_Mapper::Render_All_Shadows();

	if(Post_Processing)
		Post_Processor::Finish_Rendering();

	// After the post-processing effects are done, these transparent particles can be drawn

	Smoke_Particles.Shader.Activate();

	glEnable(GL_BLEND);

	glDepthMask(GL_FALSE);

	Test_Cubemap.Parse_Texture(Smoke_Particles.Shader, "Cubemap", 0);
	Test_Cubemap.Bind_Texture();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Smoke_Particles.Render();

	//

	Billboard_Smoke_Particles.Shader.Activate();

	Test_Cubemap.Parse_Texture(Billboard_Smoke_Particles.Shader, "Cubemap", 0);
	Test_Cubemap.Bind_Texture();
	Billboard_Smoke_Particles.Render();

	glEnable(GL_DEPTH_CLAMP);
	Volumetric_Cone_Particles.Shader.Activate();
	Post_Processor::Bind_G_Buffer_Textures(Volumetric_Cone_Particles.Shader);
	Bind_Screen_Dimensions(Volumetric_Cone_Particles.Shader);
	Volumetric_Cone_Particles.Render();
	glDisable(GL_DEPTH_CLAMP);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE); // We'll be using purely additive blending for the fire particles
	Billboard_Fire_Particles.Shader.Activate();
	Billboard_Fire_Particles.Render();

	Colour_Bubble_Particles.Shader.Activate();
	Colour_Bubble_Particles.Render();

	//

	// Galaxy_Particles.Shader.Activate();
	// Galaxy_Particles.Render();
}

void End_Of_Frame()
{
	Frame_Counter++;

	Time_Elapsed_Since_FPS_Update += Tick;

	if (Time_Elapsed_Since_FPS_Update > 1.0f)
	{
		glfwSetWindowTitle(Window, (
			std::string("VisionEngineGL | FPS: ") + std::to_string((int)(Frame_Counter / Time_Elapsed_Since_FPS_Update))/* +
			std::string(" | Rendering + Physics resolve: (ms) ") + std::to_string(1000.0f * (glfwGetTime() - Start_Timer)) +
			std::string(" | Begin physics + update audio/particles: (ms) ") + std::to_string(1000.0f * (Start_Timer - Other_Start_Timer)*/
			).c_str());

		printf(" >> FPS: %d\n", (int)(Frame_Counter / Time_Elapsed_Since_FPS_Update));

		Time_Elapsed_Since_FPS_Update = 0;
		Frame_Counter = 0;
	}

	glfwSwapBuffers(Window);
	glfwPollEvents();
}

void Handle_Tick()
{
	double Current_Time = glfwGetTime();
	Tick = (Current_Time - Last_Time);
	Last_Time = Current_Time;

	if (Tick > 0.15f)
	{
		Tick = 0.0f;
	}
}

void Engine_Loop()
{
	Last_Time = glfwGetTime();

	while (!glfwWindowShouldClose(Window))
	{
		Handle_Tick();

		Receive_Inputs();

		Player_Movement();

		Makarov_Pistol.Handle_Viewmodel_States();

		//

		if(Post_Processing)
			Post_Processor::Start_Rendering();
		else
			glBindFramebuffer(GL_FRAMEBUFFER, 0u);

		glDepthMask(GL_TRUE);

		glClearColor(0.2, 0.3, 0.2, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Scene_Lights[0]->Blur = 45 + 45 * sinf(glfwGetTime());

		// double Other_Start_Timer = glfwGetTime();

		Audio::Handle_Audio(Player_Camera);

		Physics::Record_Collisions();
		
		Handle_Scene();

		Smoke_Particles.Update();
		Billboard_Smoke_Particles.Update();
		Billboard_Fire_Particles.Update();
		Volumetric_Cone_Particles.Update();

		Bubble_Particles.Update();
		Colour_Bubble_Particles.Update();

		// Galaxy_Particles.Update();

		Scene_Object_Shader.Activate();

		Player_Camera.Set_Projection_Matrix();
		Player_Camera.Set_Audio_Observer();
		Player_Camera.Bind_Buffers(Camera_Uniform_Location);

		//

		// double Start_Timer = glfwGetTime();

		glDisable(GL_DEPTH_TEST);

		Physics::Resolve_Collisions();

		if(Player_Physics_Object.Velocity.y >= 0.0f)		// If the player is falling/stationary
			Player_Physics_Object.Stabilise_On_Ground();	// Stabilise their position

		Render_All();

		Handle_UI(); // We're able to handle all of the UI whilst the collisions are resolving!

		Handle_Deletions();

		//glDrawArrays(GL_TRIANGLES, 0, 3);

		//

		End_Of_Frame();

		if (Time_Elapsed_Since_FPS_Update == 0)
			for (size_t W = 0; W < 10; W++)
			{
				glm::vec3 Position, Velocity;

				Position = glm::vec3(RNG() - 3.0f, RNG() - 4.4f, RNG() - 3.0f);

				Velocity = glm::vec3(RNG() * 0.01f, RNG() * 0.01f, RNG() * 0.01f);

				Bubble_Particles.Particles.Spawn_Particle(Position, Velocity);
				Colour_Bubble_Particles.Particles.Spawn_Particle(Position, Velocity);

				// Bubble_Particles.Particles.Spawn_Particle(glm::vec3(RNG() - 3.0f, RNG() - 4.0f, RNG() - 3.0f), glm::vec3(RNG() * 0.01f, RNG() * 0.01f, RNG() * 0.01f));
				// Bubble_Colour_Particles.Particles.Spawn_Particle(glm::vec3(RNG() - 3.0f, RNG() - 4.0f, RNG() - 3.0f), glm::vec3(RNG() * 0.01f, RNG() * 0.01f, RNG() * 0.01f));
			}
	}
}

//

struct Loading_Function_Parameters
{
	void (*Function)();
	bool* Finished_Loading_Flag;

	Loading_Function_Parameters(void (*Functionp)(), bool* Finished_Loading_Flagp)
	{
		Function = Functionp;
		Finished_Loading_Flag = Finished_Loading_Flagp;
	}
};

void Loading_Function(void* Data)
{
	Loading_Function_Parameters* Parameters = static_cast<Loading_Function_Parameters*>(Data);

	Parameters->Function(); // Runs the given function

	*Parameters->Finished_Loading_Flag = true; // Once finished, lets engine know that the function is complete

	delete Parameters;
}

void Loading_Screen_Loop(bool& Finished_Loading_Flag)
{
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	UI_Elements.push_back(new UI_Element(-1.0f, -0.25f, 1.0f, 0.25f));

	UI_Elements.push_back(new UI_Element(-1.0f, -0.25f, 1.0f, 0.25f, Pull_Texture("Assets/Textures/White.png").Texture, new UI_Loading_Screen_Icon_Controller(2.0f)));
	UI_Elements.back()->Colour = glm::vec4(0.15f, 0.3f, 1.0f, 0.75f);
	UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;

	UI_Elements.push_back(new Text_UI_Element(-1.0f, -0.25f, 1.0f, 0.25f, "Loading.txt", true, glm::vec3(1.0f, 1.0f, 1.0f), &Font_Console, 0.1f));
	UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;
	UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;
	
	//UI_Elements.back()->Flags[UF_RENDER_CONTENTS]

	Last_Time = glfwGetTime();

	while (!Finished_Loading_Flag && !glfwWindowShouldClose(Window))
	{
		{
			Context_Interface::Request_Context();

			glBindFramebuffer(GL_FRAMEBUFFER, 0u);

			glClearColor(0.05f, 0.05f, 0.05f, 0.1f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Receive_Inputs();

			Context_Interface::Free_Context();
		}

		Audio::Handle_Audio(Player_Camera);

		{
			Context_Interface::Request_Context();

			Handle_UI();

			Handle_Deletions();

			// render the loading screen stuff above it

			// There's no way that we need to handle deletions on the loading screen lol

			End_Of_Frame();

			Context_Interface::Free_Context();
		}
	}

	Context_Interface::Assume_Context(); // Once the loading is finished, we need the main thread to reassume control until later
}

void UI_Loop()
{
	glDisable(GL_CULL_FACE);

	glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0u);

	UI_Continue_Looping = true;

	Last_Time = glfwGetTime();

	while (UI_Continue_Looping && !glfwWindowShouldClose(Window))
	{
		double Current_Time = glfwGetTime();
		Tick = Current_Time - Last_Time;
		Last_Time = Current_Time;

		glBindFramebuffer(GL_FRAMEBUFFER, 0u);

		glClear(GL_COLOR_BUFFER_BIT);

		Receive_Inputs();

		Audio::Handle_Audio(Player_Camera);

		Handle_UI();

		Handle_Deletions();

		End_Of_Frame();
	}
}

#endif
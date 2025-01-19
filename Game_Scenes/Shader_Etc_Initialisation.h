#ifndef VISION_ENGINE_SHADER_ETC_INITIALISATION
#define VISION_ENGINE_SHADER_ETC_INITIALISATION

#include "..\Engine_Loop_Includes.h"
#include "Items_Declarations.h"
#include "Interactable_Declarations.h"

void Init_Player_Physics_Object()
{
	if (Player_Physics_Object.Object == nullptr)
	{
		Player_Physics_Object.Flags[PF_NO_ROTATION] = true;
		Player_Physics_Object.Flags[PF_PLAYER_OBJECT] = true;
		
		Player_Physics_Object.Object = new Model();

		/*Player_Camera.Position = glm::vec3(
			// 2.0f, -4.909266, 10.0f
			3.773928 - 1.0f, -3.6 - 2.0f, -8.415 - 1.0f
		);*/

		Player_Camera.Position = glm::vec3(
			2.2f, -1.23f, 8.24f
		);

		Initialise_Physics_Object(&Player_Physics_Object, Player_Physics_Object.Object);

		Player_Physics_Object.Object->Position = Player_Camera.Position;

		Player_Physics_Object.Friction = 0.99f;

		Player_Physics_Object.Elasticity = 0.0f;

		Player_Physics_Object.Object->Hitboxes = { Generate_AABB_Hitbox(*Pull_Mesh("Assets/Hitboxes/Player_Hitbox.obj").Mesh) };
		Player_Physics_Object.Object->Hitboxes[0]->Object = Player_Physics_Object.Object;
		Player_Physics_Object.Object->Hitboxes[0]->Position = &Player_Physics_Object.Object->Position;

		Scene_Hitboxes.push_back(Player_Physics_Object.Object->Hitboxes[0]);
		std::swap(Scene_Hitboxes[Physics::Scene_Physics_Objects.size() - 1], Scene_Hitboxes.back());
	}
}

void Initialise_Fonts()
{
	Font_Table::Initialise_Font_Texture(&Font_Georgia, 512, "Assets/Font/georgia.ttf");
	Font_Table::Initialise_Font_Texture(&Font_Gothic, 512, "Assets/Font/COPRGTL.ttf");
	Font_Table::Initialise_Font_Texture(&Font_Console, 512, "Assets/Font/lucon.ttf");
}

void Initialise_Particles()
{
	Init_Player_Physics_Object();

	Shader Smoke_Particle_Shader;
	Smoke_Particle_Shader.Create_Shader("Shader_Code/Smoke_Particle.vert", "Shader_Code/Particle.frag", "Shader_Code/Vertex_Test.geom");

	Create_Particle_Renderer(Smoke_Particle_Shader, Pull_Mesh("Assets/Models/Smoke.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Smoke_Noise.png").Texture, Pull_Texture("Black").Texture, &Smoke_Particles);

	Shader Billboard_Particle_Shader;
	Billboard_Particle_Shader.Create_Shader("Shader_Code/Billboard_Smoke_Particle.vert", "Shader_Code/Lightless_Particle.frag", nullptr);

	Create_Particle_Renderer(Billboard_Particle_Shader, Billboard_Vertex_Buffer(-0.05, -0.05, 0.05, 0.05), Pull_Texture("Assets/Textures/Smoke.png").Texture, Pull_Texture("Black").Texture, &Billboard_Smoke_Particles);


	{
		Create_Particle_Renderer(Billboard_Particle_Shader, Billboard_Vertex_Buffer(-0.2, -0.2, 0.2, 0.2), Pull_Texture("Assets/Textures/Smoke.png").Texture, Pull_Texture("Black").Texture, &Vent_Smoke_Particles);
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE); // We'll be using purely additive blending for the fire particles
	Shader Billboard_Fire_Shader;
	Billboard_Fire_Shader.Create_Shader("Shader_Code/Billboard_Fire_Particle.vert", "Shader_Code/Lightless_Particle.frag", nullptr);

	Create_Particle_Renderer(Billboard_Fire_Shader, Billboard_Vertex_Buffer(-0.2, -0.2, 0.2, 0.2), Pull_Texture("Assets/Textures/Fire_2.png").Texture, Pull_Texture("Black").Texture, &Billboard_Fire_Particles);

	Shader Muzzle_Flash_Shader;
	Muzzle_Flash_Shader.Create_Shader("Shader_Code/Billboard_Muzzle_Flash_Particle.vert", "Shader_Code/Lightless_Particle.frag", nullptr);
	Create_Particle_Renderer(Muzzle_Flash_Shader, Billboard_Vertex_Buffer(-0.125f, -0.125f, 0.125f, 0.125f), Pull_Texture("Assets/Textures/Fire_2.png").Texture, Pull_Texture("Black").Texture, &Muzzle_Flash_Particles);

	//

	Shader Billboard_Galaxy_Shader;
	Billboard_Galaxy_Shader.Create_Shader("Shader_Code/Billboard_Galaxy_Particle.vert", "Shader_Code/Lightless_Particle.frag", nullptr); // We really don't need a geometry shader for this one

	Create_Particle_Renderer(Billboard_Galaxy_Shader, Billboard_Vertex_Buffer(-0.2, -0.2, 0.2, 0.2), Pull_Texture("Assets/Textures/Galaxy_Test_2.png").Texture, Pull_Texture("Black").Texture, &Galaxy_Particles);

	//

	Shader Volumetric_Cone_Shader;
	Volumetric_Cone_Shader.Create_Shader("Shader_Code/Volumetric_Cone_Particle.vert", "Shader_Code/Volumetric_Cone_Particles.frag", nullptr);

	Create_Particle_Renderer(Volumetric_Cone_Shader, Pull_Mesh("Assets/Models/Normalised_Cone.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Smoke_Noise.png").Texture, Pull_Texture("Black").Texture, &Volumetric_Cone_Particles);
	Volumetric_Cone_Particles.Bind_Textures = false;

	//

	Shader Bubble_Shader;
	Bubble_Shader.Create_Shader("Shader_Code/Bubble_Particle.vert", "Shader_Code/Bubble_Particle.frag", nullptr);

	Create_Particle_Renderer(Bubble_Shader, Pull_Mesh("Assets/Models/Bubble.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Smoke_Noise.png").Texture, Pull_Texture("Black").Texture, &Bubble_Particles);
	Bubble_Particles.Bind_Textures = false; // We don't need textures for the *bubble* shader

	//

	Shader Colour_Bubble_Shader;
	Colour_Bubble_Shader.Create_Shader("Shader_Code/Bubble_Particle.vert", "Shader_Code/Colour_Bubble_Particle.frag", nullptr);

	Create_Particle_Renderer(Colour_Bubble_Shader, Pull_Mesh("Assets/Models/Bubble.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Smoke_Noise.png").Texture, Pull_Texture("Black").Texture, &Colour_Bubble_Particles);
	Colour_Bubble_Particles.Bind_Textures = false;

	/*for (float Radius = 0.3f; Radius < 10.0f; Radius += 0.15f)
	{
		for (size_t W = 0; W < 64 * Radius; W++)
			Galaxy_Particles.Particles.Spawn_Particle(Radius + RNG() * 0.3f, (W + RNG()) * 3.14159f * 2.0f / (64 * Radius));

		for (size_t W = 0; W < 256 * Radius; W++)
			Galaxy_Particles.Particles.Spawn_Particle(Radius + RNG() * 0.1f, 6.28318f + (W + RNG() * 4.0f) * 3.14159f * 2.0f / (64 * Radius));
	}*/
}

void Initialise_Sound_Effects()
{
	Door_Opening_Sound = Sound_Engine->addSoundSourceFromFile("Assets/Audio/Door/Door_Open.wav", irrklang::ESM_AUTO_DETECT, true);
}

void Initialise_Shaders_And_Particle_Emitters()
{
	Cursor_Reset = true;

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);						// Purely additive blending

	Loading_Screen_Spiral_Shader.Create_Shader("Shader_Code/Loading_Screen_Spiral.vert", "Shader_Code/Loading_Screen_Spiral.frag", nullptr);

	Scene_Object_Shader.Create_Shader("Shader_Code/Vertex_Test.vert", "Shader_Code/Vertex_Test.frag", "Shader_Code/Vertex_Test.geom");
	Scene_Object_Shader.Activate();

	//

	Load_Cubemap(
		{
		"Assets/Cubemap/Test/Cubemap_Left.png",
		"Assets/Cubemap/Test/Cubemap_Right.png",
		"Assets/Cubemap/Test/Cubemap_Top.png",
		"Assets/Cubemap/Test/Cubemap_Bottom.png",
		"Assets/Cubemap/Test/Cubemap_Back.png",
		"Assets/Cubemap/Test/Cubemap_Front.png"
		}, &Test_Cubemap);

	Push_Merged_Specular_Reflectivity("Assets/Textures/Black.png", "Assets/Textures/Black.png", "Black");

	// Push_Merged_Material("Assets/Textures/Reddened_Wall_Spec.png", nullptr, "Assets/Textures/Reddened_Wall_Normal.png", "NPP_Wall");

	Initialise_Particles();

	Initialise_UI_Shaders();

	Initialise_Fonts();

	//

	Scene_Object_Shader.Activate();

	Model_Uniform_Location = Initialise_Model_Uniform_Locations_Object(Scene_Object_Shader);
	Light_Uniform_Location = Initialise_Light_Uniform_Locations_Object(Scene_Object_Shader);
	Camera_Uniform_Location = Initialise_Camera_Uniform_Locations_Object(Scene_Object_Shader);

	//

	Initialise_Job_System();

	if (Shadow_Mapper::Shadow_Mapping)
		Shadow_Mapper::Initialise_Shadow_Mapper();

	Initialise_Sound_Effects();
}

#endif
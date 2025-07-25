#ifndef RADIOPHOBIA_ITEMS_DECLARATIONS
#define RADIOPHOBIA_ITEMS_DECLARATIONS

#include "..\OpenGL_Declarations.h"
#include "..\Model_Declarations.h"
#include "..\Input_Handler.h"

#include "Menus_Declarations.h"

#include "..\Decal_Projection_Clipper_Declarations.h"

namespace Player_Ammo_Count
{
	size_t Makarov_Ammo;
}

class Item
{
public:
	std::string Name;
	std::vector<Model> Viewmodel_Meshes; // Viewmodel_Meshes[0] is always the hands (unless there are none in the viewmodel)

	virtual void Handle_Viewmodel_States() {}
	virtual void Render_Viewmodel() {}
	virtual void Equip_Item() {}
	virtual void Holster() {}
	
	// This probably won't be added to the list of scene models, we'll just render them directly from the item. Less worries- less complicated
};

void Shoot_Bullet_Function(Hitbox* Shooter_Hitbox, glm::vec3 Position, glm::vec3 Orientation, float Damage);

Item* Player_Current_Item; // This is the item the player is currently holding
Item* Player_Desired_Item; // This is the item that the player is currently trying to switch to.

float Player_Viewmodel_Walk_Magnitude = 0.0f;
float Player_Viewmodel_Walk_Timer = 0.0f;

inline void Holster_Current_Item() { if (Player_Current_Item != nullptr) Player_Current_Item->Holster(); Player_Desired_Item = nullptr; }

Lightsource* Player_Flashlight;

class Pistol : public Item
{
public:
	Mesh_Animator Draw, Draw_Hand; // This can be played forwards/backwards when putting away the pistol haha
	Mesh_Animator Shoot, Shoot_Hand;
	Mesh_Animator Last_Shot, Last_Shot_Hand; // This is when the player fires their last shot
	Mesh_Animator Reload, Reload_Hand;

	// Note that we need a pair of animations for the pistol and the pistol's hand-holder

	enum Pistol_State {
		Drawing,
		Idling,
		Shooting,
		Firing_Last,
		Reloading,
		Holstering
	} Current_State = Drawing;

	size_t Ammo, Max_Ammo;

	float Time_Between_Shots;
	float Shot_Timer;

	// We'll just use a simple switch-statement to handle what the pistol does haha
	// Easy as pie

	virtual void Equip_Item() override
	{
		Current_State = Drawing;
		Draw.Time = 0.0f;
		Draw_Hand.Time = 0.0f;
	}

	virtual void Holster() override
	{
		Current_State = Holstering;
		Draw.Time = Draw.Animation->Duration / Draw.Animation->Tickrate;
		Draw_Hand.Time = Draw_Hand.Animation->Duration / Draw.Animation->Tickrate;
	}

	Pistol() // To create the pistol object, we'll need to create its models, receive its name, receive its animations, etc
	{

	}

	void Spawn_Bullet_Shell_And_Muzzle_Flash(glm::vec3& Position)
	{
		Position += glm::vec3(0.3688f) * Camera_Direction;
		Position -= glm::vec3(0.049f) * Viewmodel_Meshes[0].Orientation_Up;

		Position += glm::vec3(0.096f) * Viewmodel_Meshes[0].Orientation;

		glm::vec3 Velocity = glm::vec3(0.0f);

		Velocity += glm::vec3(RNG() + 0.5f) * Viewmodel_Meshes[0].Orientation;
		Velocity += glm::vec3(RNG() + 1.5f) * Viewmodel_Meshes[0].Orientation_Up;
		Velocity += glm::vec3(0.25f * RNG()) * Camera_Direction;

		Velocity += Player_Physics_Object.Velocity;

		glm::vec4 Parsed_Velocity = glm::vec4(Velocity.x, Velocity.y, Velocity.z, -atan2f(Camera_Direction.z, Camera_Direction.x));

		Position += glm::vec3(0.1f) * Camera_Direction;

		Scene_Lights.push_back(new Lightsource(Position, glm::vec3(0.3f) * glm::vec3(RNG() * 1 + 2, RNG() + 1, RNG()), glm::vec3(1.0f, 0.0f, 0.0f), 360.0f, 1.0f, 0.35f));
		Scene_Lights.back()->Flags[LF_CAST_SHADOWS] = true;
		Scene_Lights.back()->Flags[LF_TIMER] = true;
		//Scene_Lights.back()->Flags[LF_PRIORITY] = true;
		Scene_Lights.back()->Timer = 0.02f;

		Position += glm::vec3(0.1f) * Camera_Direction; // +Viewmodel_Meshes[0].Orientation * glm::vec3(0.02f);

		for (size_t W = 0; W < 20; W++)
			Muzzle_Flash_Particles.Particles.Spawn_Particle(Position + glm::vec3(0.1f * (RNG() - 0.5f), 0.1f * (RNG() - 0.5f), 0.1f * (RNG() - 0.5f)));

		Pistol_Shell_Particles.Particles.Spawn_Particle(Position, Parsed_Velocity, Player_Camera.Position.y + reinterpret_cast<AABB_Hitbox*>(Player_Physics_Object.Object->Hitboxes[0])->B.y - 0.02f);

	}

	virtual void Handle_Viewmodel_States() override
	{
		// 2.2f, -1.23f, 8.24f
		// if(Inputs[Controls::Lean_Left])
		//	Pistol_Shell_Particles.Particles.Spawn_Particle(glm::vec3(2.2f, -1.23f, 8.24f), glm::vec4(0, 0, -0.1f, 0.0f), Player_Camera.Position.y + reinterpret_cast<AABB_Hitbox*>(Player_Physics_Object.Object->Hitboxes[0])->B.y - 0.3f);

		if(Shot_Timer < Time_Between_Shots)
			Shot_Timer += Tick;
		else
			if (Mouse_Fresh_Click(0))
				if(Ammo)
				{
					Ammo--;
					Shot_Timer = 0.0f;
					Shoot.Time = 0;
					Shoot_Hand.Time = 0;

					Last_Shot.Time = 0.0f;
					Last_Shot_Hand.Time = 0.0f;

					Current_State = Ammo ? Shooting : Firing_Last;

					Player_Camera.Orientation.x += 3 * (RNG() - 0.5f);
					Player_Camera.Orientation.y += 3 * RNG();

					irrklang::ISound* Sound = Sound_Engine->play2D(Pull_Audio("Assets/Audio/Makarov.wav").Source, false, false, false, true);
					Sound->setPlaybackSpeed(0.25f * RNG() + 0.875f);

					glm::vec3 Position = Viewmodel_Meshes[0].Position;

					Spawn_Bullet_Shell_And_Muzzle_Flash(Position);
					
					Shoot_Bullet_Function(Player_Physics_Object.Object->Hitboxes[0], Player_Physics_Object.Object->Position, Camera_Direction, 2.0f - (Selected_Difficulty == Difficulty::Hard));
				}
				else if(Current_State != Reloading)
				{
					Sound_Engine->play2D(Pull_Audio("Assets/Audio/No_Ammo_Click.wav").Source);

					Shot_Timer = 0.0f;

					Last_Shot.Time = 0.075f;
					Last_Shot_Hand.Time = 0.075f;

					Current_State = Firing_Last;
				}

		bool Loop_Flag = false;

		switch (Current_State)
		{
		case(Drawing):

			Loop_Flag = Draw.Animate_Mesh(&Viewmodel_Meshes[1].Mesh, Tick, false);
			Draw_Hand.Animate_Mesh(&Viewmodel_Meshes[0].Mesh, Tick, false);

			if (Loop_Flag)
				Current_State = Idling;

			break;

		case(Idling):

			// Eh we don't really care about an animation if we're idling.

			// But, in this state, we can do inputs!

			if (Inputs[Controls::Auxilliary])
			{
				Current_State = Reloading;
				Reload.Time = 0.0f;
				Reload_Hand.Time = 0.0f;

				Sound_Engine->play2D(Pull_Audio("Assets/Audio/Makarov_Reload.wav").Source);
			}

			break;

		case(Holstering):

			Loop_Flag = Draw.Animate_Mesh(&Viewmodel_Meshes[1].Mesh, -1.5f * Tick, false); // This plays the animation in reverse, simple
			Draw_Hand.Animate_Mesh(&Viewmodel_Meshes[0].Mesh, -1.5f * Tick, false);

			if (Loop_Flag)									// If the animation is finished (includes Time < 0 AS WELL AS Time > Length)
			{
				Player_Current_Item = Player_Desired_Item;
				Equip_Item();
			}

			break;

		case(Shooting):

			Loop_Flag = Shoot.Animate_Mesh(&Viewmodel_Meshes[1].Mesh, Tick, false);
			Shoot_Hand.Animate_Mesh(&Viewmodel_Meshes[0].Mesh, Tick, false);

			if (Loop_Flag)					// If the animation is finished,
				Current_State = Idling;		// we return to idling

			break;

		case(Firing_Last):

			Loop_Flag = Last_Shot.Animate_Mesh(&Viewmodel_Meshes[1].Mesh, Tick, false);
			Last_Shot_Hand.Animate_Mesh(&Viewmodel_Meshes[0].Mesh, Tick, false);

			if (Loop_Flag)
				Current_State = Idling;

			break;

		case(Reloading):

			Loop_Flag = Reload.Animate_Mesh(&Viewmodel_Meshes[1].Mesh, Tick, false);
			Reload_Hand.Animate_Mesh(&Viewmodel_Meshes[0].Mesh, Tick, false);

			if (Loop_Flag) // If we've finished reloading
			{
				Ammo = Max_Ammo;
				Current_State = Idling;
			}

			break;

		default:
			break;
		}
	}

	virtual void Render_Viewmodel() override
	{
		float Speed = sqrtf(Fast::Square(Player_Physics_Object.Velocity.x) + Fast::Square(Player_Physics_Object.Velocity.z));

		Speed = 1.0f - expf(-Speed * 0.2f);

		Player_Viewmodel_Walk_Magnitude = 0.1f * Speed;
		Player_Viewmodel_Walk_Timer += 14.0f * Speed * Tick;

		Player_Flashlight->Position = Player_Camera.Position + glm::vec3(0.0f, 0.1f, 0.0f);
		Player_Flashlight->Direction = Camera_Direction;

		for (size_t W = 0; W < Viewmodel_Meshes.size(); W++)
		{
			float Delta_Y = 0.0f;
			float Delta_X = 0.0f;

			Viewmodel_Meshes[W].Position = Player_Camera.Position;

			Viewmodel_Meshes[W].Orientation = glm::cross(Camera_Direction, Camera_Up_Direction);
			Viewmodel_Meshes[W].Orientation_Up = -Camera_Up_Direction;

			if (Inputs[Controls::Down]) // These are the offsets required for when the player is ADS-ing
			{
				Delta_X -= 0.096437f;
				Delta_Y += 0.030538f;
				// Viewmodel_Meshes[W].Position -= glm::vec3(0.096437f) * Viewmodel_Meshes[W].Orientation;
				// Viewmodel_Meshes[W].Position += glm::vec3(0.030538f) * Viewmodel_Meshes[W].Orientation_Up;

				Viewmodel_Meshes[W].Position -= glm::vec3(0.05f) * Camera_Direction;

				Player_Camera.FOV = 50.0f;
			}
			else
			{
				Player_Camera.FOV = 70.0f;
				Delta_Y += sinf(glfwGetTime() * 0.7f) * 0.025f - 0.04;
				// Viewmodel_Meshes[W].Position += glm::vec3(sinf(glfwGetTime() * 0.7f) * 0.025f - 0.04) * Viewmodel_Meshes[W].Orientation_Up;
			}

			Delta_Y -= Player_Viewmodel_Walk_Magnitude * Fast::Square(sin(Player_Viewmodel_Walk_Timer));
			Delta_X += Player_Viewmodel_Walk_Magnitude * cos(Player_Viewmodel_Walk_Timer);

			Viewmodel_Meshes[W].Position += glm::vec3(Delta_X) * Viewmodel_Meshes[W].Orientation;
			Viewmodel_Meshes[W].Position += glm::vec3(Delta_Y) * Viewmodel_Meshes[W].Orientation_Up;

			Viewmodel_Meshes[W].Mesh.Bind_Buffer();
			Viewmodel_Meshes[W].Mesh.Update_Vertices();

			Viewmodel_Meshes[W].Render(Scene_Object_Shader); // I don't think there's much else to this!
		}
	}
};

Pistol Makarov_Pistol;

/*
class Radio : public Item // This test object is 2D
{
public:

	float Equip_Timer = 0.0f;
	float Radio_Y_Position = 0.0f;

	enum Radio_State 
	{
		Drawing = 0u,
		Holstering = 1u,
		Checking_Message = 2u
	} Current_State;

	virtual void Equip_Item() override
	{
		Current_State = Drawing;
	}

	virtual void Holster() override
	{
		Current_State = Holstering;
		Equip_Timer = 1.0f;
	}

	virtual void Handle_Viewmodel_States() override
	{
		const float Time_Required = 0.5f;

		float Approach_Factor = sin((Equip_Timer * 2.0f - 0.5f) * 3.14159) * 0.5f + 0.5f;

		switch (Current_State)
		{
			case(Drawing):
				Equip_Timer += Tick;
				Radio_Y_Position = Approach_Factor;

				if (Equip_Timer > Time_Required)
				{
					Radio_Y_Position = 1.0f;
					Equip_Timer = Time_Required;
					Current_State = Checking_Message;
				}
			break;

			case(Holstering):
				Equip_Timer -= Tick;
				Radio_Y_Position = Approach_Factor;

				if (Equip_Timer < 0.0f)
				{
					Player_Current_Item = Player_Desired_Item;
					Equip_Timer = 0.0f;
					Current_State = Drawing;
				}
			break;

			case(Checking_Message):

			break;
		}
	}

	virtual void Render_Viewmodel() override
	{
		UI_Elements.push_back(new UI_Element(-1.4f, 1.0f - 0.8f * Radio_Y_Position, -0.4f, 2.0 - 0.8f * Radio_Y_Position, Pull_Texture("Assets/UI/Geiger.png").Texture));
		UI_Elements.back()->Flags[UF_TO_BE_DELETED] = true;
		UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;
	}
};
Radio Player_Radio;
*/

void Initialise_Player_Flashlight()
{
	Player_Flashlight = new Lightsource(glm::vec3(0, 0, 0), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), 30.0f, 3.0f);
	Player_Flashlight->Flags[LF_PRIORITY] = true;
	Scene_Lights.push_back(Player_Flashlight);
}

void Initialise_Pistol()
{
	Pull_Audio("Assets/Audio/Makarov.wav");
	Pull_Audio("Assets/Audio/Makarov_Reload.wav");
	Pull_Audio("Assets/Audio/No_Ammo_Click.wav");

	Player_Current_Item = &Makarov_Pistol;
	Player_Desired_Item = &Makarov_Pistol;

	Makarov_Pistol.Draw.Animation = Pull_Animation("Assets/Animations/Makarov_Pistol_Draw.anim").Animation;
	Makarov_Pistol.Draw.Flags[ANIMF_LOOP_BIT] = false;
	Makarov_Pistol.Draw_Hand.Animation = Pull_Animation("Assets/Animations/Makarov_Hand_Draw.anim").Animation;
	Makarov_Pistol.Draw_Hand.Flags[ANIMF_LOOP_BIT] = false;



	Makarov_Pistol.Shoot.Animation = Pull_Animation("Assets/Animations/Makarov_Pistol_Shoot.anim").Animation;
	Makarov_Pistol.Shoot.Flags[ANIMF_LOOP_BIT] = false;
	Makarov_Pistol.Shoot_Hand.Animation = Pull_Animation("Assets/Animations/Makarov_Hand_Shoot.anim").Animation;
	Makarov_Pistol.Shoot_Hand.Flags[ANIMF_LOOP_BIT] = false;



	Makarov_Pistol.Last_Shot.Animation = Pull_Animation("Assets/Animations/Makarov_Pistol_Last_Shot.anim").Animation;
	Makarov_Pistol.Last_Shot.Flags[ANIMF_LOOP_BIT] = false;
	Makarov_Pistol.Last_Shot_Hand.Animation = Pull_Animation("Assets/Animations/Makarov_Hand_Last_Shot.anim").Animation;
	Makarov_Pistol.Last_Shot_Hand.Flags[ANIMF_LOOP_BIT] = false;



	Makarov_Pistol.Reload.Animation = Pull_Animation("Assets/Animations/Makarov_Pistol_Reload.anim").Animation;
	Makarov_Pistol.Reload.Flags[ANIMF_LOOP_BIT] = false;
	Makarov_Pistol.Reload_Hand.Animation = Pull_Animation("Assets/Animations/Makarov_Hand_Reload.anim").Animation;
	Makarov_Pistol.Reload_Hand.Flags[ANIMF_LOOP_BIT] = false;

	// This loads all of the necessary animations

	Makarov_Pistol.Viewmodel_Meshes.resize(2);

	Create_Model(Pull_Mesh<THREADED>("Assets/Models/Test_Makarov.obj", LOAD_MESH_ANIM_BIT | LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture<THREADED>("Assets/Textures/Arm_Texture_2.png").Texture, Pull_Texture("Brick").Texture, &Makarov_Pistol.Viewmodel_Meshes[0], new Controller(), std::vector<Hitbox*>());
	Create_Model(Pull_Mesh<THREADED>("Assets/Models/Test_Makarov_Shoot.obj", LOAD_MESH_ANIM_BIT | LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture<THREADED>("Assets/Textures/Walther_PPK.png").Texture, Pull_Texture("Walther_PPK").Texture, &Makarov_Pistol.Viewmodel_Meshes[1], new Controller(), std::vector<Hitbox*>());

	// This loads the models

	Makarov_Pistol.Max_Ammo = 15;
	Makarov_Pistol.Ammo = Makarov_Pistol.Max_Ammo;

	Makarov_Pistol.Time_Between_Shots = 0.051f;
	//Makarov_Pistol.Time_Between_Shots = 0.051f;
	Makarov_Pistol.Shot_Timer = Makarov_Pistol.Time_Between_Shots;

	//

	{
		Context_Interface::Request_Context();

		Shader Shell_Bounce_Shader;
		Shell_Bounce_Shader.Create_Shader("Shader_Code/Shell_Bounce_Particle.vert", "Shader_Code/Vertex_Test.frag", "Shader_Code/Vertex_Test.geom");

		Create_Particle_Renderer(Shell_Bounce_Shader, Pull_Mesh("Assets/Models/Makarov_Shell.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Shell_Texture.png").Texture, Pull_Texture("Metal").Texture, &Pistol_Shell_Particles);

		//

		Shader Decal_Shader;
		Decal_Shader.Create_Shader("Shader_Code/Decal_Particle.vert", "Shader_Code/Decal_Particle.frag", nullptr);

		Create_Particle_Renderer(Decal_Shader, Decal_Vertex_Buffer(0), Pull_Texture("Assets/Textures/Bullet_Decal.png").Texture, Pull_Texture("Black").Texture, &Decal_Particles);

		//

		/*Shader Brick_Decal_Shader;
		Brick_Decal_Shader.Create_Shader("Shader_Code/Decal_Particle.vert", "Shader_Code/Exposed_Brick_Decal_Particle.frag", nullptr);

		Create_Particle_Renderer(Brick_Decal_Shader, Decal_Vertex_Buffer(0), Pull_Texture("Assets/Textures/Brick1.png").Texture, Pull_Texture("Brick").Texture, &Brick_Decal_Particles);
		*/

		Context_Interface::Free_Context();

	}

	// This sets some stats for the pistol
}

void Handle_Player_Items()
{
	if (!Player_Flags[Player_Items_Revoke_Flag]) // As long as the player's items are revoked, they can't swap back to any item
	{
		if (Inputs[Controls::Item_2])
		{
			if (Player_Current_Item != &Makarov_Pistol)
			{
				Player_Desired_Item = &Makarov_Pistol;

				if(Player_Current_Item != nullptr)		// If the player actually is holding something,
					Player_Current_Item->Holster();		// We need to holster it before they can pull out another item
			}
		}

		/*if (Inputs[Controls::Item_1])
		{
			if (Player_Current_Item != &Player_Radio)
			{
				Player_Desired_Item = &Player_Radio;

				if (Player_Current_Item != nullptr)
					Player_Current_Item->Holster();
			}
		}*/
	}
}

#endif
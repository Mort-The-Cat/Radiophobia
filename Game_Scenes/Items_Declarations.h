#ifndef RADIOPHOBIA_ITEMS_DECLARATIONS
#define RADIOPHOBIA_ITEMS_DECLARATIONS

#include "..\OpenGL_Declarations.h"
#include "..\Model_Declarations.h"
#include "..\Input_Handler.h"

namespace Player_Ammo_Count
{
	size_t Makarov_Ammo;
}

class Item
{
public:
	std::string Name;
	std::vector<Model> Viewmodel_Meshes; // Viewmodel_Meshes[0] is always the hands (unless there are none in the viewmodel)
	
	// This probably won't be added to the list of scene models, we'll just render them directly from the item. Less worries- less complicated
};

Item* Player_Current_Item; // This is the item the player is currently holding
Item* Player_Desired_Item; // This is the item that the player is currently trying to switch to.

namespace Pistol_SFX
{
	irrklang::ISoundSource *Shoot, *Click, *Draw, *Holster;
}

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

	Pistol() // To create the pistol object, we'll need to create its models, receive its name, receive its animations, etc
	{

	}

	void Handle_Viewmodel_States()
	{
		if(Shot_Timer < Time_Between_Shots)
			Shot_Timer += Tick;
		else
			if ((Controller_Inputs.axes[Gamepad_Controls::Attack] > 0.35 || Mouse_Fresh_Click(0)) && Ammo)
			{
				Ammo--;
				Shot_Timer = 0.0f;
				Shoot.Time = 0;
				Shoot_Hand.Time = 0;

				Last_Shot.Time = 0.0f;
				Last_Shot_Hand.Time = 0.0f;

				Current_State = Ammo ? Shooting : Firing_Last;

				Scene_Models.push_back(new Model({  }));
				Scene_Models.back()->Position = Viewmodel_Meshes[0].Position;
				Create_Model(Pull_Mesh("Assets/Models/Makarov_Shell.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Shell_Texture.png").Texture, Pull_Texture("Metal").Texture, Scene_Models.back(), new Controller(), std::vector<Hitbox*>{});

				Scene_Models.back()->Position += glm::vec3(0.3688f) * Camera_Direction;
				Scene_Models.back()->Position -= glm::vec3(0.049f) * Viewmodel_Meshes[0].Orientation_Up;

				Scene_Models.back()->Position += glm::vec3(0.096f) * Viewmodel_Meshes[0].Orientation;

				Scene_Models.back()->Orientation = Viewmodel_Meshes[0].Orientation;
				Scene_Models.back()->Orientation_Up = Viewmodel_Meshes[0].Orientation_Up;
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

			if (Inputs[Controls::Auxilliary] || Controller_Inputs.axes[Gamepad_Controls::Auxilliary])
			{
				Current_State = Reloading;
				Reload.Time = 0.0f;
				Reload_Hand.Time = 0.0f;
			}

			break;

		case(Holstering):

			Draw.Animate_Mesh(&Viewmodel_Meshes[1].Mesh, -Tick, false); // This plays the animation in reverse, simple
			Draw_Hand.Animate_Mesh(&Viewmodel_Meshes[0].Mesh, -Tick, false);

			Player_Current_Item = Player_Desired_Item;

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

	void Render_Viewmodel()
	{
		for (size_t W = 0; W < Viewmodel_Meshes.size(); W++)
		{
			Viewmodel_Meshes[W].Position = Player_Camera.Position;

			Viewmodel_Meshes[W].Orientation = glm::cross(Camera_Direction, Camera_Up_Direction);
			Viewmodel_Meshes[W].Orientation_Up = -Camera_Up_Direction;

			if (Inputs[Controls::Down]) // These are the offsets required for when the player is ADS-ing
			{
				Viewmodel_Meshes[W].Position -= glm::vec3(0.096437f) * Viewmodel_Meshes[W].Orientation;
				Viewmodel_Meshes[W].Position += glm::vec3(0.030538f) * Viewmodel_Meshes[W].Orientation_Up;

				Viewmodel_Meshes[W].Position -= glm::vec3(0.05f) * Camera_Direction;

				Player_Camera.FOV = 50.0f;
			}
			else
			{
				Player_Camera.FOV = 70.0f;
				Viewmodel_Meshes[W].Position += glm::vec3(sinf(glfwGetTime() * 0.7f) * 0.025f - 0.04) * Viewmodel_Meshes[W].Orientation_Up;
			}

			Viewmodel_Meshes[W].Mesh.Bind_Buffer();
			Viewmodel_Meshes[W].Mesh.Update_Vertices();

			Viewmodel_Meshes[W].Render(Scene_Object_Shader); // I don't think there's much else to this!
		}
	}
};

Pistol Makarov_Pistol;

void Initialise_Pistol()
{
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
	Create_Model(Pull_Mesh<THREADED>("Assets/Models/Test_Makarov_Shoot.obj", LOAD_MESH_ANIM_BIT | LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture<THREADED>("Assets/Textures/Gun_Texture.png").Texture, Pull_Texture("Floor").Texture, &Makarov_Pistol.Viewmodel_Meshes[1], new Controller(), std::vector<Hitbox*>());

	// This loads the models

	Makarov_Pistol.Max_Ammo = 7;
	Makarov_Pistol.Ammo = Makarov_Pistol.Max_Ammo;

	Makarov_Pistol.Time_Between_Shots = 0.15f;
	Makarov_Pistol.Shot_Timer = Makarov_Pistol.Time_Between_Shots;

	// This sets some stats for the pistol
}
#endif
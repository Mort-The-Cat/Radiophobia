#ifndef RADIOPHOBIA_INTERACTABLE_DECLARATIONS
#define RADIOPHOBIA_INTERACTABLE_DECLARATIONS

#include "..\OpenGL_Declarations.h"
#include "..\Mesh_Animator_Declarations.h"
#include "..\Job_System_Declarations.h"
#include "..\Asset_Loading_Cache.h"
#include "..\Hitdetection.h"
#include "..\Audio_Declarations.h"

void Create_UI_Popup(const char* Directory, float Transparency)
{
	UI_Elements.push_back(new Text_UI_Element(-0.9f, -0.9f, strcmp(Current_Language_Setting.c_str(), "Deutsch") == 0 ? 0.9f : 0.3f, -0.65f,
		Directory, true,
		glm::vec4(1.0f, 1.0f, 1.0f, Transparency * 2.0f), &Font_Console));

	UI_Elements.back()->Colour.w = Transparency * 0.5f;

	UI_Elements.back()->Flags[UF_IMAGE] = true;

	//UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;

	UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;

	UI_Elements.back()->Flags[UF_TO_BE_DELETED] = true;

	UI_Elements.back()->Flags[UF_AUTO_RESIZE_TO_TEXT] = true;
}

class Thought_Popups_Controller : public Controller
{
public:
	struct Thought_Trigger
	{
		std::string Inner_Monologue; // Directory of text
		glm::vec3 Position; // Position of the thought trigger
		float Radius; // Radius at which thought trigger is activated
		float Time = 0;
	};

	std::vector<Thought_Trigger> Thought_Triggers;

	Thought_Popups_Controller(std::vector<Thought_Trigger> Thought_Triggersp)
	{
		Thought_Triggers = Thought_Triggersp;

		//for (size_t W = 0; W < Thought_Triggers.size(); W++)
		//	Pull_Text(Thought_Triggers[W].Inner_Monologue.c_str(), true); // This will load all of the text in nicely
	}

	virtual void Initialise_Control(Model* Objectp) override
	{
		Object = Objectp;
	}

	virtual void Control_Function() override
	{
		// Go through every thought trigger

		for (size_t W = 0; W < Thought_Triggers.size(); W++)
		{
			glm::vec3 Delta_Vector = Player_Physics_Object.Object->Position - Thought_Triggers[W].Position;

			if (glm::dot(Delta_Vector, Delta_Vector) < Thought_Triggers[W].Radius * Thought_Triggers[W].Radius)
			{
				Thought_Triggers[W].Time = std::fminf(Thought_Triggers[W].Time + Tick, 1);
			}
			else
				Thought_Triggers[W].Time = std::fmaxf(Thought_Triggers[W].Time - Tick, 0);

			if (Thought_Triggers[W].Time > 0)
			{
				Pull_Text(("Assets/Text/" + Current_Language_Setting + "/" + Thought_Triggers[W].Inner_Monologue).c_str(), true); // This will load in the value we want

				Create_UI_Popup(Thought_Triggers[W].Inner_Monologue.c_str(), Thought_Triggers[W].Time);

				/**UI_Elements.push_back(new Text_UI_Element(-0.9f, -0.9f, strcmp(Current_Language_Setting.c_str(), "Deutsch") == 0 ? 0.9f : 0.3f, -0.65f,
					Thought_Triggers[W].Inner_Monologue, true, 
					glm::vec4(1.0f, 1.0f, 1.0f, Thought_Triggers[W].Time * 2.0f), &Font_Console));

				UI_Elements.back()->Colour.w = Thought_Triggers[W].Time * 0.5f;
				
				UI_Elements.back()->Flags[UF_IMAGE] = true;

				//UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;
				UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
				UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;

				UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;

				UI_Elements.back()->Flags[UF_TO_BE_DELETED] = true;

				UI_Elements.back()->Flags[UF_AUTO_RESIZE_TO_TEXT] = true;*/
			}
		}

		// Check if colliding with player and set hitbox pointers

		// Render popups that collide with player
	}
};

class Phone_Controller : public Controller
{
	enum State
	{
		Idle = 0,
		Ringing = 1,
		Picking_Up = 2,
		Calling = 3,
		Putting_Down = 4
	} Current_State = Ringing;

public:
	Mesh_Animator Pickup_Animation;

	Audio::Audio_Source* Sound = nullptr;

	float Time = 0.0f;

	~Phone_Controller()
	{
		Sound->Flags[ASF_TO_BE_DELETED] = true;
	}

	Phone_Controller()
	{

	}

	virtual void Initialise_Control(Model* Objectp) override
	{
		Object = Objectp;
		Pickup_Animation.Animation = Pull_Animation("Assets/Animations/Intro_Level/Phone_Pickup.anim").Animation;
		Pickup_Animation.Flags[ANIMF_LOOP_BIT] = false;

		Sound = Audio::Create_Audio_Source(Object->Position - Object->Orientation * glm::vec3(0.25f), 1.2f);

		glm::mat3 Direction_Matrix = Direction_Matrix_Calculate(glm::vec3(0.0f), Object->Orientation, Object->Orientation_Up);

		Object->Hitboxes.push_back(Generate_AABB_Hitbox(*Object->Mesh.Mesh));
		reinterpret_cast<AABB_Hitbox*>(Object->Hitboxes[0])->A = Direction_Matrix * reinterpret_cast<AABB_Hitbox*>(Object->Hitboxes[0])->A;
		reinterpret_cast<AABB_Hitbox*>(Object->Hitboxes[0])->B = Direction_Matrix * reinterpret_cast<AABB_Hitbox*>(Object->Hitboxes[0])->B;

		if (reinterpret_cast<AABB_Hitbox*>(Object->Hitboxes[0])->A.x > reinterpret_cast<AABB_Hitbox*>(Object->Hitboxes[0])->B.x)
			std::swap(reinterpret_cast<AABB_Hitbox*>(Object->Hitboxes[0])->A.x, reinterpret_cast<AABB_Hitbox*>(Object->Hitboxes[0])->B.x);

		if (reinterpret_cast<AABB_Hitbox*>(Object->Hitboxes[0])->A.z > reinterpret_cast<AABB_Hitbox*>(Object->Hitboxes[0])->B.z)
			std::swap(reinterpret_cast<AABB_Hitbox*>(Object->Hitboxes[0])->A.z, reinterpret_cast<AABB_Hitbox*>(Object->Hitboxes[0])->B.z);
	}

	virtual void Control_Function() override
	{
		// if(Inputs[Controls::Use])

		Hitbox* Collided;
		Collision_Info Info;

		Time += Tick;

		switch (Current_State)
		{
		case State::Idle:
		default:
			break;

		case State::Ringing:
			if (Time > 5.0)
			{
				Time = 0.0f;
				Sound->Play_Sound(Pull_Audio("Assets/Audio/Phone/Telephone_Ring.wav").Source);
			}

			// Handle checks for picking up

			Info = Collision_Test::Raycast(Player_Camera.Position, glm::vec3(0.0625f) * Camera_Direction, 9, Collision_Test::Not_Against_Player_Compare, &Collided, 0.05);

			if (Collided == Object->Hitboxes[0])
			{
				Create_UI_Popup("Pickup_Phone.txt", 1.0f);

				if (Inputs[Controls::Use])
				{
					Current_State = State::Picking_Up;

					//

					Time = 0.0f;

					Player_Desired_Item = nullptr;

					Holster_Current_Item();

					Fade_To_Colour(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 2.0f);

					Player_Flags[Player_Movement_Revoke_Flag] = true;
					Player_Flags[Player_Direction_Revoke_Flag] = true;
					Player_Flags[Player_Items_Revoke_Flag] = true;
				}
			}

			break;

		case State::Picking_Up:

			/*

			5.296764, -0.975419, -0.974091
			555.532898, -4.183055, 0.000000
			 >> Camera info:
			5.296764, -0.975419, -0.974091
			555.532898, -4.183055, 0.000000

			*/

			if (Time > 0.5f && Time < 3.0f)
			{
				Fade_From_Colour(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f);

				Sound->Play_Sound(Pull_Audio("Assets/Audio/Phone/Pickup_Phone.wav").Source);

				Time = 4.0f;

				Player_Physics_Object.Object->Position = glm::vec3(5.296f, -0.975419f, -0.9741f); // Approach_Vector(Player_Physics_Object.Object->Position, glm::vec3(5.296f, -0.975419f, -0.9741f), 0.75f * Tick);

				Player_Camera.Orientation.x = Player_Camera.Orientation.x - 360.0f * std::floorf(Player_Camera.Orientation.x / 360.0f);

				//

				Player_Camera.Orientation = glm::vec3(195.0f, -4.183055, 0.0f);
			}

			if (Time < 3.0f)
				break;

			Object->Flags[MF_UPDATE_MESH] = !Pickup_Animation.Animate_Mesh(&Object->Mesh, Tick, true);

			if (!Object->Flags[MF_UPDATE_MESH])
			{
				Player_Camera.Orientation = glm::vec3(195.0f, -4.183055, 0.0f);

				Current_State = State::Calling;
				Time = 0.0f;
				Sound->Play_Sound(Pull_Audio("Assets/Audio/Phone/Static.wav").Source);
			}
			break;

		case State::Calling:
			if (Time > 1.0f && Time < 10.0f)
			{
				Sound->Play_Sound(Pull_Audio("Assets/Audio/Phone/I_CAN_SEE_YOU2.wav").Source);
				Time = 10.0f;
			}
			else if (Time > 16.0f && Time < 30.0f)
			{
				Sound->Play_Sound(Pull_Audio("Assets/Audio/Phone/Endcall_Beep.wav").Source);
				Time = 30.0f;
			}

			if (Time > 35.0f)
			{
				Current_State = State::Putting_Down;
				Time = 0.0f;
			}
			break;

		case State::Putting_Down:
			Object->Flags[MF_UPDATE_MESH] = !Pickup_Animation.Animate_Mesh(&Object->Mesh, -Tick, true);

			if (Time > 0.675f && Time < 10.0f)
			{
				Sound->Volume += 0.3f;
				Sound->Play_Sound(Pull_Audio("Assets/Audio/Phone/Putdown_Phone.wav").Source);
				Time = 10.0f;
			}

			if (Pickup_Animation.Time <= 0.0f)
			{
				Current_State = State::Idle;

				Player_Flags[Player_Movement_Revoke_Flag] = false;
				Player_Flags[Player_Direction_Revoke_Flag] = false;
				Player_Flags[Player_Items_Revoke_Flag] = false;

				Player_Desired_Item = &Makarov_Pistol;
				Makarov_Pistol.Equip_Item();
			}
			
			break;
		}
	}
};

class Damageable_Vent_Controller : public Damageable_Controller
{
	std::string Directory;

public:
	enum State
	{
		Untouched = 0,
		Damaged = 1u,
		Removed = 2u
	} Current_State = State::Untouched;

	Mesh_Animator Damaged_Animation;

	Mesh_Animator Remove_Animation;

	Audio::Audio_Source* Damaged_Sound;

	~Damageable_Vent_Controller()
	{
		Damaged_Sound->Flags[ASF_TO_BE_DELETED] = true;
	}

	Damageable_Vent_Controller(std::string Directoryp) 
	{
		Directory = Directoryp;
		Health = 8.0f;
	}

	virtual void Control_Function() override
	{
		switch (Current_State)
		{
		case State::Damaged:
			Object->Flags[MF_UPDATE_MESH] = true;// !Damaged_Animation.Animate_Mesh(&Object->Mesh, Tick, true);
			Current_State = State::Untouched;
			break;

		case State::Removed:
			Object->Flags[MF_UPDATE_MESH] = !Remove_Animation.Animate_Mesh(&Object->Mesh, Tick, true);
			Object->Flags[MF_ACTIVE] &= Object->Flags[MF_UPDATE_MESH];
			break;

		default:
			Object->Flags[MF_UPDATE_MESH] = false;
			break;
		}
	}

	virtual void Initialise_Control(Model* Objectp) override
	{
		Object = Objectp;
		Object->Flags[MF_TAKES_DAMAGE] = true;

		Damaged_Animation.Animation = Pull_Animation((Directory + "_Damage.anim").c_str()).Animation;
		Damaged_Animation.Flags[ANIMF_LOOP_BIT] = false;
		Damaged_Animation.Time = 0.0f;
		Remove_Animation.Animation = Pull_Animation((Directory + "_Remove.anim").c_str()).Animation;
		Remove_Animation.Flags[ANIMF_LOOP_BIT] = false;
		Remove_Animation.Time = 0.0f;

		Damaged_Sound = Audio::Create_Audio_Source(glm::vec3(0.0f), 1.0f);
	}

	virtual void Damage(float Delta, Damage_Source Damage_Type = Damage_Source::Bullet) override
	{
		Health -= Delta;

		if (Health < 0.0f)
		{
			Damaged_Sound->Position = reinterpret_cast<AABB_Hitbox*>(Object->Hitboxes[0])->A;
			// Player interacted with the door!

			// This'll basically remove any collision that the object has
			reinterpret_cast<AABB_Hitbox*>(Object->Hitboxes[0])->A.y += 9999.0f;
			reinterpret_cast<AABB_Hitbox*>(Object->Hitboxes[0])->B.y += 9999.0f;

			Damaged_Sound->Play_Sound(Pull_Audio("Assets/Audio/Door/Door_Open.wav").Source);

			Current_State = State::Removed;
		}
		else if (Health < 6.5f)
		{
			Damaged_Animation.Animate_Mesh(&Object->Mesh, Delta / 60.0f, true);
			Current_State = State::Damaged;
		}
	}
};

class Level_Exit_Door_Controller : public Controller
{
	void(*Load_New_Level_Function)();
public:

	Level_Exit_Door_Controller(void(*Load_New_Level_Functionp)())
	{
		Load_New_Level_Function = Load_New_Level_Functionp;
	}

	virtual void Control_Function() override
	{
		Hitbox* Collided;
		Collision_Info Info = Collision_Test::Raycast(Player_Camera.Position, glm::vec3(0.15f) * Camera_Direction, 4, Collision_Test::Not_Against_Player_Compare, &Collided, 0.1f);

		if (Collided == Object->Hitboxes[0])
		{
			Create_UI_Popup("Open_Door.txt", 1.0f);

			if (Inputs[Controls::Use])
				Load_New_Level_Function();
		}
	}
};

class Door_Controller : public Controller
{
	std::string Animation_Name;

	bool Opened = false;

public:
	Mesh_Animator Animator;

	Audio::Audio_Source* Opening_Sound;

	~Door_Controller()
	{
		Opening_Sound->Flags[ASF_TO_BE_DELETED] = true;
	}

	Door_Controller(const char* Directory)
	{
		Animation_Name = Directory;
	}

	virtual void Control_Function() override
	{
		if (Opened)
		{
			Object->Flags[MF_UPDATE_MESH] = !Animator.Animate_Mesh(&Object->Mesh, Tick, true);

			if (!Object->Flags[MF_UPDATE_MESH])
			{
				Object->Flags[MF_ACTIVE] = false;
				//Opening_Sound->Play_Sound(Door_Finished_Opening_Sound);
			}

			//Object->Flags[MF_ACTIVE] &= Object->Flags[MF_UPDATE_MESH];
		}
		else
		{
			// Object->Flags[MF_UPDATE_MESH] = false;

			Hitbox* Collided;
			Collision_Info Info = Collision_Test::Raycast(Player_Camera.Position, glm::vec3(0.25f) * Camera_Direction, 3, Collision_Test::Not_Against_Player_Compare, &Collided);

			if (Collided == Object->Hitboxes[0])
			{
				Create_UI_Popup("Open_Door.txt", 1.0f);

				if (Inputs[Controls::Use])
				{
					Opening_Sound->Position = reinterpret_cast<AABB_Hitbox*>(Object->Hitboxes[0])->A;
					// Player interacted with the door!

					// This'll basically remove any collision that the object has
					reinterpret_cast<AABB_Hitbox*>(Object->Hitboxes[0])->A.y += 9999.0f;
					reinterpret_cast<AABB_Hitbox*>(Object->Hitboxes[0])->B.y += 9999.0f;

					Opened = true;

					Opening_Sound->Play_Sound(Pull_Audio("Assets/Audio/Door/Door_Open.wav").Source);
				}
			}
		}
	}

	virtual void Initialise_Control(Model* Objectp) override
	{
		Object = Objectp;

		Animator.Animation = Pull_Animation(Animation_Name.c_str()).Animation;
		Animator.Time = 0.0f;
		Animator.Flags[ANIMF_LOOP_BIT] = false;

		// Animator.Animate_Mesh(&Object->Mesh, 0.0f, false, true); // We just want to correct the animation if there's an error once it was loaded

		//Object->Mesh.Bind_Buffer();
		//Object->Mesh.Update_Vertices();

		Object->Hitboxes.push_back(Generate_AABB_Hitbox(*Object->Mesh.Mesh));

		// Object->Flags[MF_UPDATE_MESH] = true;

		Opening_Sound = Audio::Create_Audio_Source(Object->Position, 1.0f);
	}
};

class Alarm_Light_Controller : public Controller // Need to continue work on this
{
public:
	float Direction = 0.0f;
	Lightsource* Light;
	Alarm_Light_Controller() {}

	virtual void Initialise_Control(Model* Objectp) override
	{
		Object = Objectp;

		Light = new Lightsource(Object->Position, glm::vec3(0.75f) * glm::vec3(1.0f, 0.4f, 0.4f), glm::vec3(0.0f), 35.0f, 10.0f, 0.6f);
		Scene_Lights.push_back(Light);

		Light->Light_Group = LG::Group_One;

		// Light->Flags[LF_PRIORITY] = true;

		/*Scene_Lights.push_back(new Lightsource(Object->Position, glm::vec3(0.75f) * glm::vec3(1.0f, 0.4f, 0.4f), glm::vec3(0.0f), 35.0f, 10.0f, 0.6f));
		Scene_Lights.back()->Flags[LF_TO_BE_DELETED] = true;
		Scene_Lights.back()->Flags[LF_PRIORITY] = true;*/

		// This will help the lighting BVH to generate the BVH in a more helpful way
	}

	virtual void Control_Function() override
	{
		Direction += Tick * 3.14159f * 1.0f;

		glm::vec3 Direction_Vector;

		Direction_Vector.x = sin(Direction) * 0.866f;
		Direction_Vector.z = cos(Direction) * 0.866f;
		Direction_Vector.y = 0.5f;

		//Direction_Vector.x =  0.5f;
		//Direction_Vector.y = sin(Direction) *0.866f;
		//Direction_Vector.z = cos(Direction) *0.866f;

		Light->Direction = Direction_Vector;
		
		//Scene_Lights.back()->Flags[LF_PRIORITY] = true;
		//Scene_Lights.back()->Flags[LF_CAST_SHADOWS] = true;

		//Scene_Lights.push_back(new Lightsource(Object->Position, glm::vec3(0.75f) * glm::vec3(1.3f, 0.25f, 0.25f), Direction_Vector, 35.0f, 10.0f, 0.6f));
		//Scene_Lights.back()->Flags[LF_TO_BE_DELETED] = true;
		//Scene_Lights.back()->Flags[LF_PRIORITY] = true;

		//Volumetric_Cone_Particles.Particles.Particles_Data.clear();
		//Volumetric_Cone_Particles.Particles.Spawn_Particle(Object->Position, -Direction_Vector, glm::vec3(1.0f, 0.4f, 0.4f), 0.15f, 30.0f);
	}
};

class Flicker_Light_Controller : public Controller
{
	struct Light_Flickerer
	{
		float Time_Until_Flicker = 3.0f;	// Time until next toggle
		uint8_t Flicker_Toggle = 0;		// This tells the controller whether or not the light needs to be halved or doubled
	};

public:
	std::vector<Light_Flickerer> Flickers;
	std::vector<Lightsource*> Lightsources;

	Flicker_Light_Controller(std::vector<Lightsource*> Lightsourcesp) 
	{
		Lightsources = Lightsourcesp;
		Flickers.resize(Lightsources.size() >> 1u);
	}

	virtual void Initialise_Control(Model* Objectp) override
	{
		Object = Objectp;
	}

	virtual void Control_Function() override
	{
		size_t Size = Flickers.size();
		for (size_t W = 0; W < Size; W++)
		{
			Flickers[W].Time_Until_Flicker -= Tick;

			if (Flickers[W].Time_Until_Flicker < 0.0f)
			{
				if (Flickers[W].Flicker_Toggle)
				{
					Lightsources[W]->Colour *= glm::vec3(4.0f);
					Lightsources[W + Size]->Colour *= glm::vec3(4.0f);
					Flickers[W].Time_Until_Flicker = RNG() * 3;
				}
				else
				{
					Lightsources[W]->Colour *= glm::vec3(0.25f);
					Lightsources[W + Size]->Colour *= glm::vec3(0.25f);
					Flickers[W].Time_Until_Flicker = RNG();
				}

				Flickers[W].Flicker_Toggle = !Flickers[W].Flicker_Toggle;
			}
		}
	}

};

#endif
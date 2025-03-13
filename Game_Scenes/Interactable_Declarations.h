#ifndef RADIOPHOBIA_INTERACTABLE_DECLARATIONS
#define RADIOPHOBIA_INTERACTABLE_DECLARATIONS

#include "..\OpenGL_Declarations.h"
#include "..\Mesh_Animator_Declarations.h"
#include "..\Job_System_Declarations.h"
#include "..\Asset_Loading_Cache.h"
#include "..\Hitdetection.h"
#include "..\Audio_Declarations.h"

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
				UI_Elements.push_back(new Text_UI_Element(-0.9f, -0.9f, strcmp(Current_Language_Setting.c_str(), "Deutsch") == 0 ? 0.9f : 0.3f, -0.65f, 
					Thought_Triggers[W].Inner_Monologue, true, 
					glm::vec4(1.0f, 1.0f, 1.0f, Thought_Triggers[W].Time * 2.0f), &Font_Console));
				UI_Elements.back()->Colour.w = Thought_Triggers[W].Time * 0.5f;
				
				UI_Elements.back()->Flags[UF_IMAGE] = true;

				//UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;
				UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
				UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;

				UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;

				UI_Elements.back()->Flags[UF_TO_BE_DELETED] = true;
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
				UI_Elements.push_back(new Text_UI_Element(-0.9f, -0.9f, strcmp(Current_Language_Setting.c_str(), "Deutsch") == 0 ? 0.9f : 0.3f, -0.65f, "Pickup_Phone.txt", true, glm::vec4(1.0f, 1.0f, 1.0f, 2.0f), &Font_Console));
				/* Added extra check here because German is so much longer xd */

				UI_Elements.back()->Colour.w = 0.5f;

				UI_Elements.back()->Flags[UF_IMAGE] = true;

				//UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;
				UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
				UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;

				UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;

				UI_Elements.back()->Flags[UF_TO_BE_DELETED] = true;

				if (Inputs[Controls::Use])
				{
					Current_State = State::Picking_Up;
					Sound->Play_Sound(Pull_Audio("Assets/Audio/Phone/Pickup_Phone.wav").Source);

					Fade_From_Colour(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f);
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

			Holster_Current_Item();

			Player_Flags[Player_Movement_Revoke_Flag] = true;
			Player_Flags[Player_Direction_Revoke_Flag] = true;

			Player_Physics_Object.Object->Position = glm::vec3(5.296f, -0.975419f, -0.9741f); // Approach_Vector(Player_Physics_Object.Object->Position, glm::vec3(5.296f, -0.975419f, -0.9741f), 0.75f * Tick);

			Player_Camera.Orientation.x = Player_Camera.Orientation.x - 360.0f * std::floorf(Player_Camera.Orientation.x / 360.0f);

			//

			Player_Camera.Orientation = glm::vec3(195.0f, -4.183055, 0.0f);

			//Player_Camera.Orientation *= glm::vec3(1.0f - 3.5f * Tick);
			//Player_Camera.Orientation += glm::vec3(3.5f * Tick) * glm::vec3(195.0f, -4.183055, 0.0f);

			// Player_Camera.Orientation = glm::vec3(195.0f, -4.183055, 0.0f); //glm::normalize((reinterpret_cast<AABB_Hitbox*>(Object->Hitboxes[0])->A + Object->Position) - Player_Physics_Object.Object->Position);
			
			// Player_Camera.Orientation = Approach_Vector(Player_Camera.Orientation, glm::vec3(195.0f, -4.183055, 0.0f), 90.0f * Tick);

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
			Hitbox* Collided;
			Collision_Info Info = Collision_Test::Raycast(Player_Camera.Position, glm::vec3(0.25f) * Camera_Direction, 3, Collision_Test::Not_Against_Player_Compare, &Collided);

			if (Collided == Object->Hitboxes[0])
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

	virtual void Initialise_Control(Model* Objectp) override
	{
		Object = Objectp;

		Object->Hitboxes.push_back(Generate_AABB_Hitbox(*Object->Mesh.Mesh));

		Animator.Animation = Pull_Animation(Animation_Name.c_str()).Animation;
		Animator.Time = 0.0f;
		Animator.Flags[ANIMF_LOOP_BIT] = false;

		Opening_Sound = Audio::Create_Audio_Source(Object->Position, 1.0f);
	}
};

#endif
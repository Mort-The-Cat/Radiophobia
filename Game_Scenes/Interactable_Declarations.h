#ifndef RADIOPHOBIA_INTERACTABLE_DECLARATIONS
#define RADIOPHOBIA_INTERACTABLE_DECLARATIONS

#include "..\OpenGL_Declarations.h"
#include "..\Mesh_Animator_Declarations.h"
#include "..\Job_System_Declarations.h"
#include "..\Asset_Loading_Cache.h"
#include "..\Hitdetection.h"
#include "..\Audio_Declarations.h"

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

	Damageable_Vent_Controller(std::string Directoryp) 
	{
		Directory = Directoryp;
		Health = 5.0f;
	}

	virtual void Control_Function() override
	{
		switch (Current_State)
		{
		case State::Damaged:
			Object->Flags[MF_UPDATE_MESH] = !Damaged_Animation.Animate_Mesh(&Object->Mesh, Tick, true);
			break;

		case State::Removed:
			Object->Flags[MF_UPDATE_MESH] = !Remove_Animation.Animate_Mesh(&Object->Mesh, Tick, true);
			Object->Flags[MF_ACTIVE] &= Object->Flags[MF_UPDATE_MESH];
			break;

		default:
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
		else if(Health < 3.0f)
			Current_State = State::Damaged;
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

			Object->Flags[MF_ACTIVE] &= Object->Flags[MF_UPDATE_MESH];

			/*if (!Object->Flags[MF_UPDATE_MESH])
			{
				Opening_Sound->Play_Sound(Door_Finished_Opening_Sound);
			}*/
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
#ifndef RADIOPHOBIA_INTERACTABLE_DECLARATIONS
#define RADIOPHOBIA_INTERACTABLE_DECLARATIONS

#include "..\OpenGL_Declarations.h"
#include "..\Mesh_Animator_Declarations.h"
#include "..\Job_System_Declarations.h"
#include "..\Asset_Loading_Cache.h"
#include "..\Hitdetection.h"
#include "..\Audio_Declarations.h"

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
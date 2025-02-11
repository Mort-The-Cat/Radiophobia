#ifndef RADIOPHOBIA_ENEMY_DECLARATIONS
#define RADIOPHOBIA_ENEMY_DECLARATIONS

#include "../OpenGL_Declarations.h"
#include "../Job_System.h"
#include "../Job_System_Declarations.h"
#include "../Asset_Loading_Cache.h"

#include "../Model_Declarations.h"

class Test_Enemy_Controller : public Damageable_Controller
{
public:
	Mesh_Animator Animator;

	Test_Enemy_Controller(float Healthp)
	{ 
		Health = Healthp; 
	}

	virtual void Initialise_Control(Model* Objectp) override
	{
		Object = Objectp;
		Object->Flags[MF_TAKES_DAMAGE] = true;

		Animator.Animation = Pull_Animation("Assets/Animations/Murderer_Idle.anim").Animation;
		Animator.Time = 0.0f;
		Animator.Flags[ANIMF_LOOP_BIT] = true;
	}

	virtual void Control_Function() override
	{
		Object->Flags[MF_TO_BE_DELETED] |= Health < 0.0f;
		Object->Hitboxes[0]->Flags[MF_TO_BE_DELETED] |= Health < 0.0f;

		if (Health < 0.0f)
		{
			Audio::Audio_Source* Ambient_Hit = Audio::Create_Audio_Source(glm::vec3(0.0f), 1.0f);

			Ambient_Hit->Flags[ASF_WITHOUT_POSITION] = true;
			Ambient_Hit->Flags[ASF_DELETE_ONCE_FINISHED] = true;
			Ambient_Hit->Play_Sound(Pull_Audio("Assets/Audio/Music/Ambient_Hit.wav").Source);
		}

		// Then we'll see if we can move in direction of player!

		glm::vec3 To_Player_Vector = glm::vec3(Tick, 0.0f, Tick) * glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * (Player_Camera.Position - Object->Position));

		Object->Orientation *= glm::vec3(1.0f - Tick, 0.0f, 1.0f - Tick);
		Object->Orientation += To_Player_Vector;

		Object->Orientation = glm::normalize(Object->Orientation);

		Animator.Animate_Mesh(&Object->Mesh, Tick, true);

		Object->Flags[MF_UPDATE_MESH] = true;

		// This interpolates between the vectors accordingly
	}
};

#endif
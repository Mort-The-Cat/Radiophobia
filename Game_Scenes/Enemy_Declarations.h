#ifndef RADIOPHOBIA_ENEMY_DECLARATIONS
#define RADIOPHOBIA_ENEMY_DECLARATIONS

#include "../OpenGL_Declarations.h"
#include "../Job_System.h"
#include "../Job_System_Declarations.h"
#include "../Asset_Loading_Cache.h"

#include "../Model_Declarations.h"

#include "../AI_Navigation_Grid.h"

class Test_Enemy_Controller : public Damageable_Controller
{
public:
	Mesh_Animator Animator;

	~Test_Enemy_Controller()
	{
		// When we delete this object, we want to free the animated model object

		Object->Mesh.Delete_Buffer();
	}

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

			Object->Flags[MF_UPDATE_MESH] = false;

			return;
		}

		// Then we'll see if we can move in direction of player!

		glm::vec3 To_Player_Vector = glm::vec3(1.0f, 0.0f, 1.0f) * (Player_Camera.Position - Object->Position);// glm::vec3(Tick, 0.0f, Tick)* glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * (Player_Camera.Position - Object->Position));

		float Squared_Length = glm::dot(To_Player_Vector, To_Player_Vector);

		if (Squared_Length > 6.0f)
		{
			glm::vec3 Movement_Direction = glm::vec3(Tick) * Navigation::Get_Path_Direction(Object->Position, Player_Camera.Position);

			Object->Position += Movement_Direction;
		}

		Squared_Length = Tick * Fast::Inverse_Sqrt(Squared_Length);

		To_Player_Vector.x *= Squared_Length;
		To_Player_Vector.z *= Squared_Length;
		// We know that To_Player_Vector.y == 0

		Object->Orientation *= glm::vec3(1.0f - Tick, 0.0f, 1.0f - Tick);
		Object->Orientation += To_Player_Vector;

		Object->Orientation = glm::normalize(Object->Orientation);

		Animator.Animate_Mesh(&Object->Mesh, Tick, true);

		Object->Flags[MF_UPDATE_MESH] = true;

		// This interpolates between the vectors accordingly
	}
};

#endif
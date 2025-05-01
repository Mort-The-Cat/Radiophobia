#ifndef RADIOPHOBIA_OBJECT_CONTROLLERS
#define RADIOPHOBIA_OBJECT_CONTROLLERS

#include "../Job_System_Declarations.h"
#include "../OpenGL_Declarations.h"
#include "../Model_Declarations.h"
#include "../Lighting_Handler.h"

#include "../Mesh_Animator_Declarations.h"

#include "../Audio_Handler_Declarations.h"

class Damageable_Controller : public Controller
{
public:
	enum Damage_Source
	{
		Bullet = 0,
		Blunt_Force = 1
	};

	float Health; // This is the health of the object, able to be decreased with the "damage" function

	Damageable_Controller() {}

	Damageable_Controller(float Healthp) { Health = Healthp; }

	virtual void Initialise_Control(Model* Objectp) override
	{
		Object = Objectp;
		Object->Flags[MF_TAKES_DAMAGE] = true;
	}

	virtual void Damage(float Delta, Damage_Source Type = Damage_Source::Bullet)
	{
		Health -= Delta;
	}
};

class Test_Animation_Controller : public Controller
{
	std::string Animation_Name;

public:
	Mesh_Animator Animator;

	Test_Animation_Controller(const char* Directory)
	{
		Animation_Name = Directory;
	}

	virtual void Control_Function() override
	{
		Animator.Animate_Mesh(&Object->Mesh, Tick, true);

		Object->Flags[MF_UPDATE_MESH] = true;		// Keep in mind that this flag doesn't *always* have to be set after the mesh is updated
		// 
	}
	virtual void Initialise_Control(Model* Objectp) override
	{
		Object = Objectp;

		Animator.Animation = Pull_Animation(Animation_Name.c_str()).Animation;
		Animator.Time = 0;
	}
};

class Test_Deletion_Controller : public Controller
{
public:
	float Timer = 8;
	Lightsource* Light;
	virtual void Control_Function() override
	{
		Timer -= Tick;
		Object->Flags[MF_TO_BE_DELETED] = Timer < 0; // If the timer is less than zero, the object should be deleted!
		Light->Flags[LF_TO_BE_DELETED] = Timer < 0;
	}
	virtual void Initialise_Control(Model* Objectp) override
	{
		Object = Objectp;
		Light = new Lightsource(Object->Position, glm::vec3(0.1, 0.1, 0.1), glm::vec3(0, 1, 0), 360, 3);
		Scene_Lights.push_back(Light);
	}
};

#endif
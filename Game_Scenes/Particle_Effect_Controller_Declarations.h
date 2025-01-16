#ifndef RADIOPHOBIA_PARTICLE_EFFECT_DECLARATIONS
#define RADIOPHOBIA_PARTICLE_EFFECT_DECLARATIONS

#include "..\Particle_System_Declarations.h"
#include "..\OpenGL_Declarations.h"

struct Shell_Particle
{
	glm::vec4 Position; // .w is age
	glm::vec4 Velocity; // .w is gravity/acceleration

	float E; // This is the elasticity of the collision
	float Delta, Q, Height, Height_Ratio, Floor_Position, Inverse_Delta;
};

class Pistol_Shell_Particle_Info : public Particle_Info<Shell_Particle>
{
public:

	Pistol_Shell_Particle_Info() { Particles_Per_Call = 100; }

	void Spawn_Particle(glm::vec4 Position, glm::vec4 Velocity)
	{

	}
};

#endif
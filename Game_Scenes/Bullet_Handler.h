#ifndef BULLET_HANDLER_RADIOPHOBIA
#define BULLET_HANDLER_RADIOPHOBIA

#include "Items_Declarations.h"

void Shoot_Bullet_Function(Hitbox* Shooter_Hitbox, glm::vec3 Position, glm::vec3 Orientation, float Damage)
{
	Hitbox* Collided_Hitbox;

	auto Not_Against_Shooter_Compare{ [Shooter_Hitbox](Hitbox* A, Hitbox* B)->bool {	return B != Shooter_Hitbox;	} };

	Collision_Info Info = Collision_Test::Raycast(Position, glm::vec3(0.02f) * Orientation, 500, Not_Against_Shooter_Compare, &Collided_Hitbox, 0.04f);

	if (Collided_Hitbox == nullptr) // Out of range, we don't do anything!
		return;

	if (Collided_Hitbox->Object->Flags[MF_TAKES_DAMAGE]) // If the object takes damage
	{
		Damageable_Controller* Controller = (Damageable_Controller*)Collided_Hitbox->Object->Control;

		Controller->Damage(Damage);
		// we deal damage to it!!
	}
	else if (Collided_Hitbox->Object->Flags[MF_PHYSICS_TEST])
	{
		Physics_Object_Controller* Control = (Physics_Object_Controller*)Collided_Hitbox->Object->Control;

		Control->Time -= 10;
	}

	// in any case, we wanna spawn some decals and particle effects!

	irrklang::ISoundSource* Sound; // This is set in the following switch statement

	const char* Metal_Audio_Directories[] =
	{
		"Assets/Audio/Impact/Metal_Impact_0.wav",
		"Assets/Audio/Impact/Metal_Impact_1.wav",
		"Assets/Audio/Impact/Metal_Impact_2.wav"
	};

	const char* Wood_Audio_Directories[] =
	{
		"Assets/Audio/Impact/Wood_0.wav",
		"Assets/Audio/Impact/Wood_1.wav",
		"Assets/Audio/IMpact/Wood_2.wav"
	};

	const char* Barrel_Audio_Directories[] =
	{
		"Assets/Audio/Impact/Barrel_0.wav",
		"Assets/Audio/Impact/Barrel_1.wav",
		"Assets/Audio/Impact/Barrel_2.wav"
	};

	const char* Wound_Audio_Directories[] =
	{
		"Assets/Audio/Impact/Bullet_Wound_1.wav",
		"Assets/Audio/Impact/Bullet_Wound_2.wav",
		"Assets/Audio/Impact/Bullet_Wound_3.wav"
	};

	switch (Collided_Hitbox->Object->Material_Flag)
	{

	case Object_Material::Barrel:
		Sound = Pull_Audio(Barrel_Audio_Directories[((size_t)rand()) % 3u]).Source;

		if (Collided_Hitbox->Object->Flags[MF_USE_DECALS])
			Create_Bullet_Decal(Collided_Hitbox->Object->Mesh.Mesh, Info.Collision_Position, Info.Collision_Normal, 0.025f);

		for (size_t W = 0; W < 20; W++)
		{
			glm::vec3 Direction = glm::reflect(Orientation, Info.Collision_Normal) + glm::vec3(1.5f) * glm::vec3(RNG() - 0.5f, RNG() - 0.75f, RNG() - 0.5f);
			glm::vec3 Position = Info.Collision_Position + glm::vec3(0.05f) * glm::vec3(RNG() - 0.5f, RNG() - 0.5f, RNG() - 0.5f);

			for (size_t V = 0; V < 1; V++)
				Spark_Particles.Particles.Spawn_Particle(Position, Direction);
		}

		break;

	case Object_Material::Metal:

		Sound = Pull_Audio(Metal_Audio_Directories[((size_t)rand()) % 3u]).Source;

		if (Collided_Hitbox->Object->Flags[MF_USE_DECALS])
			Create_Bullet_Decal(Collided_Hitbox->Object->Mesh.Mesh, Info.Collision_Position, Info.Collision_Normal, 0.025f);

		for (size_t W = 0; W < 20; W++)
		{
			glm::vec3 Direction = glm::reflect(Orientation, Info.Collision_Normal) + glm::vec3(1.5f) * glm::vec3(RNG() - 0.5f, RNG() - 0.75f, RNG() - 0.5f);
			glm::vec3 Position = Info.Collision_Position + glm::vec3(0.05f) * glm::vec3(RNG() - 0.5f, RNG() - 0.5f, RNG() - 0.5f);

			for (size_t V = 0; V < 1; V++)
				Spark_Particles.Particles.Spawn_Particle(Position, Direction);
		}
		break;

		//

	case Object_Material::Enemy:

		Sound = Pull_Audio(Wound_Audio_Directories[((size_t)rand()) % 3u]).Source;

		for (size_t W = 0; W < 20; W++)
		{
			glm::vec3 Direction = glm::reflect(Orientation, Info.Collision_Normal) + glm::vec3(1.5f) * glm::vec3(RNG() - 0.5f, RNG() - 0.75f, RNG() - 0.5f);
			glm::vec3 Position = Info.Collision_Position + glm::vec3(0.05f) * glm::vec3(RNG() - 0.5f, RNG() - 0.5f, RNG() - 0.5f);

			for (size_t V = 0; V < 1; V++)
				Spark_Particles.Particles.Spawn_Particle(Position, Direction);
		}

		break;

	case Object_Material::Stone:
	case Object_Material::Floor_Tiles:
	case Object_Material::Concrete:
	default:
		Sound = Pull_Audio("Assets/Audio/Impact/Concrete_Impact.wav").Source;

		if (Collided_Hitbox->Object->Flags[MF_USE_DECALS])
			Create_Bullet_Decal(Collided_Hitbox->Object->Mesh.Mesh, Info.Collision_Position, Info.Collision_Normal, 0.025f);

		break;
		//

	case Object_Material::Wood:
		Sound = Pull_Audio(Wood_Audio_Directories[((size_t)rand()) % 3u]).Source;

		if (Collided_Hitbox->Object->Flags[MF_USE_DECALS])
			Create_Bullet_Decal(Collided_Hitbox->Object->Mesh.Mesh, Info.Collision_Position, Info.Collision_Normal, 0.025f);

		break;
	}

	Audio::Audio_Source* Impact_Sound_Source = Audio::Create_Audio_Source(Info.Collision_Position, 2.5f);
	Impact_Sound_Source->Play_Sound(Sound);
	Impact_Sound_Source->Flags[ASF_DELETE_ONCE_FINISHED];

	// I'll need to work on the bullet decals and suchs

	// for(size_t W = 0; W < 10; W++)
	// Vent_Smoke_Particles.Particles.Spawn_Particle(Info.Collision_Position, -Info.Collision_Normal);
}

#endif
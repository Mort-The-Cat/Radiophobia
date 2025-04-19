#ifndef VISION_AUDIO_DECLARATIONS
#define VISION_AUDIO_DECLARATIONS

#include "irrKlang.h"
#include "ik_vec3d.h"

#include "glm/glm.hpp"

#pragma comment(lib, "irrKlang.lib")

#define ASF_TO_BE_DELETED 0u
#define ASF_WITHOUT_POSITION 1u

#define ASF_DELETE_ONCE_FINISHED 2u

irrklang::ISoundEngine* Sound_Engine;

irrklang::ISound* Music;

float Sound_Effect_Volume = 0.5f;
float Music_Volume = 0.5f;

// irrklang::ISoundSource* Bump_Sound_Effect_Source;

const glm::vec3 Audio_Position_Multiplier = glm::vec3(0.25);

glm::vec3 Current_Listener_Right_Ear_Vector;

irrklang::vec3df Get_Klang_Vector(glm::vec3 Vector)
{
	return { Vector.x, Vector.y, Vector.z };
}

void Initialise_Sound_Engine()
{
	Sound_Engine = irrklang::createIrrKlangDevice();

	Sound_Engine->setSoundVolume(0.5f);

	//Sound_Engine->play2D(Pull_Audio("Assets/Audio/UI/Beep.wav").Source)->drop();
}

void Destroy_Sound_Engine()
{
	Sound_Engine->drop();
	Sound_Engine = nullptr;
}

#endif

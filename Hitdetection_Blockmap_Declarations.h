#ifndef VISION_HITDETECTION_BLOCKMAP_DECLARATIONS
#define VISION_HITDETECTION_BLOCKMAP_DECLARATIONS

#include "OpenGL_Declarations.h"

constexpr const bool Should_Use_Blockmap = true;

class Hitbox;

struct Blockmap_Bounds
{
	glm::vec2 A, B;
};

namespace Blockmap
{
	std::vector<Hitbox*> Read_Blockmap(const glm::vec3 Reader_Position);

	void Initialise_Blockmap();
}

#endif
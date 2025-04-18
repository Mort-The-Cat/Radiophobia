#ifndef LIGHTING_HANDLER_OPENGL
#define LIGHTING_HANDLER_OPENGL

#include "OpenGL_Declarations.h"

#include "Shadow_Map_Renderer_Declarations.h"

namespace Lighting_BVH
{
	void Parse_Partition_Nodes_To_Shader(Shader& Shader);
	size_t Get_BVH_Node(float X, float Z);
}

struct Light_Uniform_Location_Object
{
	int Position;
	int Colour;
	int Direction;

	int Cubemap;

} Light_Uniform_Location;

Light_Uniform_Location_Object Initialise_Light_Uniform_Locations_Object(Shader Shader)
{
	Light_Uniform_Location_Object Location;
	Location.Position = glGetUniformLocation(Shader.Program_ID, "Light_Position");
	Location.Colour = glGetUniformLocation(Shader.Program_ID, "Light_Colour");
	Location.Direction = glGetUniformLocation(Shader.Program_ID, "Light_Direction");

	Location.Cubemap = glGetUniformLocation(Shader.Program_ID, "Cubemap");

	return Location;
}

#define NUMBER_OF_LIGHTS 32

class Light_Uniform_Buffer // We'll update this and implement any BVH code later on. Until then, we don't need any more
{
public:
	std::array<glm::vec4, NUMBER_OF_LIGHTS> Position;
	std::array<glm::vec4, NUMBER_OF_LIGHTS> Colour;
	std::array<glm::vec4, NUMBER_OF_LIGHTS> Direction;

	void Update_Buffer(Light_Uniform_Location_Object Location)
	{
		glUniform4fv(Location.Position, NUMBER_OF_LIGHTS, glm::value_ptr(Position[0]));
		glUniform4fv(Location.Colour, NUMBER_OF_LIGHTS, glm::value_ptr(Colour[0]));
		glUniform4fv(Location.Direction, NUMBER_OF_LIGHTS, glm::value_ptr(Direction[0]));
	}
} Light_Uniforms;

#define LF_TO_BE_DELETED 0u
#define LF_CAST_SHADOWS 1u
#define LF_EXCLUDE_FROM_BVH 2u // This flag is set if the lights can be ignored entirely from the Light BVH, meaning other light sources can be prioritised
#define LF_TIMER 3u
#define LF_PRIORITY 4u

enum LG // Light_Group
{
	No_Group_Sector = 0xFFu, // This is the "no group" flag for the sectors, we need separate ones because of how the checking code works
	No_Group = 0,
	Group_One = 1,
	Group_Two = 2,
	Group_Three = 3,
	Group_Four = 4,
	Group_Five = 5,
	Group_Six = 6,
	Group_Seven = 7
};

class Lightsource
{
public:
	glm::vec3 Position;
	glm::vec3 Colour;
	glm::vec3 Direction; 

	float FOV;
	float Blur;

	float Attenuation; // This is important for things like fire vs things like ordinary lights- it'd be strange if your viewmodel's colour was overexposed the whole game usw.

	float Timer; // This doesn't necessarily apply

	bool Flags[5] = { false, false, false, false, false };

	LG Light_Group = LG::No_Group;

	Lightsource() {}

	Lightsource(glm::vec3 Positionp, glm::vec3 Colourp, glm::vec3 Directionp, float FOVp = 360, float Blurp = 1.0f, float Attenuationp = 0.6f, LG Light_Groupp = LG::No_Group)
	{
		Position = Positionp;
		Colour = Colourp;
		Direction = Directionp;
		FOV = FOVp;
		Blur = Blurp;
		Attenuation = Attenuationp;

		Light_Group = Light_Groupp;
	}
};

std::vector<Lightsource*> Scene_Lights;

void Rearrange_Light_Priority()
{
	size_t Counter = 0u;

	for (size_t W = 0; W < Scene_Lights.size(); W++)
		if (Scene_Lights[W]->Flags[LF_CAST_SHADOWS])
		{
			std::swap(Scene_Lights[W], Scene_Lights[Counter]);

			Counter++;
		}
}

namespace Lighting_BVH
{
	void Update_Leaf_Node_Data();
	void Generate_Light_BVH_Tree();
}

void Update_Lighting_Buffer()
{
	Rearrange_Light_Priority();

	size_t Max_Index = std::min((int)Scene_Lights.size(), NUMBER_OF_LIGHTS);
	for (size_t W = 0; W < Max_Index; W++)
	{
		Light_Uniforms.Position[W].x = Scene_Lights[W]->Position.x;
		Light_Uniforms.Position[W].y = Scene_Lights[W]->Position.y;
		Light_Uniforms.Position[W].z = Scene_Lights[W]->Position.z;
		Light_Uniforms.Position[W].w = 1.0f / Scene_Lights[W]->Blur;		// Blur value

		Light_Uniforms.Colour[W].r = Scene_Lights[W]->Colour.r;
		Light_Uniforms.Colour[W].g = Scene_Lights[W]->Colour.g;
		Light_Uniforms.Colour[W].b = Scene_Lights[W]->Colour.b;
		Light_Uniforms.Colour[W].w = (Scene_Lights[W]->Attenuation);		// Attenuation value

		Light_Uniforms.Direction[W].x = Scene_Lights[W]->Direction.x;
		Light_Uniforms.Direction[W].y = Scene_Lights[W]->Direction.y;
		Light_Uniforms.Direction[W].z = Scene_Lights[W]->Direction.z;
		Light_Uniforms.Direction[W].w = Scene_Lights[W]->FOV;
	}

	for (size_t W = Max_Index; W < NUMBER_OF_LIGHTS; W++)
	{
		Light_Uniforms.Position[W] = glm::vec4(0, 0, 0, 0);
		Light_Uniforms.Colour[W] = glm::vec4(0, 0, 0, 0);
		Light_Uniforms.Direction[W] = glm::vec4(0, 0, 0, 0);
	}

	if (Shadow_Mapper::Shadow_Mapping)
	{
		Max_Index = std::min((unsigned int)Scene_Lights.size(), NUMBER_OF_SHADOW_MAPS);
		for (size_t W = 0; W < Max_Index; W++)
			Shadow_Mapper::Shadow_Maps[W].Update_View_Matrices(Scene_Lights[W]->Position);
	}

	Light_Uniforms.Update_Buffer(Light_Uniform_Location);

	// Lighting_BVH::Generate_Light_BVH_Tree();

	Lighting_BVH::Update_Leaf_Node_Data();
}

#endif
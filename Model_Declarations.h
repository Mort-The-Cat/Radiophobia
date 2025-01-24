#ifndef MODEL_DECLARATIONS_VISIONGL
#define MODEL_DECLARATIONS_VISIONGL

#include "OpenGL_Declarations.h"
#include "Vertex_Buffer_Declarations.h"
#include "Uniform_Buffer_Declarations.h"
#include "Texture_Declarations.h"
#include "Job_System_Declarations.h"

#include "Mesh_Animator_Declarations.h"

#include "Hitdetection.h"

#define MF_TO_BE_DELETED 0
#define MF_ACTIVE 1
#define MF_SOLID 2
#define MF_PHYSICS_TEST 3

#define MF_UPDATE_MESH 4

#define MF_CAST_SHADOWS 5

#define MF_TAKES_DAMAGE 6

#define MF_USE_DECALS 7

glm::mat4 Direction_Matrix_Calculate(glm::vec3 Position, glm::vec3 Forward_Vector, glm::vec3 Up_Vector)
{
	glm::vec3 Forward = Forward_Vector;
	glm::vec3 Right = glm::cross(Forward_Vector, Up_Vector);
	glm::vec3 Up = glm::cross(Forward_Vector, Right);

	return glm::mat4(
		Right.x, Right.y, Right.z, 0.0f,
		Up.x, Up.y, Up.z, 0.0f,
		Forward.x, Forward.y, Forward.z, 0.0f,
		Position.x, Position.y, Position.z, 1.0f);
}

enum Object_Material
{
	Metal = 0,			// sparks when shot at grazing angle

	Wood = 1,			// splinters

	Wall = 2,			// decal reveals brick behind wall

	Floor_Tiles = 3,	// 

	Stone = 4,			// kicks up rubble and extra dust
	Electronics = 5,	// metal sound effect but has zaps as well

	Enemy = 6,

	Glass = 7,

	Concrete = 8,		// This is the concrete-like wall

	Barrel = 9			// Has slightly different metallic sound

	// Cushioned = 7		// Murderer's bed etc
};

class Model
{
public:
	Model_Vertex_Buffer Mesh;
	Model_Uniform_Buffer Uniforms;

	Texture Albedo;
	Texture Material;

	glm::vec3 Position;
	//glm::vec3 Orientation_Old;

	glm::vec3 Orientation_Up = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, 1.0f);

	Controller* Control;

	std::vector<Hitbox*> Hitboxes;

	bool Flags[8] = { false, false, false, false, false, false, false, false }; // Doesn't really matter how many bits we use for this

	Object_Material Material_Flag = Object_Material::Concrete;

	Model() {}

	Model(std::vector<size_t> Flagsp, Object_Material Material_Flagp) 
	{ 
		for (size_t W = 0; W < Flagsp.size(); W++)
			Flags[Flagsp[W]] = true;

		Material_Flag = Material_Flagp;
	}

	~Model()
	{
		if (Mesh.Buffer_Storage_Hint == GL_DYNAMIC_DRAW)
			Mesh.Delete_Buffer();
		delete Control;
	}

	void Render(Shader Shader)
	{
		Mesh.Bind_Buffer();

		//if (Mesh.Buffer_Storage_Hint == GL_DYNAMIC_DRAW)
		//	Mesh.Update_Vertices();

		Uniforms.Model_Matrix = Direction_Matrix_Calculate(Position, Orientation, Orientation_Up);

		Uniforms.Model_Position = Position;

		Uniforms.Update_Buffer(Model_Uniform_Location);

		Albedo.Parse_Texture(Shader, "Albedo", 0);
		Albedo.Bind_Texture();
		Material.Parse_Texture(Shader, "Material", 1);
		Material.Bind_Texture();

		glDrawElements(GL_TRIANGLES, Mesh.Indices_Count, GL_UNSIGNED_INT, 0);
	}
};

glm::mat4 Hitbox_Direction_Matrix_Calculate(Model* Model)
{
	return Direction_Matrix_Calculate(glm::vec3(0, 0, 0), Model->Orientation, Model->Orientation_Up);
}

void Create_Model(Model_Vertex_Buffer Mesh, Texture Albedo, Texture Material, Model* Target_Model, Controller* Controlp, std::vector<Hitbox*> Hitboxp);

std::vector<Model*> Scene_Models;

#endif
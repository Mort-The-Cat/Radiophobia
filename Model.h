#ifndef MODEL_VISIONGL
#define MODEL_VISIONGL

#include "Model_Declarations.h"
#include "Physics_Engine.h"

Model::~Model()
{
		if (Mesh.Buffer_Storage_Hint == GL_DYNAMIC_DRAW)
			Mesh.Delete_Buffer();

		if (Flags[MF_PHYSICS_TEST])
			((Physics_Object_Controller*)Control)->Physics_Info->Flags[0u] = true; // PF_TO_BE_DELETED

		for (size_t W = 0; W < Hitboxes.size(); W++)
			Hitboxes[W]->Flags[HF_TO_BE_DELETED] = true;

		delete Control;
}

void Create_Model(Model_Vertex_Buffer Mesh, Texture Albedo, Texture Material, Model* Target_Model, Controller* Controlp, std::vector<Hitbox*> Hitboxp)
{
	Target_Model->Mesh = Mesh;
	Target_Model->Albedo = Albedo;
	Target_Model->Material = Material;
	Target_Model->Control = Controlp;
	Target_Model->Hitboxes = Hitboxp;

	Controlp->Initialise_Control(Target_Model);

	if (Target_Model->Flags[MF_SOLID] && Target_Model->Hitboxes.size())
	{
		for (size_t W = 0; W < Target_Model->Hitboxes.size(); W++)
		{
			Target_Model->Hitboxes[W]->Position = &Target_Model->Position;
			Target_Model->Hitboxes[W]->Object = Target_Model;

			Scene_Hitboxes.push_back(Target_Model->Hitboxes[W]);
			if (Target_Model->Flags[MF_PHYSICS_TEST] && Scene_Hitboxes.size() > Physics::Scene_Physics_Objects.size()) // If it's a physics object, we need it at the *front* of the vector
				std::swap(Scene_Hitboxes[Physics::Scene_Physics_Objects.size() - 1], Scene_Hitboxes.back());
		}
	}
}

#endif

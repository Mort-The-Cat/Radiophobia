#ifndef HITDETECTION_DECLARATIONS_VISIONGL
#define HITDETECTION_DECLARATIONS_VISIONGL

#include "OpenGL_Declarations.h"
#include "Model_Declarations.h"

#include "Hitdetection_Blockmap_Declarations.h"

class Collision_Info
{
public:
	glm::vec3 Collision_Position = { 0, 0, 0 };
	glm::vec3 Collision_Normal = { 0, 0, 0 };
	float Overlap = 0;

	Collision_Info() {}
	Collision_Info(glm::vec3 Collision_Positionp, glm::vec3 Collision_Normalp, float Overlapp)
	{
		Collision_Position = Collision_Positionp;
		Collision_Normal = Collision_Normalp;
		Overlap = Overlapp;
	}
};

Collision_Info Inverse_Collision(Collision_Info Info)
{
	Info.Collision_Normal *= -1;

	return Info;
}

class AABB_Hitbox;
class Sphere_Hitbox;
class Mesh_Hitbox;

#define HF_TO_BE_DELETED 0u
#define HF_IN_BLOCKMAP 1u

class Hitbox
{
public:
	glm::vec3* Position;
	Model* Object;

	bool Flags[2] = { false, false };

	Hitbox() {}
	Hitbox(glm::vec3* Positionp) { Position = Positionp; }

	virtual Collision_Info Hitdetection(Hitbox* Other) { return Collision_Info(); }

	virtual Collision_Info AABB_Hitdetection(AABB_Hitbox* Other) { return Collision_Info(); }
	virtual Collision_Info Sphere_Hitdetection(Sphere_Hitbox* Other) { return Collision_Info(); }
	virtual Collision_Info Mesh_Hitdetection(Mesh_Hitbox* Other) { return Collision_Info(); }

	virtual Blockmap_Bounds Get_Bounds() { return { glm::vec3(0.0f), glm::vec3(0.0f) }; };

	virtual void Update_Hitbox() {  }
};

class AABB_Hitbox : public Hitbox
{
public:
	glm::vec3 A = glm::vec3(999,999,999), B = glm::vec3(-999,-999,-999);

	bool operator==(AABB_Hitbox* Other)
	{
		return A == Other->A && B == Other->B;
	}

	virtual Collision_Info Hitdetection(Hitbox* Other) override
	{
		return Other->AABB_Hitdetection(this);
	}

	virtual Collision_Info AABB_Hitdetection(AABB_Hitbox* Other) override;
	virtual Collision_Info Sphere_Hitdetection(Sphere_Hitbox* Other) override;
	virtual Collision_Info Mesh_Hitdetection(Mesh_Hitbox* Other) override;

	virtual Blockmap_Bounds Get_Bounds() override
	{
		Blockmap_Bounds Bounds;
		
		Bounds.A = glm::vec2(A.x, A.z) + glm::vec2(Position->x, Position->z);
		Bounds.B = glm::vec2(B.x, B.z) + glm::vec2(Position->x, Position->z);

		return Bounds;
	}
};

class Sphere_Hitbox : public Hitbox
{
public:
	float Radius;

	virtual Collision_Info Hitdetection(Hitbox* Other) override
	{ 
		return Other->Sphere_Hitdetection(this); 
	}

	virtual Collision_Info AABB_Hitdetection(AABB_Hitbox* Other) override;
	virtual Collision_Info Sphere_Hitdetection(Sphere_Hitbox* Other) override;
	virtual Collision_Info Mesh_Hitdetection(Mesh_Hitbox* Other) override;

	virtual Blockmap_Bounds Get_Bounds() override
	{
		Blockmap_Bounds Bounds;

		Bounds.A = glm::vec2(Position->x - Radius, Position->z - Radius);
		Bounds.B = glm::vec2(Position->x + Radius, Position->z + Radius);

		return Bounds;
	}
};

glm::mat4 Hitbox_Direction_Matrix_Calculate(Model* Object);

class Mesh_Hitbox : public Hitbox
{
public:
	std::vector<glm::vec3> Vertices;
	std::vector<uint32_t> Indices; // These are the indices for each tri !

	std::vector<glm::vec3> Transformed_Vertices; // These are the vertices that have been transformed with the rotation matrices and the object's hitbox's position

	glm::mat3 Matrix_Rotation = glm::mat3(1.0f); // This is the matrix rotation applied to the vertices during hit detection- perhaps I can initialise this from the control function

	Mesh_Hitbox() {}

	virtual void Update_Hitbox() override
	{
		Matrix_Rotation = Hitbox_Direction_Matrix_Calculate(Object);

		for (size_t W = 0; W < Vertices.size(); W++)
			Transformed_Vertices[W] = Matrix_Rotation * Vertices[W];
	}

	virtual Collision_Info Hitdetection(Hitbox* Other) override
	{
		return Other->Mesh_Hitdetection(this);
	}

	virtual Collision_Info Mesh_Hitdetection(Mesh_Hitbox* Other) override;
	virtual Collision_Info Sphere_Hitdetection(Sphere_Hitbox* Other) override;
	virtual Collision_Info AABB_Hitdetection(AABB_Hitbox* Other) override;

	virtual Blockmap_Bounds Get_Bounds() override
	{
		Blockmap_Bounds Bounds;
		Bounds.A = glm::vec2(Position->x + Vertices[0].x, Position->z + Vertices[0].z);
		Bounds.B = Bounds.A;

		for (size_t W = 1; W < Vertices.size(); W++)
		{
			Bounds.A.x = MIN(Bounds.A.x, Vertices[W].x + Position->x);
			Bounds.A.y = MIN(Bounds.A.y, Vertices[W].z + Position->z);

			Bounds.B.x = MAX(Bounds.B.x, Vertices[W].x + Position->x);
			Bounds.B.y = MAX(Bounds.B.y, Vertices[W].z + Position->z);
		}

		return Bounds;
	}

	bool Normal_Already_Included(glm::vec3 Normal)
	{
		for (size_t W = 0; W < Indices.size(); W += 3)
		{
			glm::vec3 Calculated_Normal = Calculate_Surface_Normal(Vertices[Indices[W]], Vertices[Indices[W + 1]], Vertices[Indices[W + 2]]);

			if (Normal == Calculated_Normal)
				return true;
		}

		return false;
	}
};

std::vector<Hitbox*> Scene_Hitboxes; // The first n objects (where n = Scene_Physics_Objects.size()) are always physics objects

void Remove_Duplicate_AABB_Hitbox(std::vector<Hitbox*>* Hitboxes)
{
	AABB_Hitbox* Most_Recent = reinterpret_cast<AABB_Hitbox*>(Hitboxes->back());

	for (size_t W = 0; W < Hitboxes->size() - 1; W++)
	{
		if (*Most_Recent == reinterpret_cast<AABB_Hitbox*>(Hitboxes->at(W)))
		{
			delete Most_Recent;
			Hitboxes->pop_back();

			return;
		}

		// We don't have to do anything!
	}
}

bool Check_Greedy_Hitboxing(AABB_Hitbox* A, AABB_Hitbox* B)
{
	bool X_Match = A->A.x == B->A.x && A->B.x == B->B.x,
		Y_Match = A->A.y == B->A.y && A->B.y == B->B.y,
		Z_Match = A->A.z == B->A.z && A->B.z == B->B.z;

	return
		(X_Match && Y_Match && (A->A.z == B->B.z)) ||
		(Y_Match && Z_Match && (A->A.x == B->B.x)) ||
		(Z_Match && X_Match && (A->A.y == B->B.y));
}

std::vector<Hitbox*> Apply_Greedy_Hitboxing(std::vector<Hitbox*> Hitboxes) // Doesn't have to be fast
{
	// Apply Greedy-Hitboxing to list

	std::vector<Hitbox*> New_List = Hitboxes;
	bool List_Modified;

	do
	{
		List_Modified = false;

		for (size_t Index = 0; Index < New_List.size(); Index++)
		{
			for (size_t W = Index + 1; W < New_List.size(); W++)
			{
				// If we get a hit, update list and break from these loops to iterate through again

				if (Check_Greedy_Hitboxing(reinterpret_cast<AABB_Hitbox*>(New_List[Index]), reinterpret_cast<AABB_Hitbox*>(New_List[W])) || 
					Check_Greedy_Hitboxing(reinterpret_cast<AABB_Hitbox*>(New_List[W]), reinterpret_cast<AABB_Hitbox*>(New_List[Index])))
				{
					// handle it

					reinterpret_cast<AABB_Hitbox*>(New_List[Index])->A = 
						glm::min(
							reinterpret_cast<AABB_Hitbox*>(New_List[Index])->A, 
							reinterpret_cast<AABB_Hitbox*>(New_List[W])->A);

					reinterpret_cast<AABB_Hitbox*>(New_List[Index])->B =
						glm::max(
							reinterpret_cast<AABB_Hitbox*>(New_List[Index])->B,
							reinterpret_cast<AABB_Hitbox*>(New_List[W])->B);

					delete New_List[W];
					New_List[W] = nullptr;
					New_List.erase(
						std::remove_if(New_List.begin(), New_List.end(), [&](Hitbox* H)->bool { return H == nullptr; }),
						New_List.end());

					Index = New_List.size();
					List_Modified = true;
					break;
				}
			}
		}
	} while (List_Modified);

	return New_List;
}

std::vector<Hitbox*> Wrap_AABB_Hitboxes(Model_Mesh& Mesh, bool Greedy_Hitboxing = true)
{
	std::vector<Hitbox*> Hitboxes;

	std::array<glm::vec3, 6> Points;

	for (size_t W = 0; W < Mesh.Indices.size(); W += 3)
	{
		for (size_t V = 0; V < 3; V++)
		{
			Model_Vertex Vertex = Mesh.Vertices[Mesh.Indices[W + V]];
			Points[V] = Vertex.Position;
			Points[V + 3] = Vertex.Position - glm::vec3(0.0625f) * Vertex.Normal;
		}

		AABB_Hitbox* Box = new AABB_Hitbox();

		for (size_t V = 0; V < 6; V++)
		{
			Box->A.x = std::fminf(Box->A.x, Points[V].x);
			Box->A.y = std::fminf(Box->A.y, Points[V].y);
			Box->A.z = std::fminf(Box->A.z, Points[V].z);

			Box->B.x = std::fmaxf(Box->B.x, Points[V].x);
			Box->B.y = std::fmaxf(Box->B.y, Points[V].y);
			Box->B.z = std::fmaxf(Box->B.z, Points[V].z);
		}

		Hitboxes.push_back(Box);

		Remove_Duplicate_AABB_Hitbox(&Hitboxes);
	}

	if(Greedy_Hitboxing)
		return Apply_Greedy_Hitboxing(Hitboxes);
	else
		return Hitboxes;
}

std::vector<Hitbox*> Generate_AABB_Hitboxes(std::string File_Name)
{
	std::vector<Hitbox*> Return;

	// This will load the file of the hitbox

	std::ifstream File(File_Name);

	if (File.is_open())
	{
		std::string Line;

		size_t Count = 0u;

		while (std::getline(File, Line))
		{
			std::stringstream Buffer;

			Buffer << Line;

			std::string Prefix;

			Buffer >> Prefix;

			if (Prefix == "o" || Prefix == "g" ||
				(Count > 8)) // This signals that a new hitbox object needs to be created
			{
				Return.push_back(new AABB_Hitbox());

				Count = 0;
			}

			if (Prefix == "v") // This will tell the engine to evaluate and add the positions of this object's vertices to the hitbox
			{
				glm::vec3 Vertex;

				Buffer >> Vertex.x >> Vertex.y >> Vertex.z;

				Vertex.y *= -1;

				//

				((AABB_Hitbox*)Return.back())->A.x = std::fminf(reinterpret_cast<AABB_Hitbox*>(Return.back())->A.x, Vertex.x);
				((AABB_Hitbox*)Return.back())->A.y = std::fminf(reinterpret_cast<AABB_Hitbox*>(Return.back())->A.y, Vertex.y);
				((AABB_Hitbox*)Return.back())->A.z = std::fminf(reinterpret_cast<AABB_Hitbox*>(Return.back())->A.z, Vertex.z);

				((AABB_Hitbox*)Return.back())->B.x = std::fmaxf(reinterpret_cast<AABB_Hitbox*>(Return.back())->B.x, Vertex.x);
				((AABB_Hitbox*)Return.back())->B.y = std::fmaxf(reinterpret_cast<AABB_Hitbox*>(Return.back())->B.y, Vertex.y);
				((AABB_Hitbox*)Return.back())->B.z = std::fmaxf(reinterpret_cast<AABB_Hitbox*>(Return.back())->B.z, Vertex.z);

				Count++;
			}
		}

		File.close();

		return Return;
	}
	else
		Throw_Error("Unable to open hitbox file!\n");
}

/*std::vector<Hitbox*> Generate_AABB_Hitboxes(Model_Mesh& Mesh)
{
	std::vector<Hitbox*> Return;

	for (size_t Cube = 0; Cube < Mesh.Vertices.size(); Cube += 8) // Cubes have 8 vertices xd I'm silly
	{
		Return.push_back(new AABB_Hitbox());

		for (size_t W = Cube; W < Cube + 8; W++)
		{
			((AABB_Hitbox*)Return.back())->A.x = std::fminf(((AABB_Hitbox*)Return.back())->A.x, Mesh.Vertices[W].Position.x);
			((AABB_Hitbox*)Return.back())->A.y = std::fminf(((AABB_Hitbox*)Return.back())->A.y, Mesh.Vertices[W].Position.y);
			((AABB_Hitbox*)Return.back())->A.z = std::fminf(((AABB_Hitbox*)Return.back())->A.z, Mesh.Vertices[W].Position.z);

			((AABB_Hitbox*)Return.back())->B.x = std::fmaxf(((AABB_Hitbox*)Return.back())->B.x, Mesh.Vertices[W].Position.x);
			((AABB_Hitbox*)Return.back())->B.y = std::fmaxf(((AABB_Hitbox*)Return.back())->B.y, Mesh.Vertices[W].Position.y);
			((AABB_Hitbox*)Return.back())->B.z = std::fmaxf(((AABB_Hitbox*)Return.back())->B.z, Mesh.Vertices[W].Position.z);
		}
	}

	return Return;
}*/



AABB_Hitbox* Generate_AABB_Hitbox(Model_Mesh& Mesh)
{
	AABB_Hitbox* Return = new AABB_Hitbox();
	for (size_t W = 0; W < Mesh.Vertices.size(); W++)
	{
		Return->A.x = std::fminf(Return->A.x, Mesh.Vertices[W].Position.x);
		Return->A.y = std::fminf(Return->A.y, Mesh.Vertices[W].Position.y);
		Return->A.z = std::fminf(Return->A.z, Mesh.Vertices[W].Position.z);

		Return->B.x = std::fmaxf(Return->B.x, Mesh.Vertices[W].Position.x);
		Return->B.y = std::fmaxf(Return->B.y, Mesh.Vertices[W].Position.y);
		Return->B.z = std::fmaxf(Return->B.z, Mesh.Vertices[W].Position.z);
	}

	return Return;
}

Sphere_Hitbox* Generate_Sphere_Hitbox(Model_Mesh& Mesh)
{
	Sphere_Hitbox* Return = new Sphere_Hitbox();
	for (size_t W = 0; W < Mesh.Vertices.size(); W++)
	{
		Return->Radius = std::fmaxf(Return->Radius, glm::dot(Mesh.Vertices[W].Position, Mesh.Vertices[W].Position));
	}

	Return->Radius = sqrtf(Return->Radius);

	return Return;
}

Mesh_Hitbox* Generate_Mesh_Hitbox(Model_Mesh& Mesh)
{
	Mesh_Hitbox* Hitbox = new Mesh_Hitbox();

	// Get indices for all of the edges in the mesh

	// For this, it'll be good to calculate all of the 

	Hitbox->Vertices.resize(Mesh.Vertices.size());

	//Hitbox->Indices = Mesh.Indices; // We can copy this 1-1

	for (size_t W = 0; W < Mesh.Vertices.size(); W++)
		Hitbox->Vertices[W] = Mesh.Vertices[W].Position;

	for (size_t W = 0; W < Mesh.Indices.size(); W += 3) // We're dealing with triangles so that's how we'll iterate
	{
		glm::vec3 Normal = Calculate_Surface_Normal(Mesh.Vertices[Mesh.Indices[W]].Position, Mesh.Vertices[Mesh.Indices[W + 1]].Position, Mesh.Vertices[Mesh.Indices[W + 2]].Position);

		if (!Hitbox->Normal_Already_Included(Normal))
		{
			Hitbox->Indices.reserve(Hitbox->Indices.size() + 3);

			for (char V = 0; V < 3; V++)
				Hitbox->Indices.push_back(Mesh.Indices[W + V]);
		}

		// We check if we've included this normal in the model before!

	}

	// I believe that makes it a success!!

	/*printf("\n\n");

	for (size_t W = 0; W < Hitbox->Vertices.size(); W++)
		printf("glm::vec3(%sX, %sY, %sZ),\n",
			Hitbox->Vertices[W].x > 0 ? "Max" : "Min",
			Hitbox->Vertices[W].y > 0 ? "Max" : "Min",
			Hitbox->Vertices[W].z > 0 ? "Max" : "Min");*/

	Hitbox->Matrix_Rotation = glm::mat3(1.0f);

	Hitbox->Transformed_Vertices = Hitbox->Vertices;

	return Hitbox;
}

#endif
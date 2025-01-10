#ifndef HITDETECTION_VISIONGL
#define HITDETECTION_VISIONGL

#include "Hitdetection_Declarations.h"
#include <stdexcept>
#include <array>

#define DEBUGGING_HITDETECTION 1u

struct Separating_Axis_Theorem_Info
{
	float Delta;
	uint32_t Indices_Index;
	Mesh_Hitbox* Mesh_Hitbox;
	glm::vec3 Normal;

	glm::vec3 Average_Collision_Position;
	size_t Number_Of_Collision_Points;
};

namespace Collision_Test
{
	void Sphere_Separating_Axis_Theorem(Sphere_Hitbox& Sphere, Mesh_Hitbox& Mesh, float* Out_Delta, glm::vec3* Out_Normal)
	{
		glm::vec3 Normal = glm::normalize(*Mesh.Position - *Sphere.Position);

		*Out_Normal = Normal;

		*Out_Delta = 999999;

		for (size_t W = 0; W < Mesh.Transformed_Vertices.size(); W++)
		{
			glm::vec3 A_Point = Mesh.Transformed_Vertices[W];

			A_Point -= *Sphere.Position;
			A_Point += *Mesh.Position;

			*Out_Delta = std::fminf(*Out_Delta, glm::dot(Normal, A_Point));
		}
	}

	void Separating_Axis_Theorem(Mesh_Hitbox& A, Mesh_Hitbox& B, float* Delta, uint32_t* Indices_Index, glm::vec3* Ideal_Normal, Separating_Axis_Theorem_Info* Info)
	{
		std::vector<glm::vec3> A_Points(A.Vertices.size());
		std::vector<glm::vec3> B_Points(B.Vertices.size());

		std::vector<unsigned char> Excluded_Points(B.Vertices.size());

		for (size_t W = 0; W < A.Indices.size(); W += 3)
		{
			// This will check the collision detection between a certain projection of the points

			memcpy(A_Points.data(), A.Transformed_Vertices.data(), A_Points.size() * sizeof(glm::vec3));
			memcpy(B_Points.data(), B.Transformed_Vertices.data(), B_Points.size() * sizeof(glm::vec3));

			for (size_t V = 0; V < A.Transformed_Vertices.size(); V++)
			{
				A_Points[V] -= (A.Transformed_Vertices[A.Indices[W]] + *A.Position);
				A_Points[V] += *A.Position;
			}

			for (size_t V = 0; V < B.Transformed_Vertices.size(); V++)
			{
				B_Points[V] -= (A.Transformed_Vertices[A.Indices[W]] + *A.Position);
				B_Points[V] += *B.Position;
			}

			glm::vec3 Normal = -Calculate_Surface_Normal(A.Transformed_Vertices[A.Indices[W]], A.Transformed_Vertices[A.Indices[W + 1]], A.Transformed_Vertices[A.Indices[W + 2]]);// Calculate_Surface_Normal(A_Points[A.Indices[W]], A_Points[A.Indices[W + 1]], A_Points[A.Indices[W + 2]]);

			//glm::vec3 Average_Face_Position = A.Transformed_Vertices[A.Indices[W]] + A.Transformed_Vertices[A.Indices[W + 1]] + A.Transformed_Vertices[A.Indices[W + 2]];

			//Normal *= copysignf(1, glm::dot(Normal, Average_Face_Position));

			float Local_Delta = glm::dot(Normal, B_Points[0]);

			size_t Local_Opposite_Index = 0;

			//glm::vec3 Average_Collision_Position = Local_Delta < 0.0f ? B.Transformed_Vertices[0] : glm::vec3(0.0f);
			//size_t Number_Of_Collision_Positions = Local_Delta < 0.0f;

			Excluded_Points[0] |= (Local_Delta > 0.0f);

			for (size_t V = 1; V < B.Transformed_Vertices.size(); V++)
			{
				float New_Local_Delta = glm::dot(Normal, B_Points[V]);

				Excluded_Points[V] |= (New_Local_Delta > 0.0f);

				Local_Delta = std::fminf(New_Local_Delta, Local_Delta);

				//Local_Delta = std::fminf(Local_Delta, glm::dot(Normal, B_Points[V]));
			}

			if (Local_Delta > *Delta)
			{
				*Delta = Local_Delta;
				*Indices_Index = Local_Opposite_Index;
				*Ideal_Normal = Normal;
			}
		}

		Info->Number_Of_Collision_Points = 0;

		Info->Average_Collision_Position = glm::vec3(0, 0, 0);

		for (size_t W = 0; W < B.Transformed_Vertices.size(); W++)
		{
			if (!Excluded_Points[W])
			{
				Info->Average_Collision_Position += B.Transformed_Vertices[W] + *B.Position;
				Info->Number_Of_Collision_Points++;
			}
		}

		//if(Number_Of_Collision_Points)
		//	Info->Average_Collision_Position /= Number_Of_Collision_Points;
	}
	
	Collision_Info Mesh_Against_Mesh(Mesh_Hitbox* A, Mesh_Hitbox* B)
	{
		Separating_Axis_Theorem_Info Infos[2] =
		{
			{ -9999, 9999, A },
			{ -9999, 9999, B }
		};

		Collision_Test::Separating_Axis_Theorem(*A, *B, &Infos[0].Delta, &Infos[0].Indices_Index, &Infos[0].Normal, &Infos[0]);
		Collision_Test::Separating_Axis_Theorem(*B, *A, &Infos[1].Delta, &Infos[1].Indices_Index, &Infos[1].Normal, &Infos[1]);

		//Infos[1].Normal *= -1;

		size_t Infos_Index = Infos[0].Delta < Infos[1].Delta;

		if (Infos[Infos_Index].Delta <= 0) // && Infos[Infos_Index].Average_Collision_Position.length() != 0.0f)
		{
#define Hitbox_Vertices Infos[1u - Infos_Index].Mesh_Hitbox->Transformed_Vertices
#define Ideal_Index Infos[Infos_Index].Indices_Index

			glm::vec3 Average_Collision_Position = Infos[0].Average_Collision_Position + Infos[1].Average_Collision_Position;
			Average_Collision_Position /= (Infos[0].Number_Of_Collision_Points + Infos[1].Number_Of_Collision_Points);

			// glm::vec3 Normal = glm::vec3(Infos[Infos_Index].Normal.x, 0, Infos[Infos_Index].Normal.y);

			float Sign_Bits[] = { 1, -1 }; // If the measurement was taken from the other hitbox's perspective, we need to flip it back around to our perspective.

			glm::vec3 Normal = Sign_Bits[Infos_Index] * Infos[Infos_Index].Normal; // *Calculate_Surface_Normal(Hitbox_Vertices[Hitbox_Indices[Ideal_Index]], Hitbox_Vertices[Hitbox_Indices[Ideal_Index + 1]], Hitbox_Vertices[Hitbox_Indices[Ideal_Index + 2]]);
			
			return Collision_Info(Average_Collision_Position - Normal * Infos[Infos_Index].Delta * 0.0f, -Normal, Infos[Infos_Index].Delta);

#undef Hitbox_Vertices
#undef Hitbox_Indices
#undef Ideal_Index
		}

		return Collision_Info();
	}

	Collision_Info Mesh_Against_Sphere(Mesh_Hitbox* Mesh, Sphere_Hitbox* Sphere)
	{
		Mesh_Hitbox Temp_Sphere;

		Separating_Axis_Theorem_Info Infos[2] =
		{
			{ -9999, 9999, Mesh },
			{ -9999, 9999, &Temp_Sphere }
		};

		Temp_Sphere.Position = Sphere->Position;
		Temp_Sphere.Transformed_Vertices.push_back(glm::vec3(0, 0, 0));
		Temp_Sphere.Vertices.push_back(glm::vec3(0, 0, 0));
		Temp_Sphere.Indices.push_back(0);

		Separating_Axis_Theorem(*Mesh, Temp_Sphere, &Infos[0].Delta, &Infos[0].Indices_Index, &Infos[0].Normal, &Infos[0]);
		Sphere_Separating_Axis_Theorem(*Sphere, *Mesh, &Infos[1].Delta, &Infos[1].Normal);

		Infos[1].Indices_Index = 0;

		size_t Index = Infos[0].Delta < Infos[1].Delta;

		Infos[Index].Delta -= Sphere->Radius;

		// Infos[1].Normal *= -1;

		//Infos[0].Delta *= -1;


		if (Infos[Index].Delta <= 0)
			return Collision_Info(
				*Temp_Sphere.Position + Infos[1].Normal * (Sphere->Radius - Infos[1].Delta),
				-Infos[Index].Normal, Infos[Index].Delta);
		else
			return Collision_Info();
	}

	bool Always_Return(Collision_Info Info)
	{
		return true;
	}

	bool Only_Return_Floor(Collision_Info Info)
	{
		return Info.Collision_Normal.y > 0.3; // This is the condition for an object to be a "floor"
	}

	Collision_Info Find_Collision(Hitbox* A, bool (*Should_Compare)(Hitbox*, Hitbox*), Hitbox** Target_Pointer, bool (*Should_Return)(Collision_Info) = Always_Return)
	{
		Collision_Info Info;

		for (size_t W = 0; W < Scene_Hitboxes.size(); W++)
		{
			if (Should_Compare(A, Scene_Hitboxes[W])) // If we should compare these two hitboxes
			{
				Info = A->Hitdetection(Scene_Hitboxes[W]); // Compare them

				if (Info.Overlap != 0 && Should_Return(Info)) // If there is an intersection of any kind AND the evaluated function thinks there should be a return
				{
					*Target_Pointer = Scene_Hitboxes[W]; // Set the pointer of the hitbox in question
					return Info;						// and return the collision info that we got
				}
			}
		}

		*Target_Pointer = nullptr;

		return Info;
	}

	bool Always_Compare(Hitbox* A, Hitbox* B)
	{
		return true;
	}

	bool Not_Against_Player_Compare(Hitbox* A, Hitbox* B);

	Collision_Info Raycast(glm::vec3 Origin, glm::vec3 Velocity, size_t Max_Step, bool (*Should_Compare)(Hitbox*, Hitbox*), Hitbox** Target_Pointer) // A raycast only receives 1 hitbox
	{
		Sphere_Hitbox Particle;
		Particle.Radius = 0.1;

		//AABB_Hitbox Particle;
		//Particle.A = 0.05f * glm::vec3(1, 1, 1);
		//Particle.B = 0.05f * glm::vec3(-1, -1, -1);

		Particle.Position = &Origin;

		size_t Step = 0;

		Collision_Info Info;

		do
		{
			*Particle.Position += Velocity;
			Step++;
			Info = Find_Collision(&Particle, Should_Compare, Target_Pointer);
		} while (Step < Max_Step && Info.Overlap == 0);

		Info.Collision_Position = *Particle.Position + Info.Collision_Normal * 0.1f;

		return Info; // If we never found a collision, this will have already been initialised to zero anyways so its fine
	}

	Collision_Info Sphere_Against_Sphere(Sphere_Hitbox& Sphere, Sphere_Hitbox& Other)
	{
		glm::vec3 Overlap = *Sphere.Position - *Other.Position;

		float Length = sqrtf(glm::dot(Overlap, Overlap));

		if (Sphere.Radius + Other.Radius > Length)
		{
			float Inv_Length = 1.0f / Length;
			Overlap *= Inv_Length;

			return Collision_Info(*Other.Position + Overlap * Other.Radius, Overlap, Length - (Sphere.Radius + Other.Radius));
		}
		else
			return Collision_Info();
	}

	Collision_Info AABB_Against_AABB(AABB_Hitbox& AABB, AABB_Hitbox& Other)
	{
		float Positions[6][2] =
		{
			{ AABB.A.x + AABB.Position->x, Other.B.x + Other.Position->x },
			{ AABB.A.y + AABB.Position->y, Other.B.y + Other.Position->y },
			{ AABB.A.z + AABB.Position->z, Other.B.z + Other.Position->z },

			{ Other.A.x + Other.Position->x, AABB.B.x + AABB.Position->x },
			{ Other.A.y + Other.Position->y, AABB.B.y + AABB.Position->y },
			{ Other.A.z + Other.Position->z, AABB.B.z + AABB.Position->z }
		};

		float Delta_Overlaps[6] =
		{
			Positions[0][0] - Positions[0][1], // positive x (towards the initial caller of the "hitdetection" function)
			Positions[1][0] - Positions[1][1], // positive y
			Positions[2][0] - Positions[2][1], // positive z

			Positions[3][0] - Positions[3][1], // negative x
			Positions[4][0] - Positions[4][1], // negative y
			Positions[5][0] - Positions[5][1], // negative z
		};

		glm::vec3 Normals[6] =
		{
			glm::vec3(1, 0, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 0, 1),

			glm::vec3(-1, 0, 0),
			glm::vec3(0, -1, 0),
			glm::vec3(0, 0, -1)
		};

		uint8_t X_Index = Delta_Overlaps[0] > Delta_Overlaps[3] ? 0 : 3;
		uint8_t Y_Index = Delta_Overlaps[1] > Delta_Overlaps[4] ? 1 : 4;
		uint8_t Z_Index = Delta_Overlaps[2] > Delta_Overlaps[5] ? 2 : 5;

		bool X_Clip, Y_Clip, Z_Clip;

		X_Clip = Delta_Overlaps[X_Index] < 0; // < 0 && Delta_Overlaps[3] < 0;
		Y_Clip = Delta_Overlaps[Y_Index] < 0; // < 0 && Delta_Overlaps[4] < 0;
		Z_Clip = Delta_Overlaps[Z_Index] < 0; // < 0 && Delta_Overlaps[5] < 0;

		if ((!(X_Clip && Y_Clip && Z_Clip)) || &Other == &AABB)
			return Collision_Info();
		else
		{
			glm::vec3 Average_Position;

			Average_Position.x = (Positions[X_Index][0] + Positions[X_Index][1]) * 0.5f;
			Average_Position.y = (Positions[Y_Index][0] + Positions[Y_Index][1]) * 0.5f;
			Average_Position.z = (Positions[Z_Index][0] + Positions[Z_Index][1]) * 0.5f;

			uint8_t Index = Delta_Overlaps[X_Index] > Delta_Overlaps[Y_Index] ? X_Index : Y_Index;
			Index = Delta_Overlaps[Index] > Delta_Overlaps[Z_Index] ? Index : Z_Index;

			return Collision_Info(Average_Position, Normals[Index], Delta_Overlaps[Index]);
		}
	}

	Collision_Info AABB_Against_Mesh(AABB_Hitbox& AABB, Mesh_Hitbox& Mesh)
	{
		Mesh_Hitbox Temp;

#define MinX AABB.A.x
#define MaxX AABB.B.x
#define MinY AABB.A.y
#define MaxY AABB.B.y
#define MinZ AABB.A.z
#define MaxZ AABB.B.z

		Temp.Transformed_Vertices = std::vector<glm::vec3>
		{
			glm::vec3(MaxX, MinY, MaxZ),
			glm::vec3(MinX, MaxY, MaxZ),
			glm::vec3(MaxX, MaxY, MaxZ),
			glm::vec3(MinX, MinY, MaxZ),
			glm::vec3(MinX, MaxY, MinZ),
			glm::vec3(MinX, MaxY, MaxZ),
			glm::vec3(MinX, MinY, MinZ),
			glm::vec3(MinX, MinY, MaxZ),
			glm::vec3(MaxX, MinY, MaxZ),
			glm::vec3(MaxX, MaxY, MinZ),
			glm::vec3(MaxX, MaxY, MaxZ),
			glm::vec3(MinX, MaxY, MaxZ),
			glm::vec3(MaxX, MinY, MinZ),
			glm::vec3(MaxX, MinY, MaxZ),
			glm::vec3(MaxX, MaxY, MaxZ),
			glm::vec3(MinX, MinY, MinZ),
			glm::vec3(MaxX, MinY, MinZ),
			glm::vec3(MaxX, MaxY, MinZ)
		};

		Temp.Vertices = Temp.Transformed_Vertices;

		// Temp.Transformed_Vertices.resize(8);

#undef MinX
#undef MaxX
#undef MinY
#undef MaxY
#undef MinZ
#undef MaxZ

		Temp.Indices = std::vector<uint32_t> // These are just hard-coded values for the indices
		{
			0, 1, 2,
			3, 4, 5,
			6, 7, 8,
			9, 10, 11,
			12, 13, 14,
			15, 16, 17
		};

		Temp.Position = AABB.Position;
		Temp.Matrix_Rotation = glm::mat3(1.0f);

		return Mesh.Mesh_Hitdetection(&Temp);
	}

	Collision_Info AABB_Against_Sphere(AABB_Hitbox& AABB, Sphere_Hitbox& Sphere)
	{
		glm::vec3 AABB_Position;
		AABB_Position.x = std::fminf(std::fmaxf(Sphere.Position->x, AABB.A.x + AABB.Position->x), AABB.B.x + AABB.Position->x);
		AABB_Position.y = std::fminf(std::fmaxf(Sphere.Position->y, AABB.A.y + AABB.Position->y), AABB.B.y + AABB.Position->y);
		AABB_Position.z = std::fminf(std::fmaxf(Sphere.Position->z, AABB.A.z + AABB.Position->z), AABB.B.z + AABB.Position->z);

		glm::vec3 Overlap = AABB_Position - *Sphere.Position;

		if (Overlap == glm::vec3(0, 0, 0))
		{
			float Positions[6][2] =
			{
				{ AABB.A.x + AABB.Position->x, Sphere.Position->x },
				{ AABB.A.y + AABB.Position->y, Sphere.Position->y },
				{ AABB.A.z + AABB.Position->z, Sphere.Position->z },

				{ Sphere.Position->x, AABB.B.x + AABB.Position->x },
				{ Sphere.Position->y, AABB.B.y + AABB.Position->y },
				{ Sphere.Position->z, AABB.B.z + AABB.Position->z }
			};

			float Delta_Overlaps[6] =
			{
				Positions[0][0] - Positions[0][1] - Sphere.Radius, // positive x (towards the initial caller of the "hitdetection" function)
				Positions[1][0] - Positions[1][1] - Sphere.Radius, // positive y
				Positions[2][0] - Positions[2][1] - Sphere.Radius, // positive z

				Positions[3][0] - Positions[3][1] - Sphere.Radius, // negative x
				Positions[4][0] - Positions[4][1] - Sphere.Radius, // negative y
				Positions[5][0] - Positions[5][1] - Sphere.Radius, // negative z
			};

			glm::vec3 Normals[6] =
			{
				glm::vec3(1, 0, 0),
				glm::vec3(0, 1, 0),
				glm::vec3(0, 0, 1),

				glm::vec3(-1, 0, 0),
				glm::vec3(0, -1, 0),
				glm::vec3(0, 0, -1)
			};

			uint8_t X_Index = Delta_Overlaps[0] > Delta_Overlaps[3] ? 0 : 3;
			uint8_t Y_Index = Delta_Overlaps[1] > Delta_Overlaps[4] ? 1 : 4;
			uint8_t Z_Index = Delta_Overlaps[2] > Delta_Overlaps[5] ? 2 : 5;

			glm::vec3 Average_Position;

			Average_Position.x = (Positions[X_Index][0] + Positions[X_Index][1]) * 0.5f;
			Average_Position.y = (Positions[Y_Index][0] + Positions[Y_Index][1]) * 0.5f;
			Average_Position.z = (Positions[Z_Index][0] + Positions[Z_Index][1]) * 0.5f;

			uint8_t Index = Delta_Overlaps[X_Index] > Delta_Overlaps[Y_Index] ? X_Index : Y_Index;
			Index = Delta_Overlaps[Index] > Delta_Overlaps[Z_Index] ? Index : Z_Index;

			return Collision_Info(Average_Position, Normals[Index], Delta_Overlaps[Index]);
		}

		float Length = sqrtf(glm::dot(Overlap, Overlap));

		if (Sphere.Radius > Length)
		{
			float Inv_Length = 1.0f / Length;
			Overlap *= Inv_Length;

			return Collision_Info(AABB_Position, Overlap, Length - Sphere.Radius);
		}
		else
			return Collision_Info();
	}
};

Collision_Info AABB_Hitbox::AABB_Hitdetection(AABB_Hitbox* Other)
{
	return Collision_Test::AABB_Against_AABB(*this, *Other);
}

Collision_Info AABB_Hitbox::Sphere_Hitdetection(Sphere_Hitbox* Other)
{
	return Collision_Test::AABB_Against_Sphere(*this, *Other);
}

Collision_Info AABB_Hitbox::Mesh_Hitdetection(Mesh_Hitbox* Other)
{
	return Inverse_Collision(Collision_Test::AABB_Against_Mesh(*this, *Other));
}

//

Collision_Info Sphere_Hitbox::AABB_Hitdetection(AABB_Hitbox* Other)
{
	return Inverse_Collision(Collision_Test::AABB_Against_Sphere(*Other, *this));
}

Collision_Info Sphere_Hitbox::Sphere_Hitdetection(Sphere_Hitbox* Other)
{
	return Collision_Test::Sphere_Against_Sphere(*this, *Other);
}

Collision_Info Sphere_Hitbox::Mesh_Hitdetection(Mesh_Hitbox* Other)
{
	return Inverse_Collision(Collision_Test::Mesh_Against_Sphere(Other, this));
}

//

Collision_Info Mesh_Hitbox::Mesh_Hitdetection(Mesh_Hitbox* Other)
{
	return Collision_Test::Mesh_Against_Mesh(this, Other);
}

Collision_Info Mesh_Hitbox::Sphere_Hitdetection(Sphere_Hitbox* Other)
{
	return Collision_Test::Mesh_Against_Sphere(this, Other);
}

Collision_Info Mesh_Hitbox::AABB_Hitdetection(AABB_Hitbox* Other)
{
	return Collision_Test::AABB_Against_Mesh(*Other, *this);
}

#endif
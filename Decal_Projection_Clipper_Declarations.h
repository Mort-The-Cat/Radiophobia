#ifndef VISION_DECAL_PROJECTION_CLIPPER_DECLARATIONS
#define VISION_DECAL_PROJECTION_CLIPPER_DECLARATIONS

#include "OpenGL_Declarations.h"
#include "Vertex_Buffer_Declarations.h"

#include "Particle_System_Declarations.h"

// http://blog.wolfire.com/2009/06/how-to-project-decals/

// save for later! Ty!

namespace Decal
{
	struct Point_Projection
	{
		Model_Vertex Vertex; // This is the vertex itself
		glm::vec3 Projected; // This is the projected point

		void Rotate_Projected_Frustum()
		{
			float Temp = Projected.x;
			Projected.x = Projected.y;
			Projected.y = -Temp;
		}

		Point_Projection operator* (float Scalar)
		{
			Point_Projection New;
			New.Vertex.Position = Vertex.Position * glm::vec3(Scalar);
			New.Vertex.Normal = Vertex.Normal * glm::vec3(Scalar);
			New.Vertex.UV = Vertex.UV * glm::vec2(Scalar);

			New.Projected = Projected * glm::vec3(Scalar);

			return New;
		}

		Point_Projection operator+ (Point_Projection Other)
		{
			Point_Projection New;
			New.Vertex.Position = Vertex.Position + Other.Vertex.Position;
			New.Vertex.Normal = Vertex.Normal + Other.Vertex.Normal;
			New.Vertex.UV = Vertex.UV + Other.Vertex.UV;

			New.Projected = Projected + Other.Projected;

			return New;
		}
	};

	class Decal_Space_Transform
	{
	public:
		glm::vec3 Origin;
		glm::mat3 TBN; // This is the TBN matrix of the decal

		glm::mat3 Inverse_TBN; // Inverse thereof

		Decal_Space_Transform(glm::vec3 Originp, glm::vec3 Orientation)
		{
			Origin = Originp;

			glm::vec3 Tangent = glm::normalize(
				glm::cross(Orientation,
					glm::vec3(RNG() - 0.5f, RNG() - 0.5f, RNG() - 0.5f)
				));

			glm::vec3 Bitangent = glm::cross(Tangent, Orientation);

			TBN = glm::mat3(Tangent, Bitangent, Orientation); // Orientation should be Z, so T and B can be x and y

			Inverse_TBN = glm::transpose(TBN);
		}

		glm::vec3 To_Decal_Space(glm::vec3 Point) // This converts to decal space
		{
			Point -= Origin;
			return Inverse_TBN * Point;
		}
	};

	size_t Increment(size_t Index, size_t Size)
	{
		Index++;

		return Index * (Index != Size);
	}

	size_t Decrement(size_t Index, size_t Size)
	{
		if (0 == Index)
			Index = Size;

		Index--;

		return Index;
	}

	std::vector<Point_Projection> Clip_Poly_Against_Frustum(std::vector<Point_Projection> Poly) // In this case, we want x to be less than or equal to 1.0f
	{
		// std::vector<Point_Projection> New_Poly;

		if (Poly.size() == 0)
			return std::vector<Point_Projection>(0);

		size_t Greatest_Index = 0, Lowest_Index = 0;

		for (size_t W = 1; W < Poly.size(); W++)
		{
			if (Poly[W].Projected.x > Poly[Greatest_Index].Projected.x)
				Greatest_Index = W;

			if (Poly[W].Projected.x < Poly[Lowest_Index].Projected.x)
				Lowest_Index = W;
		}

		// This gets the greatest/lowest indices

		if (Poly[Lowest_Index].Projected.x >= 1.0f)		// If no points are in view frustum
			return std::vector<Point_Projection>(0);	// just return no points
		
		if (Poly[Greatest_Index].Projected.x < 1.0f)	// if all points are in view frustum
			return Poly;								// no worries

		// Otherwise? We do some clipping!

		std::vector<Point_Projection> New_Poly = { Poly[Lowest_Index] };

		size_t Index = Increment(Lowest_Index, Poly.size());

		while (Poly[Index].Projected.x < 1.0f) // While within frustum
		{
			New_Poly.push_back(Poly[Index]);
			Index = Increment(Index, Poly.size());
		}

		//

		{
			size_t A = Decrement(Index, Poly.size());

			// We know that A was within frustum and Index is not

			// As soon as its not in frustum!

			// we wanna get the new val

			float Phi = (Poly[A].Projected.x - 1.0f) / (Poly[A].Projected.x - Poly[Index].Projected.x);

			New_Poly.push_back((Poly[A] * (1.0f - Phi)) + (Poly[Index] * Phi));
		}

		/*
		
		Z = A * (1 - phi) + B * phi

		where 1.0f = A.x - phi * A.x + phi * B.x

		phi = (A.x - 1.0f) / (A.x - B.x)
		
		*/

		Index = Decrement(Lowest_Index, Poly.size());

		while (Poly[Index].Projected.x < 1.0f) // While within frustum
		{
			New_Poly.insert(New_Poly.begin(), Poly[Index]);


			Index = Decrement(Index, Poly.size());
		}

		{
			size_t A = Increment(Index, Poly.size());

			float Phi = (Poly[A].Projected.x - 1.0f) / (Poly[A].Projected.x - Poly[Index].Projected.x);

			New_Poly.insert(New_Poly.begin(), (Poly[A] * (1.0f - Phi)) + (Poly[Index] * Phi));
		}

		return New_Poly;
	}

	/*void Debug_Clip_Against_View_Frustum()
	{
		std::vector<Point_Projection> Points(3);

		Points[0].Projected = glm::vec3(0.0f, 0.0f, 1.0f);
		Points[1].Projected = glm::vec3(-0.5f, -0.5f, 1.0f);
		Points[2].Projected = glm::vec3(1.25f, -0.75f, 1.0f);

		Points = Clip_Poly_Against_Frustum(Points);
	}*/

	std::vector<Point_Projection> Clip_Triangle(Decal_Space_Transform Decal, Model_Vertex A, Model_Vertex B, Model_Vertex C)
	{
		std::vector<Point_Projection> Points = std::vector<Point_Projection>(3);
		Points[0].Vertex = A;
		Points[1].Vertex = B;
		Points[2].Vertex = C;

		for (size_t W = 0; W < 3; W++)
			Points[W].Projected = Decal.To_Decal_Space(Points[W].Vertex.Position);

		for (size_t Sides = 0; Sides < 4; Sides++)
		{
			Points = Clip_Poly_Against_Frustum(Points);

			for (size_t W = 0; W < Points.size(); W++)
				Points[W].Rotate_Projected_Frustum();
		}
		
		// At the end of this, you can extract all the values you want from the vector of projected points

		return Points;	// Note that this will most likely not be a proper triangle anymore after clipping has been performed

		// Other functions will have to split this up into triangles
	}

	std::vector<std::vector<Point_Projection>> Polygon_To_Triangles(std::vector<Point_Projection> Points)
	{
		if (Points.size() < 3)										// if no triangles? Rubbish
			return std::vector<std::vector<Point_Projection>>(0);

		std::vector<std::vector<Point_Projection>> Triangles;

		size_t A = 1, B = 2;

		while (B != 0)
		{
			Triangles.push_back(std::vector<Point_Projection>(3));

			Triangles.back()[0] = Points[0];
			Triangles.back()[1] = Points[A];
			Triangles.back()[2] = Points[B];

			A = Increment(A, Points.size());
			B = Increment(B, Points.size());
		}

		return Triangles;
	}


}

class Decal_Particle_Info : public Particle_Info<Decal_Particle>
{
public:
	Decal_Particle_Info() { Particles_Per_Call = 25; }

	void Spawn_Particle(Decal::Point_Projection A, Decal::Point_Projection B, Decal::Point_Projection C)
	{
		const float Bias = 0.0075f;

		Decal_Particle New_Particle;
		New_Particle.A = A.Vertex.Position + glm::vec3(Bias) * A.Vertex.Normal;
		New_Particle.A_UV = glm::vec2(A.Projected.x, A.Projected.y);

		New_Particle.B = B.Vertex.Position + glm::vec3(Bias) * B.Vertex.Normal;
		New_Particle.B_UV = glm::vec2(B.Projected.x, B.Projected.y);

		New_Particle.C = C.Vertex.Position + glm::vec3(Bias) * C.Vertex.Normal;
		New_Particle.C_UV = glm::vec2(C.Projected.x, C.Projected.y);

		Particles_Data.push_back(New_Particle);
	}

	void Update()
	{
		return; // I won't worry about this rn haha
	}
};

Particle_Renderer<Decal_Particle_Info, Decal_Vertex_Buffer> Decal_Particles; // This uses a billboard vertex buffer but it uses the parsed data for the vertex info

void Add_Some_Test_Decals()
{
	glm::vec3 Position = glm::vec3(2.2f, -1.23f, 8.27f);

	/*Decal::Point_Projection A, B, C;
	A.Vertex.Position = Position + glm::vec3(RNG(), RNG(), RNG());
	B.Vertex.Position = Position + glm::vec3(RNG(), RNG(), RNG());
	C.Vertex.Position = Position + glm::vec3(RNG(), RNG(), RNG());

	A.Projected = glm::vec3(0.0f, 0.0f, 1.0f);
	B.Projected = glm::vec3(1.0f, 0.0f, 1.0f);
	C.Projected = glm::vec3(0.0f, 1.0f, 1.0f);

	Decal_Particles.Particles.Spawn_Particle(A, B, C);*/

	Decal::Decal_Space_Transform Transform(Position, glm::vec3(0.0f, 1.0f, 0.0f));

	Model_Mesh* Level = Pull_Mesh("Assets/Models/Intro_Level/Test_Intro_Level_Floor.obj", LOAD_MESH_OBJ_BIT).Mesh;

	for (size_t Index = 0; Index < Level->Indices.size(); Index += 3)
	{
		std::vector<std::vector<Decal::Point_Projection>> Triangles = Decal::Polygon_To_Triangles(Decal::Clip_Triangle(
			Transform,
			Level->Vertices[Level->Indices[Index]],
			Level->Vertices[Level->Indices[Index + 1]],
			Level->Vertices[Level->Indices[Index + 2]]
		));

		for (size_t W = 0; W < Triangles.size(); W++)
			Decal_Particles.Particles.Spawn_Particle(Triangles[W][0], Triangles[W][1], Triangles[W][2]);
	}
}

#endif
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

		void Projected_To_Decal_UV()
		{
			Projected *= glm::vec3(0.5f); // no longer [-1,1] now [-0.5,0.5]
			Projected += glm::vec3(0.5f); // no [0,1]
		}

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

		Decal_Space_Transform(glm::vec3 Originp, glm::vec3 Orientation, float Decal_Radius)
		{
			Origin = Originp;

			glm::vec3 Tangent = glm::normalize(
				glm::cross(Orientation,
					glm::vec3(RNG() - 0.5f, RNG() - 0.5f, RNG() - 0.5f)
				));

			glm::vec3 Bitangent = glm::cross(Tangent, Orientation);

			Tangent /= Decal_Radius;
			Bitangent /= Decal_Radius;
			Orientation /= Decal_Radius;

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
	Decal_Particle_Info() { Particles_Per_Call = 50; }

	void Spawn_Particle(Decal::Point_Projection A, Decal::Point_Projection B, Decal::Point_Projection C)
	{
		const float Bias = 0.0025f;

		Decal_Particle New_Particle;
		New_Particle.A = A.Vertex.Position + glm::vec3(Bias) * A.Vertex.Normal;
		New_Particle.A_UV = glm::vec2(A.Projected.x, A.Projected.y);

		New_Particle.B = B.Vertex.Position + glm::vec3(Bias) * B.Vertex.Normal;
		New_Particle.B_UV = glm::vec2(B.Projected.x, B.Projected.y);

		New_Particle.C = C.Vertex.Position + glm::vec3(Bias) * C.Vertex.Normal;
		New_Particle.C_UV = glm::vec2(C.Projected.x, C.Projected.y);

		//

		glm::vec2 UV_A = New_Particle.B_UV - New_Particle.A_UV;
		glm::vec2 UV_B = New_Particle.C_UV - New_Particle.A_UV;

		glm::vec3 Edge_A; // = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
		glm::vec3 Edge_B; // = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

		if ((UV_A.x * UV_B.y - UV_B.x * UV_A.y) == 0)
		{
			UV_A = UV_B;
			UV_B = New_Particle.B_UV - New_Particle.A_UV;

			Edge_A = New_Particle.C - New_Particle.A;
			Edge_B = New_Particle.B - New_Particle.A;
		}
		else
		{
			Edge_A = New_Particle.B - New_Particle.A;
			Edge_B = New_Particle.C - New_Particle.A;
		}

		float Inv = 1.0f / (UV_A.x * UV_B.y - UV_B.x * UV_A.y);

		glm::vec3 Tangent = -glm::normalize(Inv * (UV_B.y * Edge_A - UV_A.y * Edge_B));

		New_Particle.Normal = -glm::normalize(glm::cross(B.Vertex.Position - A.Vertex.Position, C.Vertex.Position - A.Vertex.Position));
		New_Particle.Tangent = Tangent;
		New_Particle.Bitangent = glm::cross(New_Particle.Tangent, New_Particle.Normal);

		//

		Particles_Data.push_back(New_Particle);
	}

	void Update()
	{
		return; // I won't worry about this rn haha
	}
};

Particle_Renderer<Decal_Particle_Info, Decal_Vertex_Buffer> Decal_Particles; // This uses a billboard vertex buffer but it uses the parsed data for the vertex info

void Create_Bullet_Decal(Model_Mesh* Model,glm::vec3 Position, glm::vec3 Orientation, float Decal_Radius)
{
	Decal::Decal_Space_Transform Transform(Position, Orientation, Decal_Radius);

	for (size_t Index = 0; Index < Model->Indices.size(); Index += 3)
	{
		std::vector<std::vector<Decal::Point_Projection>> Triangles = Decal::Polygon_To_Triangles(Decal::Clip_Triangle(
			Transform,
			Model->Vertices[Model->Indices[Index]],
			Model->Vertices[Model->Indices[Index + 1]],
			Model->Vertices[Model->Indices[Index + 2]]
		));

		for (size_t W = 0; W < Triangles.size(); W++)
		{
			Triangles[W][0].Projected_To_Decal_UV();
			Triangles[W][1].Projected_To_Decal_UV();
			Triangles[W][2].Projected_To_Decal_UV();

			glm::vec3 Average_Position = Triangles[W][0].Vertex.Position + Triangles[W][1].Vertex.Position + Triangles[W][2].Vertex.Position;
			Average_Position *= glm::vec3(0.333f);

			if (glm::length(Average_Position - Position) > Decal_Radius * 4)
				continue;

			Decal_Particles.Particles.Spawn_Particle(Triangles[W][0], Triangles[W][1], Triangles[W][2]);
		}
	}
}

#endif
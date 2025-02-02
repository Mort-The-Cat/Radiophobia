#ifndef PARTICLE_SYSTEM_DECLARATIONS_VISIONGL
#define PARTICLE_SYSTEM_DECLARATIONS_VISIONGL

#include "OpenGL_Declarations.h"
#include "Vertex_Buffer_Declarations.h"
#include "Texture_Declarations.h"

// I want to make a pseudo-volumetric light volume in the engine

struct Decal_Particle
{
	glm::vec3 A, B, C;
	glm::vec3 Normal, Bitangent, Tangent;
	glm::vec2 A_UV, B_UV, C_UV;
};

struct Spark_Particle
{
	glm::vec4 Position;		// .w is time
	glm::vec4 Orientation;	// .w is a random value (which will be useful for a bunch of things
};

struct Volumetric_Cone_Particle
{
	float Volume_Density;

	float C, Cosine_Theta_Squared, Dot_Source_Direction; // dot(alef - alpha, delta)

	glm::mat3 Scaling_Rotation; // This handles the scaling and rotation thereof

	glm::vec3 Cone_Origin;
	glm::vec3 Cone_Direction;
	glm::vec3 Colour;
};

struct Muzzle_Flash_Particle
{
	glm::vec4 Position; // .w is age
	float Angular_Velocity;
};

struct Smoke_Particle
{
	glm::vec4 Position; // .w is age
	glm::vec4 Velocity; // .w is transparency
};

//

struct Galaxy_Test_Particle
{
	float Radius; // Angular offset of orbit is determined by the radius so this isn't a problem

	// We could also maybe determine the number of particles per orbit based on the radius and it'd make sense to do so but I'll implement that later

	float Age; // This is the age of the particle - determines how far around the orbit the particle has travelled

	float Index;

	// float Max_In_Orbit;

	// The index of the galaxy test particle is determined by the instance id of the particle. 

	// A modulo operation *might* need to be performed in order to loop it between some values (e.g. 0-100) but if it's only used to calculate an angular offset that literally doesn't matter
};

bool Smoke_Particle_Remove_If(const Smoke_Particle& A)
{
	return A.Position.w > 6; // Smoke disappears after 6 seconds
}

bool Spark_Particle_Remove_If(const Spark_Particle& A)
{
	return A.Position.w > 0.12f;
}

template<typename Particle>
class Particle_Info
{
public:
	size_t Count;
	size_t Particles_Per_Call;
	std::vector<Particle> Particles_Data;

	Camera_Uniform_Location_Object Camera_Location;
	Light_Uniform_Location_Object Light_Location;

	int Uniform_Location; // We're parsing this all to one array in the GPU
	size_t Float_Size;

	void Init_Shader_Information(Shader Shader)
	{
		Shader.Activate();
		Uniform_Location = glGetUniformLocation(Shader.Program_ID, "Particle_Data");

		Camera_Location = Initialise_Camera_Uniform_Locations_Object(Shader);
		Light_Location = Initialise_Light_Uniform_Locations_Object(Shader);

		Float_Size = sizeof(Particle) >> 2; // This is just the number of 32-bit chunks of memory that the particle uses i.e. how many "words"
	}

	size_t Parse_Values(size_t Index) 
	{
		size_t Count = std::min(Particles_Per_Call, Particles_Data.size() - Index);
		glUniform1fv(Uniform_Location, Count * Float_Size, (const GLfloat*)(&Particles_Data[Index]));

		return Count;
	}

};

class Spark_Particle_Info : public Particle_Info<Spark_Particle>
{
public:

	Spark_Particle_Info() { Particles_Per_Call = 75; }

	void Spawn_Particle(glm::vec3 Position, glm::vec3 Orientation)
	{
		Spark_Particle New_Particle;

		Orientation = glm::normalize(Orientation);

		New_Particle.Position = glm::vec4(Position.x, Position.y, Position.z, 0.0f);
		New_Particle.Orientation = glm::vec4(Orientation.x, Orientation.y, Orientation.z, RNG()); // There's our normalised random value

		Particles_Data.push_back(New_Particle);
	}

	void Update()
	{
		for (size_t W = 0; W < Particles_Data.size(); W++)
			Particles_Data[W].Position.w += Tick;

		auto Particles_To_Remove = std::remove_if(Particles_Data.begin(), Particles_Data.end(), Spark_Particle_Remove_If);

		Particles_Data.erase(Particles_To_Remove, Particles_Data.end());
	}
};

class Galaxy_Particle_Info : public Particle_Info<Galaxy_Test_Particle>
{
public:
	// This has only 2 floating point values

	Galaxy_Particle_Info() { Particles_Per_Call = 50; }

	void Spawn_Particle(float Radius, float Index) // Radius is all that matters
	{
		Galaxy_Test_Particle New_Particle;
		New_Particle.Radius = Radius;
		New_Particle.Age = 0.0f;
		New_Particle.Index = Index;
		// New_Particle.Max_In_Orbit = 6.28318f / static_cast<float>(Max_In_Orbit);
		Particles_Data.push_back(New_Particle);
	}

	void Update()
	{
		for (size_t W = 0; W < Particles_Data.size(); W++)
			Particles_Data[W].Age += Tick;
	}
};

bool Muzzle_Flash_Particle_Remove_If(const Muzzle_Flash_Particle& Particle)
{
	return Particle.Position.w > 0.05f;
}

class Muzzle_Flash_Particle_Info : public Particle_Info<Muzzle_Flash_Particle>
{
public:
	// This has 5 floating point values

	Muzzle_Flash_Particle_Info()
	{
		Particles_Per_Call = 60;
	}

	void Spawn_Particle(glm::vec3 Position)
	{
		Muzzle_Flash_Particle New_Particle;

		New_Particle.Position.x = Position.x;
		New_Particle.Position.y = Position.y;
		New_Particle.Position.z = Position.z;

		New_Particle.Position.w = 0.0f;

		New_Particle.Angular_Velocity = 30 * (RNG() - 0.5f);

		Particles_Data.push_back(New_Particle);
	}

	void Update()
	{
		for (size_t W = 0; W < Particles_Data.size(); W++)
			Particles_Data[W].Position.w += Tick;

		auto Particles_To_Remove = std::remove_if(Particles_Data.begin(), Particles_Data.end(), Muzzle_Flash_Particle_Remove_If);

		Particles_Data.erase(Particles_To_Remove, Particles_Data.end());
	}
};

class Smoke_Particle_Info : public Particle_Info<Smoke_Particle>
{
public:
	// This has 8 floating point values

	Smoke_Particle_Info()
	{
		Particles_Per_Call = 75;
	}

	void Spawn_Particle(glm::vec3 Position, glm::vec3 Velocity)
	{
		Smoke_Particle New_Particle;
		New_Particle.Position = glm::vec4(Position, 0);
		New_Particle.Velocity = glm::vec4(Velocity, RNG() * 24 - 12);
		Particles_Data.push_back(New_Particle);
	}

	void Update()
	{
		for (size_t W = 0; W < Particles_Data.size(); W++)
			Particles_Data[W].Position.w += Tick;

		auto Particles_To_Remove = std::remove_if(Particles_Data.begin(), Particles_Data.end(), Smoke_Particle_Remove_If);

		Particles_Data.erase(Particles_To_Remove, Particles_Data.end());
	}
};

class Volumetric_Cone_Particle_Info : public Particle_Info<Volumetric_Cone_Particle>
{
public:
	Volumetric_Cone_Particle_Info() 
	{
		Particles_Per_Call = 50;
	}

	void Spawn_Particle(glm::vec3 Cone_Origin, glm::vec3 Cone_Direction, glm::vec3 Colour, float Density, float Cone_Angle) // Cone angle is in degrees
	{
		Volumetric_Cone_Particle New_Particle;
		New_Particle.Cone_Origin = Cone_Origin;
		New_Particle.Cone_Direction = -Cone_Direction;

		New_Particle.Cosine_Theta_Squared = (cosf(Cone_Angle * DTR));
		New_Particle.Cosine_Theta_Squared *= New_Particle.Cosine_Theta_Squared; // This squares it

		New_Particle.Colour = Colour;
		New_Particle.Volume_Density = Density;

		/*New_Particle.Scaling_Rotation = glm::mat3(
			glm::scale(glm::mat4(1.0f), glm::vec3(tanf(Cone_Angle * DTR), 1.0f, tanf(Cone_Angle * DTR))) * 
			glm::lookAt(Cone_Direction, glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));*/



		/*New_Particle.Scaling_Rotation = glm::mat3(
			// glm::scale(glm::mat4(1.0f), glm::vec3(tanf(Cone_Angle * DTR), 1.0f, tanf(Cone_Angle * DTR))) *
			glm::lookAt(Random_Perpendicular_Vector(Cone_Direction), glm::vec3(0.0f), Cone_Direction)
		);*/

		New_Particle.Scaling_Rotation = Random_Direction_Matrix(Cone_Direction);

		// This will give us a matrix for the cone that is properly scaled and rotated in the direction we want

		Particles_Data.push_back(New_Particle);
	}

	void Update()
	{
		for (size_t W = 0; W < Particles_Data.size(); W++)
		{
			// Update value of c and dot(alef - alpha, delta)

			glm::vec3 Alef_Alpha = (Player_Camera.Position - Particles_Data[W].Cone_Origin);

			Particles_Data[W].Dot_Source_Direction = glm::dot(Alef_Alpha, Particles_Data[W].Cone_Direction);

			if (Particles_Data[W].Dot_Source_Direction * Particles_Data[W].Dot_Source_Direction > Particles_Data[W].Cosine_Theta_Squared * glm::dot(Alef_Alpha, Alef_Alpha))
			 	Particles_Data[W].Dot_Source_Direction = -1.0f; // This indicates rubbish expected GPU-side values

			Particles_Data[W].C = Particles_Data[W].Dot_Source_Direction * Particles_Data[W].Dot_Source_Direction - Particles_Data[W].Cosine_Theta_Squared * glm::dot(Alef_Alpha, Alef_Alpha);
		}

		// Might be cool if I used these often in the game
	}
};

template<typename Particle, typename Vertex_Buffer>
	class Particle_Renderer
	{
	public:
		bool Bind_Textures = true;

		Shader Shader;
		Particle Particles;
		Vertex_Buffer Mesh;
		Texture Albedo;
		Texture Material;

		void Delete_All_Particles()
		{
			Particles.Particles_Data.clear();
		}

		void Update()
		{
			Particles.Update();	// This will update all of the particles to account for the new delta time
		}

		void Render()
		{
			Shader.Activate();

			// glUniform2f(glGetUniformLocation(Shader.Program_ID, "Window_Dimensions"), Window_Width, Window_Height);
			glUniform2f(glGetUniformLocation(Shader.Program_ID, "Inv_Window_Dimensions"), 1.0f / (float)(Window_Width), 1.0f / (float)(Window_Height));

			Player_Camera.Bind_Buffers(Particles.Camera_Location);
			Light_Uniforms.Update_Buffer(Particles.Light_Location);

			Mesh.Bind_Buffer();

			if (Bind_Textures)
			{
				Albedo.Parse_Texture(Shader, "Albedo", 0);
				Albedo.Bind_Texture();

				Material.Parse_Texture(Shader, "Material", 1);
				Material.Bind_Texture();
			}

			for (size_t W = 0; W < Particles.Particles_Data.size(); W += Particles.Particles_Per_Call)
			{
				size_t Count = Particles.Parse_Values(W);

				glDrawElementsInstanced(GL_TRIANGLES, Mesh.Indices_Count, GL_UNSIGNED_INT, 0, Count);
			}
		}
	};

template<typename Particle, typename Vertex_Buffer>
	void Create_Particle_Renderer(Shader Shader, Vertex_Buffer Mesh, Texture Albedo, Texture Material, Particle_Renderer<Particle, Vertex_Buffer>* Target_Renderer)
	{
		Target_Renderer->Shader = Shader;
		Target_Renderer->Mesh = Mesh;
		Target_Renderer->Albedo = Albedo;
		Target_Renderer->Material = Material;

		Target_Renderer->Particles.Init_Shader_Information(Shader);
	}

Particle_Renderer<Smoke_Particle_Info, Model_Vertex_Buffer> Smoke_Particles;

Particle_Renderer<Smoke_Particle_Info, Billboard_Vertex_Buffer> Billboard_Smoke_Particles;

Particle_Renderer<Smoke_Particle_Info, Billboard_Vertex_Buffer> Billboard_Fire_Particles;

//

Particle_Renderer<Galaxy_Particle_Info, Billboard_Vertex_Buffer> Galaxy_Particles;

//

Particle_Renderer<Volumetric_Cone_Particle_Info, Model_Vertex_Buffer> Volumetric_Cone_Particles;

Particle_Renderer<Smoke_Particle_Info, Model_Vertex_Buffer> Bubble_Particles;
Particle_Renderer<Smoke_Particle_Info, Model_Vertex_Buffer> Colour_Bubble_Particles;

Particle_Renderer<Muzzle_Flash_Particle_Info, Billboard_Vertex_Buffer> Muzzle_Flash_Particles;

Particle_Renderer<Spark_Particle_Info, Billboard_Vertex_Buffer> Spark_Particles;

#endif
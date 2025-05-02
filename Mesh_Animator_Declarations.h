#ifndef MESH_ANIMATOR_DECLARATIONS
#define MESH_ANIMATOR_DECLARATIONS

// https://db.in.tum.de/~finis/x86-intrin-cheatsheet-v2.1.pdf

#include "OpenGL_Declarations.h"
#include "Vertex_Buffer_Declarations.h"
#include "Quaternion.h"
#include "Job_System_Declarations.h"

#include "Float_Text_Encoder.h"

//

struct Keyframe_Vertex // This requires less memory than a typical model vertex
{
	glm::vec3 Position, Normal;
};

struct Mesh_Animation
{
	float Duration, Tickrate;
	std::vector<std::vector<Keyframe_Vertex>> Keyframes;
};

#define ANIMF_TO_BE_DELETED 0u
#define ANIMF_LOOP_BIT 1u
#define NUMBER_OF_ANIMATION_WORKERS 6u

#define MAX(x, y) (x > y ? x : y)
#define MIN(x, y) (x < y ? x : y)

class Mesh_Animator;

struct Worker_Mesh_Animator_Info
{
	Mesh_Animator* Animator;
	Model_Vertex_Buffer* Mesh;
	size_t Offset, Keyframe_Index;
	float Time_Scalar;

	Worker_Mesh_Animator_Info(Mesh_Animator* Animatorp, Model_Vertex_Buffer* Meshp, size_t Offsetp, size_t Keyframe_Indexp, float Time_Scalarp)
	{
		Animator = Animatorp;
		Mesh = Meshp;
		Offset = Offsetp;
		Keyframe_Index = Keyframe_Indexp;
		Time_Scalar = Time_Scalarp;
	}
};

void Execute_Mesh_Animator_Animation(void* Parameter);

class Mesh_Animator
{
public:
	float Time = 0.0f;

	bool Flags[2] = { false, true };

	Mesh_Animation* Animation; // This will certainly be shared between multiple mesh_animators, so storing it in a cache for all to use is wise

	void Handle_Update(Model_Vertex_Buffer* Mesh)
	{
		size_t Keyframe_Index = MIN(Time * Animation->Tickrate, Animation->Keyframes.size() - 2);

		float Time_Scalar = MIN(Time * Animation->Tickrate, Animation->Keyframes.size() - 1) - ((float)Keyframe_Index);

		__m256 Time_Vector = _mm256_set1_ps(Time_Scalar);					// We can just set both of these before we start multiplying
		__m256 Opposite_Time_Vector = _mm256_set1_ps(1.0f - Time_Scalar);

		char* A_Source = (char*)Animation->Keyframes[Keyframe_Index].data();
		char* B_Source = (char*)Animation->Keyframes[Keyframe_Index + 1].data();

		Model_Vertex* Destination = Mesh->Mesh->Vertices.data();

		__m256 A, B, Interpolated_Vector, Expanded;

		for (size_t V = 0; V < Mesh->Mesh->Vertices.size(); V++)
		{
			A = _mm256_mul_ps(Opposite_Time_Vector, *(__m256*)A_Source);
			B = _mm256_mul_ps(Time_Vector, *(__m256*)B_Source);

			Interpolated_Vector = _mm256_add_ps(A, B);
			memcpy(Destination, &Interpolated_Vector, 24);

			Destination++;
			A_Source += 24;
			B_Source += 24;
		}
	}

	void Handle_Update_Threaded(Model_Vertex_Buffer* Mesh)
	{
		size_t Keyframe_Index = MIN(Time * Animation->Tickrate, Animation->Keyframes.size() - 2);

		float Time_Scalar = MIN(Time * Animation->Tickrate, Animation->Keyframes.size() - 1) - ((float)Keyframe_Index);

		// Mesh->Update_Vertices();

		for (size_t W = 0; W < NUMBER_OF_ANIMATION_WORKERS; W++)
			Job_System::Submit_Job(Job_System::Job(Execute_Mesh_Animator_Animation, new Worker_Mesh_Animator_Info(this, Mesh, W, Keyframe_Index, Time_Scalar)));

		// Execute_Mesh_Animator_Animation(new Worker_Mesh_Animator_Info(this, Mesh, NUMBER_OF_WORKERS - 1, Keyframe_Index, Time_Scalar));
	}

	bool Animate_Mesh(Model_Vertex_Buffer* Mesh, float Increment, bool Threaded = false, bool Update_Mesh = true) /*	You're able to tell the engine *not* to update the mesh once the animation is incremented. This can be done to save processing time when the object is offscreen for example */
	{
		// Since it gives a keyframe every tick, we can use a kind of array indexing to get the keyframe indices quickly

		bool Loop_Flag = false;

		Time += Increment;

		if (Time * Animation->Tickrate >= (Animation->Duration) || Time < 0)
		{
			if (Flags[ANIMF_LOOP_BIT] || Time < 0)
				Time = 0;
			else
				Time = (Animation->Duration) / Animation->Tickrate;

			Loop_Flag = true;
		}

		if (Update_Mesh)
		{
			if (Threaded)
				Handle_Update_Threaded(Mesh);
			else
				Handle_Update(Mesh);
		}

		return Loop_Flag;
	}
};

void Execute_Mesh_Animator_Animation(void* Parameter)
{
	Worker_Mesh_Animator_Info* Info = (Worker_Mesh_Animator_Info*)Parameter;

	__m256 Time_Vector = _mm256_set1_ps(Info->Time_Scalar);					// We can just set both of these before we start multiplying
	__m256 Opposite_Time_Vector = _mm256_set1_ps(1.0f - Info->Time_Scalar);

	Keyframe_Vertex* A_Source = Info->Animator->Animation->Keyframes[Info->Keyframe_Index].data() + Info->Offset;
	Keyframe_Vertex* B_Source = Info->Animator->Animation->Keyframes[Info->Keyframe_Index + 1].data() + Info->Offset;

	Model_Vertex* Destination = Info->Mesh->Mesh->Vertices.data() + Info->Offset;

	__m256 A, B, Interpolated_Vector, Expanded;

	for (size_t W = Info->Offset; W < Info->Animator->Animation->Keyframes[Info->Keyframe_Index].size(); W += NUMBER_OF_ANIMATION_WORKERS)
	{
		A = _mm256_mul_ps(Opposite_Time_Vector, *(__m256*)A_Source);
		B = _mm256_mul_ps(Time_Vector, *(__m256*)B_Source);

		Interpolated_Vector = _mm256_add_ps(A, B);

		memcpy(Destination, &Interpolated_Vector, 24u);

		Destination += NUMBER_OF_ANIMATION_WORKERS;
		A_Source += NUMBER_OF_ANIMATION_WORKERS;
		B_Source += NUMBER_OF_ANIMATION_WORKERS;
	}

	delete Info;
}

//

void Load_Animation_File(const char* Directory, Mesh_Animation* Animation)
{
	std::ifstream File(Directory);

	std::string Line;

	if (File.is_open())
	{
		std::getline(File, Line);
		Animation->Tickrate = std::stoi(Line);

		float Step;
		std::getline(File, Line);
		Step = 1.0f / std::stoi(Line);

		std::getline(File, Line);
		Animation->Duration = std::stoi(Line);

		Animation->Tickrate *= Step;
		Animation->Duration *= Step;

		while (std::getline(File, Line))
		{
			if (Line.length() == 0)
			{
				Animation->Keyframes.push_back(std::vector<Keyframe_Vertex>(0));
				continue;
			}

			//std::stringstream Stream(Line);

			Keyframe_Vertex Vertex;

			// Update to use new float-test-encoding for reading each value from the files

			Vertex.Position.x = Encoder::Characters_To_Float(&Line.c_str()[0]);
			Vertex.Position.y = Encoder::Characters_To_Float(&Line.c_str()[6]);
			Vertex.Position.z = Encoder::Characters_To_Float(&Line.c_str()[12]);

			Vertex.Normal.x = Encoder::Characters_To_Float(&Line.c_str()[18]);
			Vertex.Normal.y = Encoder::Characters_To_Float(&Line.c_str()[24]);
			Vertex.Normal.z = Encoder::Characters_To_Float(&Line.c_str()[30]);


			// Stream >> Vertex.Position.x >> Vertex.Position.y >> Vertex.Position.z >> Vertex.Normal.x >> Vertex.Normal.y >> Vertex.Normal.z;

			Animation->Keyframes.back().push_back(Vertex);

		}

		File.close();
	}
	else
		Throw_Error(" >> Unable to load animation file!\n");
}

#endif
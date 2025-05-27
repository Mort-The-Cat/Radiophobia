#ifndef PHYSICS_ENGINE_VISIONGL
#define PHYSICS_ENGINE_VISIONGL

#include "OpenGL_Declarations.h"
#include "Job_System_Declarations.h"
#include "Model_Declarations.h"
#include "Hitdetection.h"

#include "Audio_Handler_Declarations.h"

#include "Hitdetection_Blockmap_Declarations.h"

#include "Particle_System_Declarations.h"

void Wait_On_Physics();

float squaref(float X) { return X * X; }

namespace Physics
{
	class Physics_Object;

	class Impulse_Object
	{
	public:
		glm::vec3 A_Position, B_Position;
		glm::vec3 A_Velocity, B_Velocity;
		
		//Quaternion::Quaternion A_Rotational_Velocity, B_Rotational_Velocity;

		glm::vec3 A_Rotation_Vector, B_Rotation_Vector;

		Hitbox* B_Hitbox;

		Physics_Object* A = nullptr;
		Physics_Object* B = nullptr;
		Collision_Info Collision;
	};

#define PF_TO_BE_DELETED 0u
#define PF_NO_ROTATION 1u
#define PF_PLAYER_OBJECT 2u

	class Physics_Object
	{
	public:
		Model* Object;

		Audio::Audio_Source* SFX;

		glm::vec3 Forces = glm::vec3(0, 0, 0);
		glm::vec3 Velocity = glm::vec3(0, 0, 0);

		// Quaternion::Quaternion Rotational_Velocity = Quaternion::Quaternion(1.0f, 0.0f, 0.0f, 0.0f);

		glm::vec3 Rotation_Vector = glm::vec3(0.0f); // This is a vector where its direction is the axis of rotation and its magnitude is the angular velocity. I think that adding velocities with this etc should be way easier and work better than a quaternion
		
		glm::vec3 Centre_Of_Mass = glm::vec3(0, 0, 0);

		float Mass, Inv_Mass;

		float Elasticity; // Bounciness

		float Friction = 0.9f; // The product of two objects friction and the perpendicular velocity is equal and opposite to the force created by friction.

		bool Flags[3] = { false, false, false };

		glm::vec3 Get_Rotational_Velocity_Tangent(Quaternion::Quaternion Object_Rotation, glm::vec3 Object_Position, Impulse_Object* Collision)
		{
			glm::vec3 To_Collision_Vector = Collision->Collision.Collision_Position - (Object_Position + Centre_Of_Mass);

			float Length = std::sqrtf(glm::dot(To_Collision_Vector, To_Collision_Vector));
			To_Collision_Vector *= 1.0f / Length;

			glm::vec4 Axis_Angle = Quaternion::Get_Axis_Angle(Object_Rotation, To_Collision_Vector);

			return Quaternion::Get_Tangent(Axis_Angle, To_Collision_Vector) * (Axis_Angle.w * Length * 3.14159f * 2.0f); // Beautiful
		}

		glm::vec3 Get_Rotation_Vector_Tangent(glm::vec3 Rotation_Vector, glm::vec3 Object_Position, Impulse_Object* Collision)
		{
			glm::vec3 To_Collision_Vector = (Object_Position + Centre_Of_Mass) - Collision->Collision.Collision_Position;

			return glm::cross(To_Collision_Vector, Rotation_Vector); // *6.28318f;
		}

		virtual void Resolve_Collision(Impulse_Object* Collision)
		{
			glm::vec3 A_Velocity = Collision->A_Velocity;
			glm::vec3 B_Velocity = Collision->B != nullptr ? Collision->B_Velocity : glm::vec3(0, 0, 0);

			float B_Friction = Collision->B != nullptr ? Collision->B->Friction : 0.9f;

			//

			glm::vec3 A_Rotational_Velocity_Tangent = Get_Rotation_Vector_Tangent(Collision->A_Rotation_Vector, Collision->A_Position, Collision);

			glm::vec3 B_Rotational_Velocity_Tangent = Collision->B != nullptr ? Collision->B->Get_Rotation_Vector_Tangent(Collision->B_Rotation_Vector, Collision->B_Position, Collision) : glm::vec3(0.0f);

			glm::vec3 Relative_Velocity = B_Velocity - A_Velocity + (A_Rotational_Velocity_Tangent - B_Rotational_Velocity_Tangent);

			float Normal_Speed = glm::dot(Relative_Velocity, -Collision->Collision.Collision_Normal);

			glm::vec3 Perpendicular_Velocity = Relative_Velocity + Normal_Speed * Collision->Collision.Collision_Normal;

			glm::vec3 Impulse(0, 0, 0);

			float J = 0;

			if (Normal_Speed > 0)
			{
				float A_Inv_Mass = Inv_Mass;
				float B_Inv_Mass = Collision->B != nullptr ? Collision->B->Inv_Mass : 0.0f;

				float Inv_Combined_Mass = 1.0f / (A_Inv_Mass + B_Inv_Mass);

				float E = Elasticity * (Collision->B != nullptr ? Collision->B->Elasticity : 1.0f);

				J = -(1.0f + E) * Normal_Speed * Inv_Combined_Mass;

				Impulse = J * Collision->Collision.Collision_Normal + Perpendicular_Velocity * B_Friction * Friction * (1 - expf(J));

				if (!(std::isnan(Impulse.x) || std::isnan(Impulse.y) || std::isnan(Impulse.z)))
					Forces += Impulse;

				// Billboard_Smoke_Particles.Particles.Spawn_Particle(Collision->Collision.Collision_Position, glm::vec3(0.0f, 0.0f, 0.0f));
			}

			float B_Active = Collision->B != nullptr;

			//

			glm::vec3 Perpendicular_Rotational_Force = Collision->Collision.Collision_Normal * (0.25f * Collision->A->Friction * B_Friction * glm::dot(Collision->Collision.Collision_Normal, Collision->A_Rotation_Vector - Collision->B_Rotation_Vector));

			//

			glm::vec3 To_Collision = Collision->Collision.Collision_Position - (Centre_Of_Mass + Collision->A_Position);

			glm::vec3 Rotation_Axis = glm::normalize(glm::cross(Impulse, To_Collision));

			glm::vec3 Perpendicular = glm::normalize(glm::cross(Rotation_Axis, To_Collision));

			float Perpendicular_Force = 0.5f * glm::dot(Impulse, Perpendicular);

			float Rotational_Acceleration_Angle = Inv_Mass * Perpendicular_Force / glm::length(To_Collision);


			if (!std::isnan(Rotational_Acceleration_Angle) && !Flags[PF_NO_ROTATION])
			{
				Rotation_Vector -= Rotation_Axis * Rotational_Acceleration_Angle
					+ Perpendicular_Rotational_Force * Inv_Mass;
			}

			// https://en.wikipedia.org/wiki/Angular_acceleration#Relation_to_torque

			// p. force / (radius * mass) = rotational acceleration
			
			//

			glm::vec3 Delta = Collision->Collision.Collision_Normal * (Fast::Add_Epsilon(Collision->Collision.Overlap, 0) / (1.0f + B_Active));

			Object->Position += Delta;

			if (Collision->B != nullptr) // Apply the forces/deltas to B accordingly (but only if B is a physics object itself) 
			{
				Collision->B->Forces -= Impulse;

				Collision->B->Object->Position -= Delta;

				//

				To_Collision = Collision->Collision.Collision_Position - (Collision->B->Centre_Of_Mass + Collision->B_Position);
				Rotation_Axis = glm::normalize(glm::cross(-Impulse, To_Collision));
				Perpendicular = glm::normalize(glm::cross(Rotation_Axis, To_Collision));

				Perpendicular_Force = 0.5f * glm::dot(Impulse, Perpendicular);
				Rotational_Acceleration_Angle = Collision->B->Inv_Mass * Perpendicular_Force / glm::length(To_Collision);

				if (!std::isnan(Rotational_Acceleration_Angle) && !Collision->B->Flags[PF_NO_ROTATION])
				{
					Collision->B->Rotation_Vector += Rotation_Axis * Rotational_Acceleration_Angle
						+ Perpendicular_Rotational_Force * Collision->B->Inv_Mass;
				}
			}
		}

		void Stabilise_On_Ground()
		{
			const float Range = 0.1f; // in both directions

			float Min = Object->Position.y - Range, Max = Object->Position.y + Range;

			glm::vec3 Player_Position = Object->Position;

			Collision_Info Info;

			Hitbox* Collided_Hitbox;

			Object->Position.y = Max; // We'll just do a little test with the lowest value to see if the floor is within the player's reach

			Info = Collision_Test::Find_Collision(Object->Hitboxes[0], Collision_Test::Always_Compare, &Collided_Hitbox, Collision_Test::Only_Return_Floor);

			Object->Position.y = Player_Position.y; // There's no worries, just set the player's position back to where it was

			if (Collided_Hitbox != nullptr)
			{
				size_t Iterations = 12;

				while (Iterations)
				{
					Info = Collision_Test::Find_Collision(Object->Hitboxes[0], Collision_Test::Always_Compare, &Collided_Hitbox, Collision_Test::Only_Return_Floor);

					if (Collided_Hitbox != nullptr) // We must go higher
						Max = Object->Position.y;
					else // we can go lower
						Min = Object->Position.y;

					Object->Position.y = 0.5f * (Min + Max);

					Iterations--;
				}

				Object->Position.y = Max;

				// Velocity.y = 0.0f;
			}

			//

			/*Info = Collision_Test::Find_Collision(Object->Hitboxes[0], Collision_Test::Always_Compare, &Collided_Hitbox);

			size_t Simple_Check = 6;

			while (Collided_Hitbox != nullptr && Simple_Check)
			{
				Object->Position += Info.Collision_Normal * Info.Overlap;

				Velocity -= Info.Collision_Normal * glm::dot(Info.Collision_Normal, Velocity);
				
				Info = Collision_Test::Find_Collision(Object->Hitboxes[0], Collision_Test::Always_Compare, &Collided_Hitbox);

				Simple_Check--;
			}*/
		}

		virtual void Step()
		{
			float Gravity = Tick * 4.9;

			float Force_Magnitude = glm::dot(Forces, Forces);

			SFX->Position = Object->Position;

			if (Force_Magnitude > Mass * 4.9)
			{
				SFX->Play_Sound(Pull_Audio("Assets/Audio/Step.wav").Source);
				SFX->Volume = 0.25 * std::fminf(10, Fast::Sqrt(Force_Magnitude));
				SFX->Sounds.back()->setPlaybackSpeed(RNG() * 0.25 + 1);
			}

			Quaternion::Quaternion Interpolated_Rotation = Quaternion::Rotation_Vector_To_Quaternion(Rotation_Vector * (Tick));

			// Quaternion::Quaternion Interpolated_Rotation = Quaternion::Sphere_Interpolate(Quaternion::Quaternion(1.0f, 0.0f, 0.0f, 0.0f), Rotational_Velocity, 180 * Tick);

			Object->Orientation = glm::normalize(Quaternion::Rotate(Interpolated_Rotation, Object->Orientation));
			Object->Orientation_Up = glm::normalize(Quaternion::Rotate(Interpolated_Rotation, Object->Orientation_Up));

			// if (glm::dot(Rotation_Vector, Rotation_Vector) < 0.1f)
			//	Rotation_Vector *= powf(0.25f, Tick);

			// Rotation_Vector *= powf(0.9, Tick);

			// Rotational_Velocity = Quaternion::Sphere_Interpolate(Quaternion::Quaternion(1.0f, 0.0f, 0.0f, 0.0f), Rotational_Velocity, std::powf(0.9f, Tick));

			// This provides a bit of damping to the rotational velocity to prevent wobbling

			for(size_t W = 0; W < Object->Hitboxes.size(); W++)
				Object->Hitboxes[W]->Update_Hitbox();

			//Forces.y += Gravity * Mass;

			if (std::isnan(Forces.x) || std::isnan(Forces.y) || std::isnan(Forces.z))		// This only happens in really rare situations such as physics engine race conditions
				Forces = glm::vec3(0.0f);

			Forces *= Inv_Mass * 0.5;

			Velocity += Forces;
			Velocity.y += Gravity;
			
			Object->Position += Velocity * abs(Tick);

			Velocity += Forces;
			Velocity.y += Gravity;

			Forces.x = 0;
			Forces.y = 0;
			Forces.z = 0;
		}
	};

	std::vector<Physics_Object*> Scene_Physics_Objects;

	std::mutex Recorded_Impulses_Mutex;
	std::vector<Impulse_Object> Recorded_Impulses;

	std::mutex Threads_Working_Count_Mutex;
	char Threads_Working_On_Physics = 0;

	bool Check_Duplicate_Collisions(Physics::Physics_Object* Object, glm::vec3 Collision_Normal) // This function assumes that the recorded impulses mutex is already locked
	{
		bool Duplicate = false;

		for (size_t W = 0; W < Recorded_Impulses.size(); W++)
		{
			/*if (Recorded_Impulses[W].B == nullptr &&
				Recorded_Impulses[W].A == Object && 
				Recorded_Impulses[W].Collision.Collision_Normal == Collision_Normal)
			{
				return true;
			}*/

			Duplicate |=
				Recorded_Impulses[W].B == nullptr &&
				Recorded_Impulses[W].A == Object &&
				Recorded_Impulses[W].Collision.Collision_Normal == Collision_Normal;
		}

		return Duplicate;
	}

	void Force_Object_Delta_Resolve(Impulse_Object* Collision)
	{
		Collision_Info Info = Collision->A->Object->Hitboxes[0]->Hitdetection(Collision->B_Hitbox);

		if (Info.Overlap < 0.0f)
		{
			// printf("Forcing object overlap! %f\n", Info.Overlap);

			glm::vec3 Delta = Info.Collision_Normal * Info.Overlap;

			if (Collision->B != nullptr)
			{
				Delta *= 0.5f;

				Collision->A->Object->Position += Delta;
				Collision->B->Object->Position -= Delta;
			}
			else
			{
				Collision->A->Object->Position += Delta;
			}

			Force_Object_Delta_Resolve(Collision);
		}
	}

	void Job_Record_Collisions(void* Data)
	{
		size_t* Offset = (size_t*)Data;

		for (size_t W = 0; W < Scene_Physics_Objects.size(); W++)
		{
			for (size_t V = W + 1 + *Offset; V < Scene_Physics_Objects.size(); V += NUMBER_OF_WORKERS)
			{
				Impulse_Object Impulse;

				Impulse.Collision = Scene_Physics_Objects[W]->Object->Hitboxes[0]->Hitdetection(Scene_Physics_Objects[V]->Object->Hitboxes[0]);
				
				Impulse.B_Hitbox = Scene_Hitboxes[V];

				Impulse.A = Scene_Physics_Objects[W];
				Impulse.B = Scene_Physics_Objects[V];

				Impulse.A_Position = Scene_Physics_Objects[W]->Object->Position;
				Impulse.B_Position = Scene_Physics_Objects[V]->Object->Position;

				Impulse.A_Velocity = Scene_Physics_Objects[W]->Velocity;
				Impulse.B_Velocity = Scene_Physics_Objects[V]->Velocity;

				Impulse.A_Rotation_Vector = Scene_Physics_Objects[W]->Rotation_Vector;
				Impulse.B_Rotation_Vector = Scene_Physics_Objects[V]->Rotation_Vector;

				if (Impulse.Collision.Overlap != 0)
				{
					Recorded_Impulses_Mutex.lock();
					Recorded_Impulses.push_back(Impulse);
					Recorded_Impulses_Mutex.unlock();
				}
			}

			if constexpr (Should_Use_Blockmap)
			{
				std::vector<Hitbox*> Nearby_Hitboxes = Blockmap::Read_Blockmap(Scene_Physics_Objects[W]->Object->Position);

				for (size_t V = *Offset; V < Nearby_Hitboxes.size(); V += NUMBER_OF_WORKERS)
				{
					Impulse_Object Impulse;

					Impulse.Collision = Scene_Physics_Objects[W]->Object->Hitboxes[0]->Hitdetection(Nearby_Hitboxes[V]);

					Impulse.B_Hitbox = Nearby_Hitboxes[V];

					Impulse.A = Scene_Physics_Objects[W];
					Impulse.B = nullptr;

					Impulse.A_Position = Scene_Physics_Objects[W]->Object->Position;
					Impulse.B_Position = *Nearby_Hitboxes[V]->Position;

					Impulse.A_Velocity = Scene_Physics_Objects[W]->Velocity;

					Impulse.A_Rotation_Vector = Scene_Physics_Objects[W]->Rotation_Vector;

					if (Impulse.Collision.Overlap != 0)
					{
						Recorded_Impulses_Mutex.lock();

						// We'll add basic checks here to make sure that physics objects don't collide with "the same" object twice in a row so to speak.
						// Hitboxes can overlap, causing the physics engine to apply a repulsive force multiple times, ruining the physics.


						if (!Check_Duplicate_Collisions(Impulse.A, Impulse.Collision.Collision_Normal)) // This ensures that the impulse is only recorded if it's not a duplicate.
							Recorded_Impulses.push_back(Impulse);

						Recorded_Impulses_Mutex.unlock();
					}
				}
			}
			else
			{
				for (size_t V = *Offset + Scene_Physics_Objects.size(); V < Scene_Hitboxes.size(); V += NUMBER_OF_WORKERS)
				{
					//if (Scene_Physics_Objects[W]->Flags[PF_PLAYER_OBJECT])
					//	continue;

					Impulse_Object Impulse;

					Impulse.Collision = Scene_Physics_Objects[W]->Object->Hitboxes[0]->Hitdetection(Scene_Hitboxes[V]);

					Impulse.B_Hitbox = Scene_Hitboxes[V];

					Impulse.A = Scene_Physics_Objects[W];
					Impulse.B = nullptr;

					Impulse.A_Position = Scene_Physics_Objects[W]->Object->Position;
					Impulse.B_Position = *Scene_Hitboxes[V]->Position;

					Impulse.A_Velocity = Scene_Physics_Objects[W]->Velocity;

					Impulse.A_Rotation_Vector = Scene_Physics_Objects[W]->Rotation_Vector;

					if (Impulse.Collision.Overlap != 0)
					{
						Recorded_Impulses_Mutex.lock();

						// We'll add basic checks here to make sure that physics objects don't collide with "the same" object twice in a row so to speak.
						// Hitboxes can overlap, causing the physics engine to apply a repulsive force multiple times, ruining the physics.


						if (!Check_Duplicate_Collisions(Impulse.A, Impulse.Collision.Collision_Normal)) // This ensures that the impulse is only recorded if it's not a duplicate.
							Recorded_Impulses.push_back(Impulse);

						Recorded_Impulses_Mutex.unlock();
					}
				}
			}
		}

		Threads_Working_Count_Mutex.lock();
		Threads_Working_On_Physics--;
		Threads_Working_Count_Mutex.unlock();

		delete Offset;
	}

	void Job_Resolve_Collisions(void* Data)
	{
		size_t* Offset = (size_t*)Data;

		for (size_t W = *Offset; W < Recorded_Impulses.size(); W += NUMBER_OF_WORKERS)
			Recorded_Impulses[W].A->Resolve_Collision(&Recorded_Impulses[W]);
		
		//for (size_t W = *Offset; W < Scene_Physics_Objects.size(); W += NUMBER_OF_WORKERS)
		//	Scene_Physics_Objects[W]->Step();

		Threads_Working_Count_Mutex.lock();
		Threads_Working_On_Physics--;
		Threads_Working_Count_Mutex.unlock();

		delete Offset;
	}

	void Record_Collisions()
	{
		if (!Engine_Continue_Looping)
			return;

		Wait_On_Physics();

		Threads_Working_Count_Mutex.lock();
		Threads_Working_On_Physics = NUMBER_OF_WORKERS;
		Threads_Working_Count_Mutex.unlock();

		Recorded_Impulses.clear(); // I'm so dumb XD I forgot to include this!

		for (size_t W = 0; W < Scene_Physics_Objects.size(); W++)
			Scene_Physics_Objects[W]->Step();

		for (size_t W = 0; W < NUMBER_OF_WORKERS; W++)
			Job_System::Submit_Job(Job_System::Job(Job_Record_Collisions, new size_t(W)));
	}

	void Resolve_Collisions()
	{
		Wait_On_Physics();

		Threads_Working_Count_Mutex.lock();
		Threads_Working_On_Physics = NUMBER_OF_WORKERS;
		Threads_Working_Count_Mutex.unlock();

		for (size_t W = 0; W < NUMBER_OF_WORKERS; W++)
			Job_System::Submit_Job(Job_System::Job(Job_Resolve_Collisions, new size_t(W)));

		Wait_On_Physics();

		// Then we'll do one last thing to resolve the collisions thoroughly

		// for (size_t W = 0; W < Recorded_Impulses.size(); W++)
		//	Force_Object_Delta_Resolve(&Recorded_Impulses[W]);

		// printf("Number of impulses: %d\n", Recorded_Impulses.size());
	}
}

void Initialise_Physics_Object(Physics::Physics_Object* Physics_Info, Model* Object)
{
	Physics_Info->Mass = 1.0f;
	Physics_Info->Inv_Mass = 1.0f;
	Physics_Info->Elasticity = 0.9f;

	Physics::Scene_Physics_Objects.push_back(Physics_Info);

	Physics_Info->SFX = Audio::Create_Audio_Source(Object->Position, 1);

	Physics_Info->Object = Object;
}

class Physics_Object_Controller : public Controller
{
public:
	Physics::Physics_Object* Physics_Info;

	// Lightsource* Light;

	float Time = 10;

	virtual void Initialise_Control(Model* Objectp) override
	{
		Object = Objectp;

		Physics_Info = new Physics::Physics_Object();

		Initialise_Physics_Object(Physics_Info, Object);
	}

	virtual void Control_Function() override
	{
		Time -= Tick;

		bool Should_Delete = Object->Position.y > 5 || Time < 0;

		Object->Flags[MF_TO_BE_DELETED] |= Should_Delete;
		Physics_Info->Flags[PF_TO_BE_DELETED] |= Should_Delete;
		Object->Hitboxes[0]->Flags[HF_TO_BE_DELETED] |= Should_Delete;
		Physics_Info->SFX->Flags[ASF_TO_BE_DELETED] |= Should_Delete;
	}
};

void Wait_On_Physics()
{
	bool Still_Working = false;

	while (Job_System::Part_Time_Work()) { ; }

	do
	{
		Physics::Threads_Working_Count_Mutex.lock();
		Still_Working = Physics::Threads_Working_On_Physics > 0;
		Physics::Threads_Working_Count_Mutex.unlock();
	} while (Still_Working);
}

#endif

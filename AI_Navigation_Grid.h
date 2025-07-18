#ifndef VISION_AI_NAVIGATION_GRID
#define VISION_AI_NAVIGATION_GRID

#include "OpenGL_Declarations.h"

#include "Physics_Engine.h"

#include "Hitdetection_Blockmap_Declarations.h"

namespace Navigation
{
	// Vom Anfang bis zum Ziel.

#define NAV_GRID_MASK 0xFFFFu

	// If the uint8_t distance is 255u, that means that the grid block is a wall and cannot be traversed.

	class Grid
	{
	public:
		std::vector<uint16_t> Distances; // These are the distances from Ziel
	} Map_Grid;

	class Pathfinding_Grid
	{
	public:
		Grid Navigation_Grid;

		size_t Z_Index; // This is the index of the grid block that the Ziel is in.
	} Saved_Grid;

	std::mutex Saved_Grid_Mutex;

	const constexpr float Block_Size = 0.2f;

	size_t Navigation_Grid_Width;
	size_t Navigation_Grid_Height;

	glm::vec2 Navigation_Grid_A;	// This is the navigation grid "origin"
	glm::vec2 Navigation_Grid_B;

	void Initialise_Level_Navigation_Grid(float Actor_Position, float Actor_Height)
	{
		Map_Grid.Distances.clear();
		Saved_Grid.Z_Index = 0xFFFFFFFFu;

		Navigation_Grid_A = glm::vec3( 99999.9f);
		Navigation_Grid_B = glm::vec3(-99999.9f);

		for (size_t W = Physics::Scene_Physics_Objects.size(); W < Scene_Hitboxes.size(); W++)
		{
			Blockmap_Bounds Bounds = Scene_Hitboxes[W]->Get_Bounds();

			Navigation_Grid_A.x = MIN(Bounds.A.x, Navigation_Grid_A.x);
			Navigation_Grid_A.y = MIN(Bounds.A.y, Navigation_Grid_A.y);

			Navigation_Grid_B.x = MAX(Bounds.B.x, Navigation_Grid_B.x);
			Navigation_Grid_B.y = MAX(Bounds.B.y, Navigation_Grid_B.y);
		}

		Navigation_Grid_A -= glm::vec2(2.0f);
		Navigation_Grid_B += glm::vec2(2.0f);

		// This gives a bit of leeway to prevent errors in the nav grid

		Navigation_Grid_B -= Navigation_Grid_A;

		Navigation_Grid_Width = std::ceilf(Navigation_Grid_B.x / Block_Size);
		Navigation_Grid_Height = std::ceilf(Navigation_Grid_B.y / Block_Size);

		Navigation_Grid_B += Navigation_Grid_A;

		// This sets the dimensions of the AI navigation grid

		Map_Grid.Distances.resize(Navigation_Grid_Width * Navigation_Grid_Height);

		glm::vec3 Block_Position = glm::vec3(0.0f);

		Block_Position.y = Actor_Position;

		AABB_Hitbox Block;

		Block.A = glm::vec3(0, -Actor_Height, 0);
		Block.B = glm::vec3(Block_Size, 0, Block_Size);

		Block.Object = nullptr;

		Block.Position = &Block_Position;

		Block_Position.x = Navigation_Grid_A.x;

		for (size_t X = 0; X < Navigation_Grid_Width; X++, Block_Position.x += Block_Size)
		{
			Block_Position.z = Navigation_Grid_A.y;

			for (size_t Z = 0; Z < Navigation_Grid_Height; Z++, Block_Position.z += Block_Size)
			{
				Hitbox* Collided_Hitbox = nullptr;

				Collision_Test::Find_Collision(&Block, Collision_Test::Always_Compare, &Collided_Hitbox);

				uint16_t Wall;

				if (Collided_Hitbox)
					Wall = NAV_GRID_MASK;
				else
					Wall = NAV_GRID_MASK - 1;

				Map_Grid.Distances[Z * Navigation_Grid_Width + X] = Wall;
			}
		}
	}

	//

	inline void Check_Flood_Fill_Iteration(
		size_t* Indices_Data, size_t Z_Index, uint16_t Distance, Pathfinding_Grid* Pathfinding,
		unsigned char* Current_Indices, unsigned char* Next_Indices, unsigned char* End_Next_Indices)
	{
		if (Z_Index >= Map_Grid.Distances.size())
			return;

		if (Pathfinding->Navigation_Grid.Distances[Z_Index] > Distance && (Pathfinding->Navigation_Grid.Distances[Z_Index] != NAV_GRID_MASK))
		{
			Indices_Data[*End_Next_Indices] = Z_Index;
			(*End_Next_Indices)++;
			Pathfinding->Navigation_Grid.Distances[Z_Index] = Distance;
		}
	}

	void Pathfinding_Fill(size_t Z_Index, Pathfinding_Grid* Pathfinding)
	{
		uint16_t Distance = 1;

		size_t Indices_Data[256];														// This looping queue saves soooo much time
		unsigned char Current_Indices = 0, Next_Indices = 1, End_Next_Indices = 1;
		Indices_Data[0] = Z_Index;

		Check_Flood_Fill_Iteration(Indices_Data, Z_Index, 0, Pathfinding, &Current_Indices, &Next_Indices, &End_Next_Indices);

		while (End_Next_Indices - Next_Indices && Distance)
		{
			Current_Indices = Next_Indices;
			Next_Indices = End_Next_Indices;
			for (unsigned char W = Current_Indices; W != Next_Indices; W++)
			{
				Check_Flood_Fill_Iteration(Indices_Data, Indices_Data[W] - Navigation_Grid_Width, Distance, Pathfinding,
					&Current_Indices, &Next_Indices, &End_Next_Indices);
				Check_Flood_Fill_Iteration(Indices_Data, Indices_Data[W] + Navigation_Grid_Width, Distance, Pathfinding,
					&Current_Indices, &Next_Indices, &End_Next_Indices);
				Check_Flood_Fill_Iteration(Indices_Data, Indices_Data[W] - 1, Distance, Pathfinding,
					&Current_Indices, &Next_Indices, &End_Next_Indices);
				Check_Flood_Fill_Iteration(Indices_Data, Indices_Data[W] + 1, Distance, Pathfinding,
					&Current_Indices, &Next_Indices, &End_Next_Indices);
			}

			Distance++;
		}
	}

	void Get_Pathfinding(float X, float Z, Pathfinding_Grid* Pathfinding) // These are the coordinates of the target object
	{
		// From here, we'll do the flood fill necessary

		size_t Z_Index = static_cast<size_t>((X - Navigation_Grid_A.x) / Block_Size) + static_cast<size_t>((Z - Navigation_Grid_A.y) / Block_Size) * Navigation_Grid_Width;

		//Saved_Grid_Mutex.lock();
		if (Saved_Grid.Z_Index == Z_Index)
		{
			Pathfinding->Navigation_Grid.Distances = Saved_Grid.Navigation_Grid.Distances;
			//Saved_Grid_Mutex.unlock();
			return;
		}
		//Saved_Grid_Mutex.unlock();

		Pathfinding->Z_Index = Z_Index;

		Pathfinding->Navigation_Grid.Distances = Map_Grid.Distances;

		// This copies the data into the nav grid

		Pathfinding_Fill(Z_Index, Pathfinding); // This fills the pathfinding grid

		Saved_Grid_Mutex.lock();
		Saved_Grid = *Pathfinding;
		Saved_Grid_Mutex.unlock();
	}

	inline void Select_Direction(glm::vec3& Direction_Vector, size_t Z_Index, float Scalar, uint16_t Distance, Pathfinding_Grid& Navigation, size_t Delta_Index)
	{
		struct Delta
		{
			glm::vec3 Vector;
			size_t Index;
		};

		Delta Deltas[4];
		Deltas[0].Vector = glm::vec3(0.0f, 0.0f, -Scalar);
		Deltas[0].Index = Z_Index - Navigation_Grid_Width;

		Deltas[1].Vector = glm::vec3(0.0f, 0.0f, Scalar);
		Deltas[1].Index = Z_Index + Navigation_Grid_Width;

		Deltas[2].Vector = glm::vec3(-Scalar, 0.0f, 0.0f);
		Deltas[2].Index = Z_Index - 1;

		Deltas[3].Vector = glm::vec3(Scalar, 0.0f, 0.0f);
		Deltas[3].Index = Z_Index + 1;

		for (size_t Count = 0; Count < 2; Count++, Delta_Index = (Delta_Index + 1) & 3u)
		{
			if (Navigation.Navigation_Grid.Distances[Deltas[Delta_Index].Index] < Distance)
			{
				Distance = Navigation.Navigation_Grid.Distances[Deltas[Delta_Index].Index];
				Direction_Vector += Deltas[Delta_Index].Vector;
				return;
			}
		}
	}

	glm::vec3 Get_Path_Direction(glm::vec3 Start, glm::vec3 End)
	{
		size_t Start_Index = static_cast<size_t>((Start.x - Navigation_Grid_A.x) / Block_Size) + static_cast<size_t>((Start.z - Navigation_Grid_A.y) / Block_Size) * Navigation_Grid_Width;

		Pathfinding_Grid Navigation;

		Get_Pathfinding(End.x, End.z, &Navigation);

		uint16_t Distance_From_Start = Navigation.Navigation_Grid.Distances[Start_Index];

		glm::vec3 Direction_Vector(0.0f);

		Select_Direction(Direction_Vector, Start_Index, 1.0f, Distance_From_Start, Navigation, 0);
		Select_Direction(Direction_Vector, Start_Index, 1.0f, Distance_From_Start, Navigation, 2);
		// Select_Direction(Direction_Vector, Start_Index, 0.33f, Distance_From_Start, Navigation);
		//Select_Direction(Direction_Vector, Start_Index, 0.25f, Distance_From_Start, Navigation);

		if (Direction_Vector.x == 0 && Direction_Vector.z == 0)
			return glm::vec3(0.0f);
		else
			return glm::normalize(Direction_Vector);

		/*if (Navigation.Navigation_Grid.Distances[Start_Index - Navigation_Grid_Width] < Distance_From_Start) // Check Z - 1
			return glm::vec3(0.0f, 0.0f, -1.0f);
		if (Navigation.Navigation_Grid.Distances[Start_Index + Navigation_Grid_Width] < Distance_From_Start) // Check Z + 1
			return glm::vec3(0.0f, 0.0f, 1.0f);
		if (Navigation.Navigation_Grid.Distances[Start_Index - 1] < Distance_From_Start) // Check X - 1
			return glm::vec3(-1.0f, 0.0f, 0.0f);
		if (Navigation.Navigation_Grid.Distances[Start_Index + 1] < Distance_From_Start)
			return glm::vec3(1.0f, 0.0f, 0.0f);

		return glm::vec3(0.0f); // WHAT*/
	}
}

#endif
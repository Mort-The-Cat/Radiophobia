#ifndef VISION_HITDETECTION_BLOCKMAP
#define VISION_HITDETECTION_BLOCKMAP

#include "Physics_Engine.h"

#include "Hitdetection_Blockmap_Declarations.h"

namespace Blockmap // This is for hitdetection
{
	std::vector<std::vector<std::vector<Hitbox*>>> Blockmap_Data;

	// 2d array of collections of hitboxes

	float Block_Size = 1.0f; // This is the "radius" of each grid of the blockmap

	size_t Blockmap_Width; // This is width in blocks (along the X axis)
	size_t Blockmap_Height; // This is height in blocks (along the Z axis)

	glm::vec2 Blockmap_A; // This is the blockmap "origin"
	glm::vec2 Blockmap_B;

	std::vector<Hitbox*> Read_Blockmap(const glm::vec3 Reader_Position)
	{
		int X = (Reader_Position.x - Blockmap_A.x) / Block_Size, Y = (Reader_Position.z - Blockmap_A.y) / Block_Size;

		X = MIN(MAX(0, X), Blockmap_Width - 1);
		Y = MIN(MAX(0, Y), Blockmap_Height - 1);

		std::vector<Hitbox*> Read_Hitboxes = Blockmap_Data[X][Y];

		for (int W = Scene_Hitboxes.size() - 1; W > 0; W--)
		{
			if (Scene_Hitboxes[W]->Flags[HF_IN_BLOCKMAP])
				return Read_Hitboxes;

			Read_Hitboxes.push_back(Scene_Hitboxes[W]);
		}

		return Read_Hitboxes;
	}

	void Initialise_Blockmap()
	{
		// Clears existing blockmap info if necessary

		Blockmap_Data.clear(); // That simple

		// Decides dimensions of blockmap

		Blockmap_A = glm::vec2(999.0f);
		Blockmap_B = glm::vec2(-999.0f); // This is important for the bounds of the blockmap

		for (size_t W = Physics::Scene_Physics_Objects.size(); W < Scene_Hitboxes.size(); W++)
		{
			Blockmap_Bounds Bounds = Scene_Hitboxes[W]->Get_Bounds();

			Blockmap_A.x = MIN(Blockmap_A.x, Bounds.A.x);
			Blockmap_A.y = MIN(Blockmap_A.y, Bounds.A.y);

			Blockmap_B.x = MAX(Blockmap_B.x, Bounds.B.x);
			Blockmap_B.y = MAX(Blockmap_B.y, Bounds.B.y);
		}

		Blockmap_B -= Blockmap_A;

		Blockmap_Width = std::ceilf(Blockmap_B.x / Block_Size);
		Blockmap_Height = std::ceilf(Blockmap_B.y / Block_Size);

		Blockmap_B += Blockmap_A;

		// Sets dimensions

		Blockmap_Data.resize(Blockmap_Width);
		for (size_t W = 0; W < Blockmap_Width; W++)
		{
			Blockmap_Data[W].resize(Blockmap_Height);

			for (size_t V = 0; V < Blockmap_Height; V++)
				Blockmap_Data[W][V].resize(0);
		}

		// Adds hitboxes to blockmap grid (EXCEPT for physics objects)

		for (size_t W = Physics::Scene_Physics_Objects.size(); W < Scene_Hitboxes.size(); W++)
		{
			Scene_Hitboxes[W]->Flags[HF_IN_BLOCKMAP] = true;

			Blockmap_Bounds Bounds = Scene_Hitboxes[W]->Get_Bounds();

			Bounds.A -= glm::vec2(Block_Size);
			Bounds.B += glm::vec2(Block_Size);

			Bounds.B.x = MIN(Bounds.B.x, Blockmap_B.x);
			Bounds.B.y = MIN(Bounds.B.y, Blockmap_B.y);

			Bounds.A.x = MAX(Bounds.A.x, Blockmap_A.x);
			Bounds.A.y = MAX(Bounds.A.y, Blockmap_A.y);

			Bounds.B -= Blockmap_A;
			Bounds.A -= Blockmap_A;

			for(float X = Bounds.A.x; X < Bounds.B.x; X += Block_Size)
				for (float Y = Bounds.A.y; Y < Bounds.B.y; Y += Block_Size)
					Blockmap_Data[floorf(X / Block_Size)][floorf(Y / Block_Size)].push_back(Scene_Hitboxes[W]);
		}

		// Nice!!
	}
}

#endif
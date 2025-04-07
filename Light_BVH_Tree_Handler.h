#ifndef VISION_LIGHT_BVH_TREE_HANDLER
#define VISION_LIGHT_BVH_TREE_HANDLER

#include "OpenGL_Declarations.h"
#include "Lighting_Handler.h"

// We will use integers to store the positions of the boundaries to save memory
// Each leaf node will have ~8 light sources, perhaps more
// The scene will either have 64-128 light sources at once.

// Later, I might make changes to the shadow mapping to allow for more than 1 shadow-mapped light source at once.

/*

		0			depth = 0
	  /	  \
	 1	   2		depth = 1
	/ \   / \
   3   4  5  6		depth = 2

   This tree has a depth of 3

   usw.

   number of nodes in binary tree = 2^(depth) - 1	or		(1u << depth) - 1u

   The index of the next node is always 2n + 1 + (side_of_partition).

   z.B. The left of 2 is equal to 2(2) + 1 + 0 = 5,
   and	the right of 2 is equal to 2(2) + 1 + 1 = 6

   and you can see this in the graph as well
*/

namespace Lighting_BVH // This uses considerably less memory than my previous design of the lighting BVH tree
{
	float Boundary_Max_Value = 22.0f;

	struct Node_Partition
	{
		float X = Boundary_Max_Value, Y = Boundary_Max_Value;
	};

	struct Leaf_Node
	{
		unsigned char Light_Indices[8]; // Each leaf node should have 8 light indices
	};

	struct Leaf_Node_Bounds // This is used only by the CPU-side to assess each node's optimal lights
	{
		LG Light_Group = LG::No_Group_Sector;
		glm::vec2 Position = glm::vec2(99999, 99999); // This is the assessed position of each node - changes can be made to this manually according to artist parameters
	};

	const size_t Binary_Tree_Depth = 7; // How many layers in the tree there are

	constexpr const size_t Number_Of_Partition_Nodes = (1u << (Binary_Tree_Depth - 1u)) - 1u;

	Node_Partition Partition_Nodes[Number_Of_Partition_Nodes]; // These are the structures that are passed to the GPU

	constexpr const size_t Number_Of_Leaf_Nodes = (1u << (Binary_Tree_Depth - 1u));

	Leaf_Node Leaf_Nodes[Number_Of_Leaf_Nodes]; // This structure is passed to the GPU, giving the indices of each leaf nodes' lights.

	//

	Leaf_Node_Bounds Leaf_Nodes_Info[Number_Of_Leaf_Nodes]; // Again, this is entirely CPU-side, used to assess each node's optimal lights

	//

	struct Boundary
	{
		float Min_X = -Boundary_Max_Value,
			Max_X = Boundary_Max_Value,
			Min_Y = -Boundary_Max_Value,
			Max_Y = Boundary_Max_Value;
		//char Min_X = -128, Max_X = 127, Min_Y = -128, Max_Y = 127;
	};

	class Light_Occluder
	{
	public:
		float Min, Max;
		float Partition;
		enum Axis
		{
			X = 0u,
			Z = 1u
		} Partition_Axis; // This states which axis the "partition" variable represents

		/*
			   (z)
				|
				|	   Max
				|		p
				|		p
				|		p
			---(o)------p-----(x)
				|		p
				|	   Min
				|
				|
			  (-z)
		*/

		// In this example, Partition_Axis == Axis::Z
		// Hence, Partition is a value on the X axis
		// And Min/Max are Z values

		// Simple enough

		bool Handle_Node(glm::vec2& Middle, glm::vec2& Max_N, glm::vec2& Min_N)
		{
			const float Required_Offset = 0.5f;

			switch (Partition_Axis)
			{
			case Axis::X:
				if (Max > Min_N.y || Min < Max_N.y)
				{
					float Left_Offset = Partition - Min_N.x;
					float Right_Offset = Max_N.x - Partition;

					if (Left_Offset > Required_Offset && Right_Offset > Required_Offset)
					{
						Middle.x = Partition;
						Max_N.y = Min_N.y;

						return true;
					}
				}

				return false;

			case Axis::Z:
				if (Max > Min_N.x || Min < Max_N.x)
				{
					float Left_Offset = Partition - Min_N.y;
					float Right_Offset = Max_N.y - Partition;

					if (Left_Offset > Required_Offset && Right_Offset > Required_Offset)
					{
						Middle.y = Partition;
						Max_N.x = Min_N.x;

						return true;
					}
				}

				return false;

			default:
				Throw_Error(" >> Light occluder not set properly!\n");

				return NULL;
			}
		}

		bool Colliding_X(float A_X, float A_Z, float B_X, float B_Z)
		{
			if ((A_X > Partition) == (B_X > Partition)) // If they're on the same side of the line,
				return false;							// we know it can't overlap with it

			// If they're not, we have some work to do!!

			float Delta_X = B_X - A_X, Delta_Z = B_Z - A_Z;

			float A_To_Partition = Partition - A_X;

			float Z = (A_To_Partition * Delta_Z / Delta_X) + A_Z;

			return Min < Z && Max > Z;
		}

		bool Colliding(float A_X, float A_Z, float B_X, float B_Z)
		{
			switch (Partition_Axis)
			{
			case Axis::X:

				return Colliding_X(A_X, A_Z, B_X, B_Z);

				//

			case Axis::Z:

				return Colliding_X(A_Z, -A_X, B_Z, -B_X); // This just rotates the coords, the logic is still the same

			default:
				Throw_Error(" >> Light occluder not set properly!\n");

				return NULL;
			}
		}
	};

	std::vector<Light_Occluder> Light_Occluders;

	void Clip_Node_With_Light_Occluders(glm::vec2& Middle, glm::vec2& Max, glm::vec2& Min)
	{
		int W = Light_Occluders.size();
		while (W--)
		{
			if (Light_Occluders[W].Handle_Node(Middle, Max, Min))
				W = 0;
		}
	}

	bool Is_Occluded(glm::vec2 A, float B_X, float B_Z)
	{
		for (size_t W = 0; W < Light_Occluders.size(); W++)
			if (Light_Occluders[W].Colliding(A.x, A.y, B_X, B_Z))
				return true;

		return false;
	}

	void Load_Light_Occluders(const char* Directory)
	{
		if (Directory == nullptr)
		{
			Light_Occluders.clear();

			return;
		}

		std::vector<Hitbox*> Occluder_Boxes = Generate_AABB_Hitboxes(Directory);

		Light_Occluders.resize(Occluder_Boxes.size());

		for (size_t W = 0; W < Occluder_Boxes.size(); W++)
		{
			AABB_Hitbox* Box = reinterpret_cast<AABB_Hitbox*>(Occluder_Boxes[W]);
			
			float Delta_X = Box->B.x - Box->A.x, Delta_Z = Box->B.z - Box->A.z;

			if (Delta_X > Delta_Z) // If the min/max is on the x axis
			{
				Light_Occluders[W].Min = Box->A.x;
				Light_Occluders[W].Max = Box->B.x;

				Light_Occluders[W].Partition = Box->A.z + Delta_Z * 0.5f;

				Light_Occluders[W].Partition_Axis = Light_Occluder::Axis::Z;
			}
			else
			{
				Light_Occluders[W].Min = Box->A.z;
				Light_Occluders[W].Max = Box->B.z;

				Light_Occluders[W].Partition = Box->A.x + Delta_X * 0.5f;

				Light_Occluders[W].Partition_Axis = Light_Occluder::Axis::X;
			}

			// This sets the values appropriately!
		}
	}

	//

	void Update_Leaf_Node_Data();

	void Parse_Partition_Nodes_To_Shader(Shader& Shader)
	{
		glUniform2fv(glGetUniformLocation(Shader.Program_ID, "Partition_Nodes"), Number_Of_Partition_Nodes, (const GLfloat*)Partition_Nodes);

		glUniform1uiv(glGetUniformLocation(Shader.Program_ID, "Leaf_Node_Indices"), Number_Of_Leaf_Nodes * 2, (const GLuint*)Leaf_Nodes);
		
		// We specifically use a count of (Number_Of_Partition_Nodes / 4) because we pack the integers in such a way that each byte is stored in 1/4th of a full 32-bit word
	}

	void Generate_Light_BVH_Tree();

	void Update_Leaf_Node_Data_Threaded(void* Parameters)
	{
		size_t* Offset = (size_t*)Parameters;


		for (size_t W = *Offset; W < Number_Of_Leaf_Nodes; W += NUMBER_OF_WORKERS)
		{
			struct Lightsource_Index_Data
			{
				unsigned char Index;
				float Distance;

				bool operator<(Lightsource_Index_Data& Other)
				{
					return Distance < Other.Distance;
				}
			};

			std::vector<Lightsource_Index_Data> Index_Data(Scene_Lights.size());

			for (size_t V = 0; V < Scene_Lights.size(); V++)
			{
				Index_Data[V].Index = V;
				Index_Data[V].Distance = (!Scene_Lights[V]->Flags[LF_PRIORITY]) * 
					(squaref(Scene_Lights[V]->Position.x - Leaf_Nodes_Info[W].Position.x) + 
						squaref(Scene_Lights[V]->Position.z - Leaf_Nodes_Info[W].Position.y));

				Index_Data[V].Distance += 999999.0f * Is_Occluded(Leaf_Nodes_Info[W].Position, Scene_Lights[V]->Position.x, Scene_Lights[V]->Position.z);
				
				// If the light is occluded, it can be treated as though it is much lower priority than it is
			}

			std::sort(Index_Data.begin(), Index_Data.end());

			for (size_t Indices = 0; Indices < 8; Indices++)
				Leaf_Nodes[W].Light_Indices[Indices] = Index_Data[Indices].Index;
		}

		delete Offset;
	}

	void Update_Leaf_Node_Data()
	{
		for (size_t W = 0; W < NUMBER_OF_WORKERS - 1u; W++)
			Update_Leaf_Node_Data_Threaded(new size_t(W));
			//Job_System::Submit_Job(Job_System::Job(Update_Leaf_Node_Data_Threaded, new size_t(W)));

		Update_Leaf_Node_Data_Threaded(new size_t(NUMBER_OF_WORKERS - 1u));
	}

	bool Determine_Side_Of_Node(float X, float Y, Node_Partition Node)
	{
		//return (Node.X < BVH_Float_To_Byte(X)) || 
		//	(Node.Y < BVH_Float_To_Byte(Y));

		return (Node.X < X || Node.Y < Y);

		// If the x coordinate is further right than the boundary OR the y coordinate is further up than the boundary, it's the right-hand child node in the binary tree

		// For this reason, default (or unimportant) values in the BVH tree will be set to 127 (the highest positive signed char value)
	}

	bool Validate_Position_Upwards_Traverse(float X, float Y, size_t Index)
	{
		if (Index)
		{
			size_t Parent_Index = (Index - 1) >> 1;

			bool Correct_Side = ((Index + 1) & 1u) == Determine_Side_Of_Node(X, Y, Partition_Nodes[Parent_Index]);

			if (Correct_Side)
				return Validate_Position_Upwards_Traverse(X, Y, Parent_Index);
			else
				return false;
		}
		else
			return true;
	}

	std::vector<Lightsource*> Get_All_Lightsources_In_Node(Boundary Volume)
	{
		std::vector<Lightsource*> List_Of_Lights;

		for (size_t W = 0; W < Scene_Lights.size(); W++)
			if (
				Scene_Lights[W]->Position.x > Volume.Min_X &&
				Scene_Lights[W]->Position.x < Volume.Max_X &&
				Scene_Lights[W]->Position.z > Volume.Min_Y &&
				Scene_Lights[W]->Position.z < Volume.Max_Y)
				//&& glm::length(Scene_Lights[W]->Position - Player_Camera.Position) < 20.0f)
				List_Of_Lights.push_back(Scene_Lights[W]);

		return List_Of_Lights;
	}

	void Generate_Light_BVH_Tree() // The light BVH tree will probably need to be updated on a semi-regular basis, every half second at least
	{
		Boundary Boundaries[Number_Of_Partition_Nodes + Number_Of_Leaf_Nodes];

		for (size_t W = 0; W < Number_Of_Partition_Nodes; W++)
		{
			// This will decide, for each node, where the partition should go based on all of the found lights within this node

			std::vector<Lightsource*> Lightsources = Get_All_Lightsources_In_Node(Boundaries[W]);

			size_t Child_Node = (W << 1) + 1;

			glm::vec2 Min = glm::vec2(Boundaries[W].Min_X, Boundaries[W].Min_Y),
				Max = glm::vec2(Boundaries[W].Max_X, Boundaries[W].Max_Y),
				Middle;

			if (Lightsources.size() > 1)
			{
				Min = glm::vec2(Lightsources[0]->Position.x, Lightsources[0]->Position.z);

				Max = Min;

				for (size_t V = 1; V < Lightsources.size(); V++)
				{
					Min.x = std::fminf(Min.x, Lightsources[V]->Position.x);
					Min.y = std::fminf(Min.y, Lightsources[V]->Position.z);

					Max.x = std::fmaxf(Max.x, Lightsources[V]->Position.x);
					Max.y = std::fmaxf(Max.y, Lightsources[V]->Position.z);
				}
			}

			//

			Middle = Min + Max;
			Middle *= 0.5f;

			Clip_Node_With_Light_Occluders(Middle, Max, Min);

			if ((Max.x - Min.x) >= (Max.y - Min.y))
			{
				Partition_Nodes[W].X = Middle.x;// BVH_Float_To_Byte(Middle.x);
				Partition_Nodes[W].Y = Boundary_Max_Value; // 127;

				Boundaries[Child_Node] = Boundaries[W];

				Boundaries[Child_Node].Max_X = Partition_Nodes[W].X;

				//

				Boundaries[Child_Node + 1] = Boundaries[W];

				Boundaries[Child_Node + 1].Min_X = Partition_Nodes[W].X;
			}
			else
			{
				Partition_Nodes[W].X = Boundary_Max_Value;
				Partition_Nodes[W].Y = Middle.y;// BVH_Float_To_Byte(Middle.y);

				Boundaries[Child_Node] = Boundaries[W];
				Boundaries[Child_Node].Max_Y = Partition_Nodes[W].Y;

				Boundaries[Child_Node + 1] = Boundaries[W];
				Boundaries[Child_Node + 1].Min_Y = Partition_Nodes[W].Y;
			}
		}

		for (size_t W = Number_Of_Partition_Nodes; W < Number_Of_Partition_Nodes + Number_Of_Leaf_Nodes; W++)
		{
			if constexpr (false)
			{
				const float Conversion = 1.0f / 30.0f;

				UI_Elements.push_back(new UI_Element(Conversion * Boundaries[W].Min_X, Conversion * Boundaries[W].Min_Y, Conversion * Boundaries[W].Max_X, Conversion * Boundaries[W].Max_Y, Pull_Texture("Assets/Textures/Pillow.png").Texture));
				UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;
				UI_Elements.back()->UI_Border_Size = 0.0f;
			}

			//

			Leaf_Nodes_Info[W - Number_Of_Partition_Nodes].Position.x = (Boundaries[W].Max_X + Boundaries[W].Min_X) * 0.5f;
			Leaf_Nodes_Info[W - Number_Of_Partition_Nodes].Position.y = (Boundaries[W].Max_Y + Boundaries[W].Min_Y) * 0.5f;
		}

		// From there, we'll generate all the leaf node's average positions
	}

	size_t Get_BVH_Node(float X, float Z)
	{
		size_t Index = 0u;
		for (size_t W = 0u; W < 6u; W++)
		{
			Node_Partition Node = Partition_Nodes[Index];

			size_t Side = (X > Node.X) || (Z > Node.Y);

			Index = Index + Index;
			Index = Index + 1u + Side;
		}

		return Index;
	}

	void Load_BVH_Tree_Corrections(const char* Directory) // These are manual tweaks to the BVH tree which ensure the lighting looks natural
	{
		std::string Buffer = Pull_Text(Directory).Text; // We need to keep a std::string alive so that the memory isn't freed until the end of the function
		const char* File_Contents = Buffer.c_str(); // There isn't really a need to cache this, it's usually under 16 bytes

		size_t Index = 0;

		while (*File_Contents)
		{
			float X, Z;
			X = Encoder::Characters_To_Float(File_Contents);
			File_Contents += 6u; // This advances to next floating point val
			Z = Encoder::Characters_To_Float(File_Contents);
			File_Contents += 7u; // This advances to next line

			size_t Node = Get_BVH_Node(X, Z) - Number_Of_Partition_Nodes;
			Leaf_Nodes_Info[Node].Position.x = X;
			Leaf_Nodes_Info[Node].Position.y = Z;
		}
	}
}

#endif
#ifndef RADIOPHOBIA_LEVEL_INIT
#define RADIOPHOBIA_LEVEL_INIT

#include "../Engine_Loop_Includes.h"
#include "../Engine_Loop.h"
#include "../UI_Textbox_Definitions.h"

#include "Menus.h"

#include "Enemy_Declarations.h"

#include "../Light_BVH_Tree_Handler.h"

#include "Interactable_Declarations.h"

#include "../Hitdetection_Blockmap.h"


void Load_Test_Scene_Assets()
{
	Context_Interface::Loading_Progress_Total = 36;

	//Push_Merged_Material<THREADED>("Assets/Textures/Brick_Specular.png", "Assets/Textures/Brick_Reflectivity.png", "Assets/Textures/Brick_Normal_Test.png", "Brick");
	Push_Merged_Material<THREADED>("Assets/Textures/Brick.png", nullptr, "Assets/Textures/Brick_Normal.png", "Brick");

	// Push_Merged_Material<THREADED>("Assets/Textures/Brick_Reflectivity.png", "Assets/Textures/Brick_Reflectivity.png", "Assets/Textures/Test_Normal.png", "Stone");

	Push_Merged_Material<THREADED>("Assets/Textures/Floor_Tile_Spec.png", "Assets/Textures/Brick_Reflectivity.png", "Assets/Textures/Floor_Tiles_Normal.png", "Floor");

	Push_Merged_Material<THREADED>("Assets/Textures/Floor_2_Tile_Specular.png", "Assets/Textures/Brick_Reflectivity.png", "Assets/Textures/Floor_2_Tile_Normal.png", "NPP_Wall");
	Push_Merged_Material<THREADED>("Assets/Textures/Reddened_Wall_Spec.png", nullptr, "Assets/Textures/Reddened_Wall_Normal_2.png", "NPP_Wall_2");

	// Push_Merged_Material<THREADED>("Assets/Textures/Brick_Specular.png", "Assets/Textures/Flat_Reflectivity.png", "Assets/Textures/Brick_Normal_Test.png", "Floor_Reflect");

	Push_Merged_Material<THREADED>("Assets/Textures/Rust_Texture.png", "Assets/Textures/Rust_Reflectivity.png", "Assets/Textures/Rust_Normal.png", "Rust");

	Push_Merged_Material<THREADED>("Assets/Textures/Rubbish_Bag.jpg", "Assets/Textures/Brick_Reflectivity.png", "Assets/Textures/Rubbish_Normal.png", "Rubbish");

	Push_Merged_Material<THREADED>("Assets/Textures/Brick_Reflectivity.png", "Assets/Textures/Brick_Reflectivity.png", "Assets/Textures/Rubble_Normal.png", "Rubble");

	Push_Merged_Material<THREADED>("Assets/Textures/Door_Texture.png", nullptr, "Assets/Textures/Door_Normal.png", "Door");

	Push_Merged_Material<THREADED>("Assets/Textures/Ceiling_Specular.png", nullptr, "Assets/Textures/Ceiling_Normal.png", "Ceiling");

	Push_Merged_Material<THREADED>("Assets/Textures/Wall_Specular.png", nullptr, "Assets/Textures/Wall_Normal.png", "Wall");

	Push_Merged_Material<THREADED>("Assets/Textures/Metal_Specular.png", nullptr, "Assets/Textures/Metal_Normal_Texture.png", "Metal");

	Push_Merged_Material<THREADED>("Assets/Textures/Vent_Duct.png", nullptr, "Assets/Textures/Vent_Normal.png", "Vent");

	Pull_Animation<THREADED>("Assets/Animations/Intro_Level/Door_0_Open.anim");
	Pull_Animation<THREADED>("Assets/Animations/Intro_Level/Door_1_Open.anim");
	Pull_Animation<THREADED>("Assets/Animations/Intro_Level/Door_2_Open.anim");
	Pull_Animation<THREADED>("Assets/Animations/Intro_Level/Door_3_Open.anim");
	Pull_Animation<THREADED>("Assets/Animations/Intro_Level/Door_5_Open.anim");
	Pull_Animation<THREADED>("Assets/Animations/Intro_Level/Vent_Damage.anim");
	Pull_Animation<THREADED>("Assets/Animations/Intro_Level/Vent_Remove.anim");

	Pull_Animation<THREADED>("Assets/Animations/Murderer_Idle.anim");

	Pull_Texture<THREADED>("Assets/Textures/Shelf_things.png");
	Pull_Texture<THREADED>("Assets/Textures/Stromkasten.png");
	Pull_Texture<THREADED>("Assets/Textures/Toilet_Texture.png");

	//

	Pull_Audio("Assets/Audio/Phone/Endcall_Beep.wav");
	Pull_Audio("Assets/Audio/Phone/I_CAN_SEE_YOU2.wav");
	Pull_Audio("Assets/Audio/Phone/Static.wav");
	Pull_Audio("Assets/Audio/Phone/Pickup_Phone.wav");
	Pull_Audio("Assets/Audio/Phone/Putdown_Phone.wav");
	Pull_Audio("Assets/Audio/Phone/Telephone_Ring.wav");
	Pull_Audio("Assets/Audio/Music/Ambience_Track.wav");

	//

	Initialise_Pistol();

	Pull_Mesh<THREADED>("Assets/Models/Intro_Level/Toilets.obj", LOAD_MESH_OBJ_BIT);
	Pull_Mesh<THREADED>("Assets/Models/Intro_Level/Tables.obj", LOAD_MESH_OBJ_BIT);
	Pull_Mesh<THREADED>("Assets/Models/Intro_Level/Heaters.obj", LOAD_MESH_OBJ_BIT);

	Pull_Mesh<THREADED>("Assets/Models/Intro_Level/Test_Intro_Level.obj", LOAD_MESH_OBJ_BIT);

	Pull_Mesh<THREADED>("Assets/Models/Intro_Tunnel/Tunnel.obj", LOAD_MESH_OBJ_BIT);
	Pull_Mesh<THREADED>("Assets/Models/Intro_Tunnel/Frame.obj", LOAD_MESH_OBJ_BIT);
	// Pull_Mesh<THREADED>("Assets/Hitboxes/Level_2_Map.obj", LOAD_MESH_OBJ_BIT);
	// Pull_Mesh<THREADED>("Assets/Models/Test_Person.obj");
}

void Create_Ambient_Light_Sources()
{
	size_t Size = Scene_Lights.size();

	for (size_t W = 1; W < Size; W+=1)
	{
		Scene_Lights.push_back(new Lightsource(*Scene_Lights[W]));

		Scene_Lights.back()->FOV = 180;
		// Scene_Lights.back()->Colour *= glm::vec3(0.5f);
		Scene_Lights.back()->Attenuation = 1.0f;
	}
}

//

//

//

void Setup_Intro_Tunnel()
{

#define LIGHT_Y -1.35f
#define LIGHT_FOV 60.0f
#define LIGHT_COLOUR glm::vec3(0.12f, 0.1f, 0.11f)

	// Lighting_BVH::Boundary_Max_Value = 10.0f;

	Scene_Lights.push_back(new Lightsource(
		glm::vec3(0.0f, LIGHT_Y, -1.21f),
		LIGHT_COLOUR,
		glm::vec3(0.0f, 1.0f, 0.0f),
		LIGHT_FOV, 1.0f, 0.6f, LG::Group_One));

	/*Scene_Lights.push_back(new Lightsource(
		glm::vec3(0.0f, LIGHT_Y, -3.037f),
		LIGHT_COLOUR,
		glm::vec3(0.0f, 1.0f, 0.0f),
		LIGHT_FOV, 1.0f, 0.6f, LG::Group_One));*/

	Scene_Lights.push_back(new Lightsource(
		glm::vec3(0.0f, LIGHT_Y, -4.967f),
		LIGHT_COLOUR,
		glm::vec3(0.0f, 1.0f, 0.0f),
		LIGHT_FOV, 1.0f, 0.6f, LG::Group_One));

	Scene_Lights.push_back(new Lightsource(
		glm::vec3(0.0f, -1.84f, -7.749f),
		LIGHT_COLOUR,
		glm::vec3(0.0f, 1.0f, 0.0f),
		LIGHT_FOV, 1.0f, 0.6f));

	Scene_Lights.back()->Flags[LF_CAST_SHADOWS] = true;

	Scene_Lights.push_back(new Lightsource(
		glm::vec3(-2.704f, LIGHT_Y, -7.749f),
		LIGHT_COLOUR,
		glm::vec3(0.0f, 1.0f, 0.0f),
		LIGHT_FOV, 1.0f, 0.6f, LG::Group_Two));

	Scene_Lights.push_back(new Lightsource(
		glm::vec3(-4.704f, LIGHT_Y, -7.749f),
		LIGHT_COLOUR,
		glm::vec3(0.0f, 1.0f, 0.0f),
		LIGHT_FOV, 1.0f, 0.6f, LG::Group_Two));

	Scene_Lights.push_back(new Lightsource(
		glm::vec3(-6.404f, LIGHT_Y, -7.749f),
		LIGHT_COLOUR,
		glm::vec3(0.0f, 1.0f, 0.0f),
		LIGHT_FOV, 1.0f, 0.6f, LG::Group_Two));

#undef LIGHT_Y
#undef LIGHT_FOV
#undef LIGHT_COLOUR

	Create_Ambient_Light_Sources();

	Scene_Models.push_back(new Model({ MF_CAST_SHADOWS, MF_ACTIVE }, Object_Material::Stone));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Tunnel/Frame.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Toilet_Texture.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), 
		new Flicker_Light_Controller(Splice_Vector(Scene_Lights, 1u, Scene_Lights.size())),
		std::vector<Hitbox*>(0));

	// We ignore index 0 because we don't want to include the player's flashlight in the flicker effect

	Scene_Models.push_back(new Model({ MF_SOLID, MF_CAST_SHADOWS, MF_USE_DECALS }, Object_Material::Concrete));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Tunnel/Tunnel.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Brick.png").Texture, Pull_Texture("Brick").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Models/Intro_Tunnel/Tunnel.obj", LOAD_MESH_OBJ_BIT).Mesh));

	Scene_Models.push_back(new Model({ MF_SOLID, MF_USE_DECALS }, Object_Material::Stone));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Tunnel/Tunnel_Floor.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Concrete.jpg").Texture, Pull_Texture("NPP_Wall_2").Texture, Scene_Models.back(), new Controller(), std::vector<Hitbox*>{ Generate_AABB_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Tunnel/Floor.obj", LOAD_MESH_OBJ_BIT).Mesh) });

	Scene_Models.push_back(new Model({ MF_SOLID, MF_CAST_SHADOWS, MF_USE_DECALS }, Object_Material::Concrete));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Tunnel/Tunnel_Cubicle.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall_2").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Hitboxes/Intro_Tunnel/Cubicle.obj", LOAD_MESH_OBJ_BIT).Mesh));

	Blockmap::Initialise_Blockmap();

	//

	Scene_Models.push_back(new Model({ MF_ACTIVE }, Object_Material::Electronics));
	Scene_Models.back()->Position = glm::vec3(0.0f, -1.45f, -3.037f);
	//Scene_Lights[2]->Position; 
	//glm::vec3(-0.44f, -0.74f, -3.0f);
	Create_Model(Pull_Mesh("Assets/Models/Alarm_Light.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Rust_Texture.png").Texture, Pull_Texture("Rust").Texture, Scene_Models.back(), new Alarm_Light_Controller(), std::vector<Hitbox*>(0));

	Player_Physics_Object.Object->Position = glm::vec3(0.0f, -1.0f, 0.0f);
	
	Player_Camera.Orientation.x = 0.0f;

	Lighting_BVH::Load_Light_Occluders("Assets/Light_Occluders/Tunnel.obj");

	Lighting_BVH::Generate_Light_BVH_Tree();
	Lighting_BVH::Update_Leaf_Node_Data();

	Lighting_BVH::Load_BVH_Tree_Corrections("Assets/Light_Occluders/Tunnel_Corrections.txt");

	Audio::Set_Music_Tracks_For_Deletion(); // This stops any music currently playing
	Audio::Create_Music_Source(Pull_Audio("Assets/Audio/Music/Falling_Shepard_Tone.wav").Source); // This plays the loaded ambience track
}

void Setup_Intro_Level()
{
	Lighting_BVH::Boundary_Max_Value = 22.0f;

	Scene_Models.push_back(new Model({ MF_SOLID, MF_CAST_SHADOWS, MF_USE_DECALS }, Object_Material::Concrete));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	//Create_Model(Pull_Mesh("Assets/Models/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Hitboxes/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Mesh));
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Test_Intro_Level.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Models/Intro_Level/Test_Intro_Level.obj", LOAD_MESH_OBJ_BIT).Mesh));

	Scene_Models.push_back(new Model({ MF_SOLID, MF_CAST_SHADOWS, MF_USE_DECALS }, Object_Material::Concrete));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	//Create_Model(Pull_Mesh("Assets/Models/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Hitboxes/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Mesh));
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Bathroom_Walls.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Toilet_Texture.png").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Models/Intro_Level/Bathroom_Walls.obj", LOAD_MESH_OBJ_BIT).Mesh));

	Scene_Models.push_back(new Model({ MF_SOLID, MF_CAST_SHADOWS, MF_USE_DECALS }, Object_Material::Wall));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	//Create_Model(Pull_Mesh("Assets/Models/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Hitboxes/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Mesh));
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Walls.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Walls.png").Texture, Pull_Texture("Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Models/Intro_Level/Walls.obj", LOAD_MESH_OBJ_BIT).Mesh));

	Scene_Models.push_back(new Model({ MF_SOLID, MF_CAST_SHADOWS, MF_USE_DECALS }, Object_Material::Wood));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	//Create_Model(Pull_Mesh("Assets/Models/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Hitboxes/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Mesh));
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Ceiling.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Ceiling_Texture.png").Texture, Pull_Texture("Ceiling").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Models/Intro_Level/Ceiling.obj", LOAD_MESH_OBJ_BIT).Mesh));

	Scene_Models.push_back(new Model({ }, Object_Material::Wood));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Ceiling_Tiles.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Ceiling_Texture.png").Texture, Pull_Texture("Ceiling").Texture, Scene_Models.back(), new Controller(), std::vector<Hitbox*>{});

	Scene_Models.push_back(new Model({  }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Fire_Extinguishers.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Fire_Extinguisher.png").Texture, Pull_Texture("Metal").Texture, Scene_Models.back(), new Controller(), std::vector<Hitbox*>{});

	Scene_Models.push_back(new Model({  }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Heaters.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Lamp.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Controller(), std::vector<Hitbox*>{});

	Scene_Models.push_back(new Model({ MF_SOLID, MF_CAST_SHADOWS, MF_USE_DECALS }, Object_Material::Wood));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Tables.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Table1.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Controller(), Generate_AABB_Hitboxes("Assets/Hitboxes/Intro_Level/Tables.obj") + std::vector<Hitbox*>{ (Hitbox*)Generate_Mesh_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Table_Hitbox.obj").Mesh) });

	Scene_Models.push_back(new Model({ MF_SOLID, MF_CAST_SHADOWS, MF_USE_DECALS }, Object_Material::Wood));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Shelves.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Table1.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Controller(),
		std::vector<Hitbox*>{
		Generate_Mesh_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Shelf_Hitbox.obj").Mesh),
			Generate_AABB_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Shelf_AABB.obj").Mesh),
			Generate_AABB_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Shelf_AABB_1.obj").Mesh)
	});

	Scene_Models.push_back(new Model({ MF_CAST_SHADOWS, MF_SOLID, MF_USE_DECALS }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Toilets.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Toilet_Texture.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Controller(), Generate_AABB_Hitboxes("Assets/Hitboxes/Intro_Level/Bathroom_Hitboxes.obj"));

	Scene_Models.push_back(new Model({ MF_CAST_SHADOWS, MF_SOLID, MF_USE_DECALS }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Electrical_Boxes.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Stromkasten.png").Texture, Pull_Texture("Metal").Texture, Scene_Models.back(), new Controller(), Generate_AABB_Hitboxes("Assets/Hitboxes/Intro_Level/Electrical_Boxes.obj"));

	//

	Scene_Models.push_back(new Model({ MF_ACTIVE, MF_SOLID, MF_USE_DECALS }, Object_Material::Electronics));
	Scene_Models.back()->Position = glm::vec3(5.22f, -0.95f, -0.722f);
	Scene_Models.back()->Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Phone_0.obj", LOAD_MESH_ANIM_BIT | LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Telefon.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Phone_Controller(), std::vector<Hitbox*>{ });

	//

	Scene_Models.push_back(new Model({  }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Fire_Extinguisher_Signs.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Fire_Extinguisher_Sign.jpg").Texture, Pull_Texture("Metal").Texture, Scene_Models.back(), new Controller(), std::vector<Hitbox*>{});

	Scene_Models.push_back(new Model({  }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Signs.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Signs.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Controller(), std::vector<Hitbox*>{});

	//

	Scene_Models.push_back(new Model({ MF_SOLID, MF_CAST_SHADOWS, MF_USE_DECALS }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Lockers.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Metal_Specular.png").Texture, Pull_Texture("Metal").Texture, Scene_Models.back(), new Controller(),
		std::vector<Hitbox*>{
		Generate_AABB_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Locker_AABB_0.obj").Mesh),
			Generate_AABB_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Locker_AABB_1.obj").Mesh),
			Generate_AABB_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Locker_AABB_2.obj").Mesh),
			Generate_Mesh_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Locker_0.obj").Mesh),
			Generate_Mesh_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Locker_1.obj").Mesh),
			Generate_Mesh_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Locker_2.obj").Mesh),
	}
	);

	Scene_Models.push_back(new Model({ MF_CAST_SHADOWS, MF_USE_DECALS }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Shelf_Objects.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Shelf_things.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Controller(), std::vector<Hitbox*>{});

	//

	Scene_Models.push_back(new Model({ MF_ACTIVE, MF_CAST_SHADOWS, MF_USE_DECALS }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Vents.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Metal_Specular.png").Texture, Pull_Texture("Metal").Texture, Scene_Models.back(),
		new Vent_Smoke_Controller(std::vector<Vent_Smoke_Controller::Vent_Info>
	{
		{ glm::vec3(3.1434f, -1.0362, 7.3434f), Vent_Smoke_Controller::Directions::West },
		{ glm::vec3(3.958f, -0.38284, 1.8101f), Vent_Smoke_Controller::Directions::South },
		{ glm::vec3(1.7492f, -0.38284, 1.8101f), Vent_Smoke_Controller::Directions::South },
		{ glm::vec3(0.7f, -1.1942, 3.5388f), Vent_Smoke_Controller::Directions::North },

		{ glm::vec3(-2.356f, -1.1545, 8.6548f), Vent_Smoke_Controller::Directions::North },
		{ glm::vec3(-4.1754f, -1.1403, 7.6125f), Vent_Smoke_Controller::Directions::East },

		{ glm::vec3(8.7325f, -1.1335, 1.8703f), Vent_Smoke_Controller::Directions::South },
	}
	), std::vector<Hitbox*>{});

	Scene_Models.push_back(new Model({ MF_SOLID, MF_USE_DECALS, MF_CAST_SHADOWS }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Air_Duct.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Vent_Duct.png").Texture, Pull_Texture("Vent").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Air_Duct.obj", LOAD_MESH_OBJ_BIT).Mesh));

	Scene_Models.push_back(new Model({ MF_ACTIVE, MF_SOLID, MF_CAST_SHADOWS }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Vent_Grate.obj", LOAD_MESH_OBJ_BIT | LOAD_MESH_ANIM_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Vent_Duct.png").Texture, Pull_Texture("Vent").Texture, Scene_Models.back(), new Damageable_Vent_Controller("Assets/Animations/Intro_Level/Vent"), std::vector<Hitbox*>{ Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Intro_Level/Vent_Grate.obj", LOAD_MESH_OBJ_BIT).Mesh) });

	Scene_Models.push_back(new Model({ MF_SOLID, MF_CAST_SHADOWS, MF_USE_DECALS }, Object_Material::Floor_Tiles));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	//Create_Model(Pull_Mesh("Assets/Models/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Hitboxes/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Mesh));
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Test_Intro_Level_Floor.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Floor_Tiles.png").Texture, Pull_Texture("Floor").Texture, Scene_Models.back(), new Controller(), Generate_AABB_Hitboxes("Assets/Hitboxes/Intro_Level/Intro_Level_Floor_Hitbox.obj"));

	Scene_Models.push_back(new Model({ MF_SOLID, MF_CAST_SHADOWS, MF_USE_DECALS, MF_ACTIVE }, Object_Material::Floor_Tiles));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	//Create_Model(Pull_Mesh("Assets/Models/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Hitboxes/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Mesh));
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Intro_Level_Stairs.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Floor_Tiles.png").Texture, Pull_Texture("Floor").Texture, Scene_Models.back(),
		new Thought_Popups_Controller(std::vector<Thought_Popups_Controller::Thought_Trigger>{
			{ "Intro_Level/Rubble_Stairs.txt", glm::vec3(-3.422f, -1.2f, 7.7f), 1.5f, 0 }, // First stairs and rubble
			{ "Intro_Level/Rubble_Hallway.txt", glm::vec3(6.4f, -0.11f, -3.27f), 1.65f, 0 }
	}),
		std::vector<Hitbox*>{
		Generate_Mesh_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Stairs_0_Hitbox.obj", LOAD_MESH_OBJ_BIT).Mesh),
			Generate_Mesh_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Stairs_1_Hitbox.obj", LOAD_MESH_OBJ_BIT).Mesh),
			Generate_Mesh_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Door_5_Hitbox.obj", LOAD_MESH_OBJ_BIT).Mesh),
			Generate_Mesh_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Door_0.obj", LOAD_MESH_OBJ_BIT).Mesh),
			Generate_Mesh_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Door_1.obj", LOAD_MESH_OBJ_BIT).Mesh),
			Generate_Mesh_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Door_2.obj", LOAD_MESH_OBJ_BIT).Mesh),
			Generate_Mesh_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Door_6.obj", LOAD_MESH_OBJ_BIT).Mesh),
			Generate_Mesh_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Door_7.obj", LOAD_MESH_OBJ_BIT).Mesh),
			Generate_Mesh_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Door_3.obj", LOAD_MESH_OBJ_BIT).Mesh)
	});

	Scene_Models.push_back(new Model({ MF_SOLID, MF_CAST_SHADOWS, MF_USE_DECALS }, Object_Material::Stone));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	//Create_Model(Pull_Mesh("Assets/Models/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Hitboxes/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Mesh));
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Intro_Level_Rubble.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Rubble.jpg").Texture, Pull_Texture("Rubble").Texture, Scene_Models.back(), new Controller(),
		std::vector<Hitbox*>{
		Generate_Mesh_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Rubble_0_Hitbox.obj", LOAD_MESH_OBJ_BIT).Mesh),
			Generate_Mesh_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Rubble_1_Hitbox.obj", LOAD_MESH_OBJ_BIT).Mesh),
			Generate_AABB_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Rubble_0_AABB_Hitbox.obj", LOAD_MESH_OBJ_BIT).Mesh)
	});

	Scene_Models.push_back(new Model({ MF_CAST_SHADOWS }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	//Create_Model(Pull_Mesh("Assets/Models/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Hitboxes/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Mesh));
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Intro_Level_Pipes.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Rust_Texture.png").Texture, Pull_Texture("Rust").Texture, Scene_Models.back(), new Controller(), std::vector<Hitbox*>{ Generate_AABB_Hitbox(*Pull_Mesh("Assets/Hitboxes/Cube.obj", LOAD_MESH_OBJ_BIT).Mesh) });

	Scene_Models.push_back(new Model({ MF_SOLID, MF_CAST_SHADOWS, MF_USE_DECALS }, Object_Material::Concrete));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	//Create_Model(Pull_Mesh("Assets/Models/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Hitboxes/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Mesh));
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Intro_Level_Rubbish.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Rubbish_Bag.jpg").Texture, Pull_Texture("Rubbish").Texture, Scene_Models.back(), new Controller(), Generate_AABB_Hitboxes("Assets/Hitboxes/Intro_Level/Rubbish_Hitboxes.obj"));

	Scene_Models.push_back(new Model({ MF_ACTIVE, MF_SOLID, MF_CAST_SHADOWS }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	//Create_Model(Pull_Mesh("Assets/Models/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Hitboxes/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Mesh));
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Door_0.obj", LOAD_MESH_OBJ_BIT | LOAD_MESH_ANIM_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Door_Texture.png").Texture, Pull_Texture("Door").Texture, Scene_Models.back(), new Door_Controller("Assets/Animations/Intro_Level/Door_0_Open.anim"), std::vector<Hitbox*>{ /*Generate_AABB_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Door_0.obj", LOAD_MESH_OBJ_BIT).Mesh)*/ });

	Scene_Models.push_back(new Model({ MF_ACTIVE, MF_SOLID, MF_CAST_SHADOWS }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	//Create_Model(Pull_Mesh("Assets/Models/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Hitboxes/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Mesh));
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Door_1.obj", LOAD_MESH_OBJ_BIT | LOAD_MESH_ANIM_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Door_Texture.png").Texture, Pull_Texture("Door").Texture, Scene_Models.back(), new Door_Controller("Assets/Animations/Intro_Level/Door_1_Open.anim"), std::vector<Hitbox*>{ /*Generate_AABB_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Door_1.obj", LOAD_MESH_OBJ_BIT).Mesh)*/ });

	Scene_Models.push_back(new Model({ MF_ACTIVE, MF_SOLID, MF_CAST_SHADOWS }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Door_2.obj", LOAD_MESH_OBJ_BIT | LOAD_MESH_ANIM_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Door_Texture.png").Texture, Pull_Texture("Door").Texture, Scene_Models.back(), new Door_Controller("Assets/Animations/Intro_Level/Door_2_Open.anim"), std::vector<Hitbox*>{ /*Generate_AABB_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Door_2.obj", LOAD_MESH_OBJ_BIT).Mesh)*/ });

	Scene_Models.push_back(new Model({ MF_ACTIVE, MF_SOLID, MF_CAST_SHADOWS }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	//Create_Model(Pull_Mesh("Assets/Models/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Hitboxes/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Mesh));
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Door_3.obj", LOAD_MESH_OBJ_BIT | LOAD_MESH_ANIM_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Door_Texture.png").Texture, Pull_Texture("Door").Texture, Scene_Models.back(), new Door_Controller("Assets/Animations/Intro_Level/Door_3_Open.anim"), std::vector<Hitbox*>{ /*Generate_AABB_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Door_2.obj", LOAD_MESH_OBJ_BIT).Mesh)*/ });

	Scene_Models.push_back(new Model({  }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Door_4.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Door_Texture.png").Texture, Pull_Texture("Door").Texture, Scene_Models.back(), new Controller(), std::vector<Hitbox*>{});

	Scene_Models.push_back(new Model({ MF_ACTIVE, MF_SOLID, MF_CAST_SHADOWS }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	//Create_Model(Pull_Mesh("Assets/Models/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Hitboxes/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Mesh));
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Door_5.obj", LOAD_MESH_OBJ_BIT | LOAD_MESH_ANIM_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Door_Texture.png").Texture, Pull_Texture("Door").Texture, Scene_Models.back(), new Door_Controller("Assets/Animations/Intro_Level/Door_5_Open.anim"), std::vector<Hitbox*>{ /*Generate_AABB_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Door_2.obj", LOAD_MESH_OBJ_BIT).Mesh)*/ });

	Scene_Models.push_back(new Model({ MF_ACTIVE, MF_SOLID, MF_CAST_SHADOWS }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	//Create_Model(Pull_Mesh("Assets/Models/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Hitboxes/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Mesh));
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Door_6.obj", LOAD_MESH_OBJ_BIT | LOAD_MESH_ANIM_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Door_Texture.png").Texture, Pull_Texture("Door").Texture, Scene_Models.back(), new Door_Controller("Assets/Animations/Intro_Level/Door_6_Open.anim"), std::vector<Hitbox*>{ /*Generate_AABB_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Door_2.obj", LOAD_MESH_OBJ_BIT).Mesh)*/ });

	Scene_Models.push_back(new Model({ MF_ACTIVE, MF_SOLID, MF_CAST_SHADOWS }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	//Create_Model(Pull_Mesh("Assets/Models/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Hitboxes/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Mesh));
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Door_7.obj", LOAD_MESH_OBJ_BIT | LOAD_MESH_ANIM_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Door_Texture.png").Texture, Pull_Texture("Door").Texture, Scene_Models.back(), new Door_Controller("Assets/Animations/Intro_Level/Door_7_Open.anim"), std::vector<Hitbox*>{ /*Generate_AABB_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Door_2.obj", LOAD_MESH_OBJ_BIT).Mesh)*/ });


	//

	Scene_Models.push_back(new Model({ MF_CAST_SHADOWS }, Object_Material::Wood));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Rutschgefahr.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Rutschgefahr.png").Texture, Pull_Texture("Floor").Texture, Scene_Models.back(), new Controller(), std::vector<Hitbox*>{});

	//

	/*Scene_Models.push_back(new Model({MF_SOLID, MF_CAST_SHADOWS, MF_USE_DECALS}, Object_Material::Enemy));
	Scene_Models.back()->Position = glm::vec3(1.394211, 0.0f, 0.438914);
	Create_Model(Pull_Mesh("Assets/Models/Test_Person.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Floor_Tiles.png").Texture, Pull_Texture("Floor").Texture, Scene_Models.back(), new Controller(), { Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Test_Person.obj").Mesh) });
	*/

	Volumetric_Cone_Particles.Particles.Spawn_Particle(glm::vec3(3.773928 - 1.0f, 0.0f - 1.0f, -8.415 - 1.0f), glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f)), glm::vec3(0.4, 0.6, 0.8), 0.6f, 30.0f);
	Scene_Lights.push_back(new Lightsource(glm::vec3(3.773928 - 1.0f, 0.0f - 1.0f, -8.415 - 1.0f), glm::vec3(1, 1.5, 2), glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f)), 30.0f, 1.0f, 0.6f));
	Scene_Models.push_back(new Model({ MF_CAST_SHADOWS, MF_SOLID, MF_USE_DECALS }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(3.773928 - 1.0f, 0.0f - 1.0f, -8.415 - 1.0f);
	Create_Model(Pull_Mesh("Assets/Models/Bubble.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Floor_Tiles.png").Texture, Pull_Texture("Floor").Texture, Scene_Models.back(), new Controller(), { Generate_Sphere_Hitbox(*Pull_Mesh("Assets/Models/Bubble.obj").Mesh) });

	//



	//

	Scene_Lights.push_back(new Lightsource(glm::vec3(2.2f, -1.23f, 8.27f), glm::vec3(0.8f, 0.65f, 0.65f), glm::vec3(0.0f, 1.0f, 0.0f), 360.0f, 10.0f, 0.6f, LG::Group_One));
	Scene_Lights.push_back(new Lightsource(glm::vec3(2.2f, -1.23f, 8.27f), glm::vec3(0.008f, 0.0065f, 0.0065f), glm::vec3(0.0f, 1.0f, 0.0f), 80.0f, 10.0f, 0.6f, LG::Group_One));

	Scene_Lights.push_back(new Lightsource(glm::vec3(2.2f, -1.23f, 4.413f), glm::vec3(0.8f, 0.65f, 0.65f), glm::vec3(0.0f, 1.0f, 0.0f), 80.0f, 10.0f, 0.6f, LG::Group_One));
	Scene_Lights.push_back(new Lightsource(glm::vec3(0.0793f, -1.23f, 7.73f), glm::vec3(0.8f, 0.65f, 0.65f), glm::vec3(0.0f, 1.0f, 0.0f), 80.0f, 10.0f, 0.6f, LG::Group_One));
	Scene_Lights.push_back(new Lightsource(glm::vec3(0.4622f, -1.23f, 2.66f), glm::vec3(0.8f, 0.65f, 0.65f), glm::vec3(0.0f, 1.0f, 0.0f), 80.0f, 10.0f, 0.6f, LG::Group_One));

	Scene_Lights.push_back(new Lightsource(glm::vec3(8.68f, -1.23f, 2.93f), glm::vec3(0.6f, 0.5f, 0.4f), glm::vec3(0.0f, 1.0f, 0.0f), 80.0f, 10.0f, 0.6f, LG::Group_One)); // Janitor's closet

	Scene_Lights.push_back(new Lightsource(glm::vec3(6.443f, -1.23f, -2.2f), glm::vec3(1.0f, 0.4f, 0.4f), glm::vec3(0.0f, 1.0f, 0.0f), 80.0f, 10.0f, 0.6f));

	Scene_Lights.push_back(new Lightsource(glm::vec3(10.08f, -1.43f, -0.749f), glm::vec3(0.55f, 0.6f, 0.7f), glm::vec3(0.0f, 1.0f, 0.0f), 80.0f, 10.0f, 0.6f)); // Locker room
	Scene_Lights.push_back(new Lightsource(glm::vec3(8.45f, -1.43f, -0.749f), glm::vec3(0.55f, 0.6f, 0.7f), glm::vec3(0.0f, 1.0f, 0.0f), 80.0f, 10.0f, 0.6f)); // Locker room

	//

	Scene_Lights.push_back(new Lightsource(glm::vec3(3.97f, -1.72f, -3.09f), glm::vec3(0.5f, 0.5f, 0.4f), glm::vec3(0.0f, 1.0f, 0.0f), 80.0f, 10.0f, 0.6f)); // Cafeteria
	Scene_Lights.push_back(new Lightsource(glm::vec3(3.97f, -1.72f, -1.26f), glm::vec3(0.5f, 0.5f, 0.4f), glm::vec3(0.0f, 1.0f, 0.0f), 80.0f, 10.0f, 0.6f));

	Scene_Lights.push_back(new Lightsource(glm::vec3(2.34f, -1.72f, -1.26f), glm::vec3(0.5f, 0.5f, 0.4f), glm::vec3(0.0f, 1.0f, 0.0f), 80.0f, 10.0f, 0.6f));
	//Scene_Lights.push_back(new Lightsource(glm::vec3(2.34f, -1.72f, -3.09f), glm::vec3(0.5f, 0.5f, 0.4f), glm::vec3(0.0f, 1.0f, 0.0f), 80.0f, 10.0f, 0.6f));

	Scene_Lights.push_back(new Lightsource(glm::vec3(0.65f, -1.72f, -1.26f), glm::vec3(0.5f, 0.5f, 0.4f), glm::vec3(0.0f, 1.0f, 0.0f), 80.0f, 10.0f, 0.6f));
	//Scene_Lights.push_back(new Lightsource(glm::vec3(0.65f, -1.72f, -3.09f), glm::vec3(0.5f, 0.5f, 0.4f), glm::vec3(0.0f, 1.0f, 0.0f), 80.0f, 10.0f, 0.6f));

	//

	// Scene_Lights.push_back(new Lightsource(glm::vec3(1.13f, -1.34f, -7.55f), glm::vec3(0.8f, 0.65f, 0.65f), glm::vec3(0.0f, 1.0f, 0.0f), 80.0f, 10.0f, 0.6f));

	/*
	New lights
	*/

	Scene_Lights.push_back(new Lightsource(glm::vec3(-0.54536f, -1.2184f, -10.531f), glm::vec3(0.8f, 0.65f, 0.65f), glm::vec3(0.0f, 1.0f, 0.0f), 80.0f, 10.0f, 0.6f));
	Scene_Lights.push_back(new Lightsource(glm::vec3(-2.259f, -1.2184f, -12.531f), glm::vec3(0.5f, 0.65f, 0.65f), glm::vec3(0.0f, 1.0f, 0.0f), 80.0f, 10.0f, 0.6f));

	Scene_Lights.push_back(new Lightsource(glm::vec3(-0.56941f, -1.4234f, -15.793f), glm::vec3(0.5f, 0.65f, 0.65f), glm::vec3(0.0f, 1.0f, 0.0f), 70.0f, 10.0f, 0.6f));
	Scene_Lights.push_back(new Lightsource(glm::vec3(2.1539f, -1.4234f, -15.793f), glm::vec3(0.5f, 0.65f, 0.65f), glm::vec3(0.0f, 1.0f, 0.0f), 70.0f, 10.0f, 0.6f));
	Scene_Lights.push_back(new Lightsource(glm::vec3(-3.2601f, -1.4234f, -15.793f), glm::vec3(0.5f, 0.65f, 0.65f), glm::vec3(0.0f, 1.0f, 0.0f), 70.0f, 10.0f, 0.6f));
	Scene_Lights.push_back(new Lightsource(glm::vec3(5.1f, -1.4234f, -15.793f), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.0f, 1.0f, 0.0f), 80.0f, 10.0f, 0.6f));

	Scene_Lights.push_back(new Lightsource(glm::vec3(8.1276f, -1.385f, -21.05f), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.0f, 1.0f, 0.0f), 80.0f, 10.0f, 0.6f));
	Scene_Lights.push_back(new Lightsource(glm::vec3(5.1276f, -2.0f, -21.5f), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(0.0f, 1.0f, 0.0f), 80.0f, 10.0f, 0.6f));

	//

	Blockmap::Initialise_Blockmap();

	// Must add dynamic objects AFTER blockmap is initialised

	Scene_Models.push_back(new Model({ MF_SOLID, MF_ACTIVE, MF_CAST_SHADOWS }, Object_Material::Enemy));
	Scene_Models.back()->Position = glm::vec3(3.773928, -0.05f, -3.415);
	Create_Model(Pull_Mesh("Assets/Models/Test_Person.obj", LOAD_MESH_ANIM_BIT | LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Person_Texture_2.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Test_Enemy_Controller(10.0f), { Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Test_Person.obj").Mesh) });

	//

	{
		Scene_Models.push_back(new Model({ MF_ACTIVE, MF_SOLID, MF_PHYSICS_TEST, MF_CAST_SHADOWS }, Object_Material::Barrel));
		Scene_Models.back()->Position = glm::vec3(2.2f, -0.7f, 8.27f);
		Create_Model(Pull_Mesh("Assets/Models/Barrel.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Barrel_UVs.png").Texture, Pull_Texture("Metal").Texture, Scene_Models.back(), new Physics_Object_Controller(), { Generate_Mesh_Hitbox(*Pull_Mesh("Assets/Hitboxes/Barrel.obj").Mesh) });

		static_cast<Physics_Object_Controller*>(Scene_Models.back()->Control)->Physics_Info->Elasticity *= 0.25;
		static_cast<Physics_Object_Controller*>(Scene_Models.back()->Control)->Time = 1000;
		static_cast<Physics_Object_Controller*>(Scene_Models.back()->Control)->Physics_Info->Mass = 4;
		static_cast<Physics_Object_Controller*>(Scene_Models.back()->Control)->Physics_Info->Inv_Mass = 1.0f / 4.0f;
	}

	// Lighting_BVH::Add_Light_Occluders(Generate_AABB_Hitboxes("Assets/Hitboxes/Intro_Level/Occluders.obj"));

	// Lighting_BVH::Add_Intro_Level_Light_Occluders();

	Lighting_BVH::Load_Light_Occluders("Assets/Light_Occluders/Intro_Level.obj");

	Lighting_BVH::Generate_Light_BVH_Tree();
	Lighting_BVH::Update_Leaf_Node_Data();

	Lighting_BVH::Load_BVH_Tree_Corrections("Assets/Light_Occluders/Intro_Level_Corrections.txt");

	//

	Audio::Set_Music_Tracks_For_Deletion(); // This stops any music currently playing
	Audio::Create_Music_Source(Pull_Audio("Assets/Audio/Music/Ambience_Track.wav").Source); // This plays the loaded ambience track

	//if (Music)
	//	Music->stop();

	//Music = Sound_Engine->play2D(Pull_Audio("Assets/Audio/Music/Ambience_Track.wav").Source, true, false, false, true);
	// Music->setVolume(Music_Volume);

	// Sound_Engine->play2D(Pull_Audio("Assets/Audio/Music/Ambience_Track.wav").Source, true, false);
}



#endif
#ifndef VISION_ENGINE_TITLE_SCREEN_HANDLING
#define VISION_ENGINE_TITLE_SCREEN_HANDLING

#include "../Engine_Loop_Includes.h"
#include "../Engine_Loop.h"
#include "../UI_Textbox_Definitions.h"

#include "Menus.h"

#include "../Light_BVH_Tree_Handler.h"

#include "Interactable_Declarations.h"

#include "../Hitdetection_Blockmap.h"

void Load_Test_Scene_Assets()
{
	Context_Interface::Loading_Progress_Total = 28;

	Push_Merged_Material<THREADED>("Assets/Textures/Brick_Specular.png", "Assets/Textures/Brick_Reflectivity.png", "Assets/Textures/Brick_Normal_Test.png", "Brick");

	Push_Merged_Material<THREADED>("Assets/Textures/Brick_Reflectivity.png", "Assets/Textures/Brick_Reflectivity.png", "Assets/Textures/Test_Normal.png", "Stone");

	Push_Merged_Material<THREADED>("Assets/Textures/Floor_Tile_Spec.png", "Assets/Textures/Brick_Reflectivity.png", "Assets/Textures/Floor_Tiles_Normal.png", "Floor");

	Push_Merged_Material<THREADED>("Assets/Textures/Floor_2_Tile_Specular.png", "Assets/Textures/Brick_Reflectivity.png", "Assets/Textures/Floor_2_Tile_Normal.png", "NPP_Wall");
	Push_Merged_Material<THREADED>("Assets/Textures/Reddened_Wall_Spec.png", nullptr, "Assets/Textures/Reddened_Wall_Normal.png", "NPP_Wall_2");

	Push_Merged_Material<THREADED>("Assets/Textures/Brick_Specular.png", "Assets/Textures/Flat_Reflectivity.png", "Assets/Textures/Brick_Normal_Test.png", "Floor_Reflect");

	Push_Merged_Material<THREADED>("Assets/Textures/Rust_Texture.png", "Assets/Textures/Rust_Reflectivity.png", "Assets/Textures/Rust_Normal.png", "Rust");

	Push_Merged_Material<THREADED>("Assets/Textures/Brick_Reflectivity.png", "Assets/Textures/Brick_Reflectivity.png", "Assets/Textures/Rubbish_Normal.png", "Rubbish");

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

	Initialise_Pistol();

	Pull_Mesh<THREADED>("Assets/Models/Intro_Level/Toilets.obj", LOAD_MESH_OBJ_BIT);
	Pull_Mesh<THREADED>("Assets/Models/Intro_Level/Tables.obj", LOAD_MESH_OBJ_BIT);
	Pull_Mesh<THREADED>("Assets/Models/Intro_Level/Heaters.obj", LOAD_MESH_OBJ_BIT);

	Pull_Mesh<THREADED>("Assets/Models/Intro_Level/Test_Intro_Level.obj", LOAD_MESH_OBJ_BIT);
	// Pull_Mesh<THREADED>("Assets/Hitboxes/Level_2_Map.obj", LOAD_MESH_OBJ_BIT);
	Pull_Mesh<THREADED>("Assets/Models/Test_Person.obj");
}

void Setup_Intro_Level()
{
	Scene_Models.push_back(new Model({ MF_SOLID, MF_CAST_SHADOWS, MF_USE_DECALS }, Object_Material::Concrete));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	//Create_Model(Pull_Mesh("Assets/Models/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Hitboxes/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Mesh));
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Test_Intro_Level.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall_2").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Models/Intro_Level/Test_Intro_Level.obj", LOAD_MESH_OBJ_BIT).Mesh));

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
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Tables.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Table1.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Controller(), Generate_AABB_Hitboxes("Assets/Hitboxes/Intro_Level/Tables.obj"));

	Scene_Models.push_back(new Model({ MF_CAST_SHADOWS, MF_USE_DECALS }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Toilets.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Toilet_Texture.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Controller(), std::vector<Hitbox*>{});

	Scene_Models.push_back(new Model({  }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Fire_Extinguisher_Signs.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Fire_Extinguisher_Sign.jpg").Texture, Pull_Texture("Metal").Texture, Scene_Models.back(), new Controller(), std::vector<Hitbox*>{});

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

	Scene_Models.push_back(new Model({ MF_SOLID, MF_USE_DECALS }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Air_Duct.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Vent_Duct.png").Texture, Pull_Texture("Vent").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Air_Duct.obj", LOAD_MESH_OBJ_BIT).Mesh));

	Scene_Models.push_back(new Model({ MF_SOLID, MF_CAST_SHADOWS, MF_USE_DECALS }, Object_Material::Floor_Tiles));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	//Create_Model(Pull_Mesh("Assets/Models/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Hitboxes/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Mesh));
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Test_Intro_Level_Floor.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Floor_Tiles.png").Texture, Pull_Texture("Floor").Texture, Scene_Models.back(), new Controller(), Generate_AABB_Hitboxes("Assets/Hitboxes/Intro_Level/Intro_Level_Floor_Hitbox.obj"));

	Scene_Models.push_back(new Model({ MF_SOLID, MF_CAST_SHADOWS, MF_USE_DECALS }, Object_Material::Floor_Tiles));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	//Create_Model(Pull_Mesh("Assets/Models/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Hitboxes/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Mesh));
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Intro_Level_Stairs.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Floor_Tiles.png").Texture, Pull_Texture("Floor").Texture, Scene_Models.back(), new Controller(), 
		std::vector<Hitbox*>{ 
			Generate_Mesh_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Stairs_0_Hitbox.obj", LOAD_MESH_OBJ_BIT).Mesh),
			Generate_Mesh_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Stairs_1_Hitbox.obj", LOAD_MESH_OBJ_BIT).Mesh)
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
	//Create_Model(Pull_Mesh("Assets/Models/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Hitboxes/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Mesh));
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Door_2.obj", LOAD_MESH_OBJ_BIT | LOAD_MESH_ANIM_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Door_Texture.png").Texture, Pull_Texture("Door").Texture, Scene_Models.back(), new Door_Controller("Assets/Animations/Intro_Level/Door_2_Open.anim"), std::vector<Hitbox*>{ /*Generate_AABB_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Door_2.obj", LOAD_MESH_OBJ_BIT).Mesh)*/ });

	Scene_Models.push_back(new Model({ MF_ACTIVE, MF_SOLID, MF_CAST_SHADOWS }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	//Create_Model(Pull_Mesh("Assets/Models/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Hitboxes/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Mesh));
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Door_3.obj", LOAD_MESH_OBJ_BIT | LOAD_MESH_ANIM_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Door_Texture.png").Texture, Pull_Texture("Door").Texture, Scene_Models.back(), new Door_Controller("Assets/Animations/Intro_Level/Door_3_Open.anim"), std::vector<Hitbox*>{ /*Generate_AABB_Hitbox(*Pull_Mesh("Assets/Hitboxes/Intro_Level/Door_2.obj", LOAD_MESH_OBJ_BIT).Mesh)*/ });

	Scene_Models.push_back(new Model({  }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Door_4.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Door_Texture.png").Texture, Pull_Texture("Door").Texture, Scene_Models.back(), new Controller(), std::vector<Hitbox*>{});

	//

	Scene_Models.push_back(new Model({  }, Object_Material::Wood));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Rutschgefahr.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Rutschgefahr.png").Texture, Pull_Texture("Floor").Texture, Scene_Models.back(), new Controller(), std::vector<Hitbox*>{});

	//

	Scene_Models.push_back(new Model({ MF_SOLID, MF_CAST_SHADOWS, MF_USE_DECALS }, Object_Material::Enemy));
	Scene_Models.back()->Position = glm::vec3(3.773928, -0.05f, -3.415);
	Create_Model(Pull_Mesh("Assets/Models/Test_Person.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Floor_Tiles.png").Texture, Pull_Texture("Floor").Texture, Scene_Models.back(), new Controller(), { Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Test_Person.obj").Mesh) });

	Scene_Models.push_back(new Model({ MF_SOLID, MF_CAST_SHADOWS, MF_USE_DECALS }, Object_Material::Enemy));
	Scene_Models.back()->Position = glm::vec3(1.394211, 0.0f, 0.438914);
	Create_Model(Pull_Mesh("Assets/Models/Test_Person.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Floor_Tiles.png").Texture, Pull_Texture("Floor").Texture, Scene_Models.back(), new Controller(), { Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Test_Person.obj").Mesh) });

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
	Scene_Lights.push_back(new Lightsource(glm::vec3(2.34f, -1.72f, -3.09f), glm::vec3(0.5f, 0.5f, 0.4f), glm::vec3(0.0f, 1.0f, 0.0f), 80.0f, 10.0f, 0.6f));

	Scene_Lights.push_back(new Lightsource(glm::vec3(0.65f, -1.72f, -1.26f), glm::vec3(0.5f, 0.5f, 0.4f), glm::vec3(0.0f, 1.0f, 0.0f), 80.0f, 10.0f, 0.6f));
	Scene_Lights.push_back(new Lightsource(glm::vec3(0.65f, -1.72f, -3.09f), glm::vec3(0.5f, 0.5f, 0.4f), glm::vec3(0.0f, 1.0f, 0.0f), 80.0f, 10.0f, 0.6f));

	//

	Scene_Lights.push_back(new Lightsource(glm::vec3(1.13f, -1.34f, -7.55f), glm::vec3(0.8f, 0.65f, 0.65f), glm::vec3(0.0f, 1.0f, 0.0f), 80.0f, 10.0f, 0.6f));

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
	Lighting_BVH::Generate_Light_BVH_Tree();

	Lighting_BVH::Update_Leaf_Node_Data();

	//
}

void Setup_Test_Scene()
{
	// Scene_Lights.push_back(new Lightsource(glm::vec3(0, 3, -3), glm::vec3(1.5, 1, 1.3), glm::vec3(0, 0, 1), 60, 3));
	Scene_Lights.push_back(new Lightsource(glm::vec3(0, 3, -3), glm::vec3(1, 1, 1.1), glm::vec3(0, 0, 1), 60, 3, 0.6f));

	// UI_Elements.push_back(new Button_UI_Element(-0.25, -0.1, 0.55, 0.5, Return_To_Game_Loop));

	// UI_Elements.push_back(new Button_UI_Element(-0.75, -0.9, -0.3, -0.3, Return_To_Game_Loop));

	// UI_Elements.push_back(new Button_UI_Element(-1, -1, 1, 1, Return_To_Game_Loop, Pull_Texture("Assets/Textures/Floor_Tiles.png").Texture));
	
	UI_Elements.push_back(new Button_Text_UI_Element(-0.9f, -0.9, 0.8f, -0.3, Return_To_Game_Loop, "Spiel beginnen? Das sieht\nschön aus! Scheiß drauf", false, glm::vec4(1.0f), &Font_Georgia, 0.1f));
	
	
	//UI_Elements.back()->Flags[UF_FILL_SCREEN] = true;

	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;

	/*Scene_Models.push_back(new Model({MF_ACTIVE}));
	Scene_Models.back()->Position = glm::vec3(0, -0.8, 0);
	Create_Model(Pull_Mesh("Assets/Models/Test_Makarov.obj", LOAD_MESH_OBJ_BIT | LOAD_MESH_ANIM_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Arm_Texture_2.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Test_Animation_Controller("Assets/Animations/Test_Makarov.anim"), { Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Viking_Room.obj").Mesh) });

	Scene_Models.push_back(new Model({ MF_ACTIVE }));
	Scene_Models.back()->Position = glm::vec3(0, -0.8, 0);
	Create_Model(Pull_Mesh("Assets/Models/Test_Makarov_Shoot.obj", LOAD_MESH_OBJ_BIT | LOAD_MESH_ANIM_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Gun_Texture.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Test_Animation_Controller("Assets/Animations/Test_Makarov_Shoot.anim"), { Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Viking_Room.obj").Mesh) });


	Scene_Models.push_back(new Model({ MF_SOLID, MF_ACTIVE }));
	Scene_Models.back()->Position = glm::vec3(-12, 0, -3);
	Create_Model(Pull_Mesh("Assets/Models/Test_Animation.obj", LOAD_MESH_OBJ_BIT | LOAD_MESH_ANIM_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Viking_Room.png").Texture, Pull_Texture("Brick").Texture, Scene_Models.back(), new Test_Animation_Controller("Assets/Animations/Test_Vertex_Anims.anim"), { Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Viking_Room.obj").Mesh) });

	Scene_Models.push_back(new Model({ MF_SOLID, MF_ACTIVE }));
	Scene_Models.back()->Position = glm::vec3(-8, 0, -3);
	Create_Model(Pull_Mesh("Assets/Models/Two_Bones_Test.obj", LOAD_MESH_OBJ_BIT | LOAD_MESH_ANIM_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Viking_Room.png").Texture, Pull_Texture("Brick").Texture, Scene_Models.back(), new Test_Animation_Controller("Assets/Animations/Two_Bones_Test.anim"), { Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Viking_Room.obj").Mesh) });
	*/

	Scene_Models.push_back(new Model({ MF_ACTIVE }, Object_Material::Enemy));
	Scene_Models.back()->Position = glm::vec3(-5.0f, -4.6f, -3.0f);
	Create_Model(Pull_Mesh("Assets/Models/Test_Makarov.obj", LOAD_MESH_OBJ_BIT | LOAD_MESH_ANIM_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Arm_Texture_2.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Test_Animation_Controller("Assets/Animations/Makarov_Hand_Reload.anim"), { Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Viking_Room.obj").Mesh) });

	Scene_Models.push_back(new Model({ MF_ACTIVE }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(-5.0f, -4.6f, -3.0f);
	Create_Model(Pull_Mesh("Assets/Models/Test_Makarov_Shoot.obj", LOAD_MESH_OBJ_BIT | LOAD_MESH_ANIM_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Gun_Texture.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Test_Animation_Controller("Assets/Animations/Makarov_Pistol_Reload.anim"), { Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Viking_Room.obj").Mesh) });


	//

	Scene_Models.push_back(new Model({ MF_SOLID, MF_CAST_SHADOWS }, Object_Material::Concrete));
	Scene_Models.back()->Position = glm::vec3(0, 0, 0);
	//Create_Model(Pull_Mesh("Assets/Models/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Hitboxes/Level_2_Map.obj", LOAD_MESH_OBJ_BIT).Mesh));
	Create_Model(Pull_Mesh("Assets/Models/Intro_Level/Test_Intro_Level.obj", LOAD_MESH_OBJ_BIT).Vertex_Buffer, Pull_Texture("Assets/Textures/Reddened_Wall.jpg").Texture, Pull_Texture("NPP_Wall").Texture, Scene_Models.back(), new Controller(), Wrap_AABB_Hitboxes(*Pull_Mesh("Assets/Models/Intro_Level/Test_Intro_Level.obj", LOAD_MESH_OBJ_BIT).Mesh));

	//

	Scene_Lights.push_back(new Lightsource(glm::vec3(-3, -4, -3), 0.5f * glm::vec3(2, 3, 4), glm::vec3(-1, 0, 0), 80, 10, 5.0f));

	// Scene_Lights.push_back(new Lightsource(glm::vec3(-5, -3, 2), 0.5f * glm::vec3(4, 3, 3), glm::normalize(glm::vec3(0.75, 0.5, 1)), 100, 0.1));

	// 554.793457, -34.912498, 0.000000

	/*
	
	Scene_Models.push_back(new Model({MF_SOLID}));
	Scene_Models.back()->Position = glm::vec3(0, 0, -3);
	Create_Model(Pull_Mesh("Assets/Models/Floor.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Brick1.png").Texture, Pull_Texture("Stone").Texture, Scene_Models.back(), new Controller(), { Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Floor.obj").Mesh) });

	Scene_Models.push_back(new Model({ MF_SOLID }));
	Scene_Models.back()->Position = glm::vec3(5, -5, -3);
	Create_Model(Pull_Mesh("Assets/Models/Floor.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Floor_Tiles.png").Texture, Pull_Texture("Floor").Texture, Scene_Models.back(), new Controller(), { Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Floor.obj").Mesh) });

	Scene_Models.push_back(new Model({ MF_SOLID }));
	Scene_Models.back()->Position = glm::vec3(18, -5, -3);
	Create_Model(Pull_Mesh("Assets/Models/Floor.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Floor_Tiles.png").Texture, Pull_Texture("Floor_Reflect").Texture, Scene_Models.back(), new Controller(), { Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Floor.obj").Mesh) });

	*/

	/*if (false)
	{
		Scene_Models.push_back(new Model({ MF_SOLID }));
		Scene_Models.back()->Position = glm::vec3(-5, -5, -3);
		Create_Model(Pull_Mesh("Assets/Models/Floor.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Brick1.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Controller(), { Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Floor.obj").Mesh) });

		Scene_Models.push_back(new Model({ MF_SOLID }));
		Scene_Models.back()->Position = glm::vec3(0, -5, -8);
		Create_Model(Pull_Mesh("Assets/Models/Floor.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Brick1.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Controller(), { Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Floor.obj").Mesh) });

		Scene_Models.push_back(new Model({ MF_SOLID }));
		Scene_Models.back()->Position = glm::vec3(0, -5, 2);
		Create_Model(Pull_Mesh("Assets/Models/Floor.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Brick1.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Controller(), { Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Floor.obj").Mesh) });
	}*/

	// 1.394211, -4.536097, 0.438914

	// 3.773928, -4.537358, -8.414638

	Scene_Models.push_back(new Model({ MF_SOLID, MF_CAST_SHADOWS }, Object_Material::Enemy));
	Scene_Models.back()->Position = glm::vec3(3.773928, -3.6, -8.415);
	Create_Model(Pull_Mesh("Assets/Models/Test_Person.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Floor_Tiles.png").Texture, Pull_Texture("Floor").Texture, Scene_Models.back(), new Controller(), { Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Test_Person.obj").Mesh) });

	Scene_Models.push_back(new Model({ MF_SOLID, MF_CAST_SHADOWS }, Object_Material::Enemy));
	Scene_Models.back()->Position = glm::vec3(1.394211, -3.6, 0.438914);
	Create_Model(Pull_Mesh("Assets/Models/Test_Person.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Floor_Tiles.png").Texture, Pull_Texture("Floor").Texture, Scene_Models.back(), new Controller(), { Generate_AABB_Hitbox(*Pull_Mesh("Assets/Models/Test_Person.obj").Mesh) });

	Volumetric_Cone_Particles.Particles.Spawn_Particle(glm::vec3(3.773928 - 1.0f, -3.6 - 1.0f, -8.415 - 1.0f), glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f)), glm::vec3(0.4, 0.6, 0.8), 0.6f, 30.0f);
	Scene_Lights.push_back(new Lightsource(glm::vec3(3.773928 - 1.0f, -3.6 - 1.0f, -8.415 - 1.0f), glm::vec3(1, 1.5, 2), glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f)), 30.0f, 1.0f, 0.6f));
	Scene_Models.push_back(new Model({ MF_CAST_SHADOWS, MF_SOLID }, Object_Material::Metal));
	Scene_Models.back()->Position = glm::vec3(3.773928 - 1.0f, -3.6 - 1.0f, -8.415 - 1.0f);
	Create_Model(Pull_Mesh("Assets/Models/Bubble.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Floor_Tiles.png").Texture, Pull_Texture("Floor").Texture, Scene_Models.back(), new Controller(), { Generate_Sphere_Hitbox(*Pull_Mesh("Assets/Models/Bubble.obj").Mesh) });


	// Scene_Models.push_back(new Model({ MF_SOLID }));
	// Scene_Models.back()->Position = glm::vec3(0, -5, -5);
	// Create_Model(Pull_Mesh("Assets/Models/Ramp.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/White.png").Texture, Pull_Texture("Brick").Texture, Scene_Models.back(), new Controller(), { Generate_Mesh_Hitbox(*Pull_Mesh("Assets/Models/Ramp.obj").Mesh) });

	glm::vec3 Rand_Direction = glm::normalize(glm::vec3(RNG() - 0.5f, RNG() - 0.5f, RNG() - 0.5f));

	//for (float X = 10; X < 40; X += 10)
	//	Volumetric_Cone_Particles.Particles.Spawn_Particle(glm::vec3(7.021941, -6.984860, -3.516123 + (X - 25.0f) * 0.3), Rand_Direction, glm::vec3(0.75, 0.75, sin(X * DTR)), 1.0f, X);

	for (size_t W = 0; W < 20; W++)
	{
		Scene_Lights.push_back(new Lightsource(glm::vec3(RNG() * 10 - 5, -4.8 /*-4.1*/, RNG() * 10 - 5), glm::vec3(RNG(), RNG(), RNG()), glm::vec3(0, 0, 0), 360.0f, 1.0f, 0.6f));
	}

	Scene_Lights.push_back(new Lightsource(glm::vec3(2.310351, -4.430879, 9.878356), glm::vec3(RNG(), RNG(), RNG()), glm::vec3(0, 0, 0), 360.0f, 1.0f, 0.6f));

	Lighting_BVH::Add_Test_Scene_Light_Occluders();

	Lighting_BVH::Generate_Light_BVH_Tree();

	Lighting_BVH::Update_Leaf_Node_Data();
}

void Run_Engine_Loop(UI_Element* Element)
{ 
	Delete_All_UI();

	//UI_Elements.push_back(new UI_Element(-1.0f, -1.0f, 1.0f, 1.0f));
	//UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;
	//UI_Elements.back()->Flags[UF_FILL_SCREEN] = true;

	Galaxy_Particles.Delete_All_Particles();

	//

	// Load_Test_Scene_Assets();

	bool Finished_Loading = false;

	Context_Interface::Loading_Progress = 1.0f;
	Context_Interface::Loading_Progress_Total = 1.0f;

	Job_System::Submit_Job(Job_System::Job(Loading_Function, new Loading_Function_Parameters(Load_Test_Scene_Assets, &Finished_Loading)));

	Loading_Screen_Loop(Finished_Loading);

	Delete_All_UI();

	//

	Setup_Intro_Level();

	Cursor_Reset = true;

	//Player_Camera.Position = glm::vec3(
	//	-1, -9.2, -4
	//);

	Engine_Loop(); 
}

void Title_Screen_Loop();

void Create_Title_Screen_Page()
{
	for (float Radius = 0.1f; Radius < 10.0f; Radius += 0.15f)
	{
		float Total = 64 * Radius;

		for (size_t W = 0; W < Total; W++)
			Galaxy_Particles.Particles.Spawn_Particle(Radius + RNG() * 0.3f, (W + RNG()) * 3.14159f * 2.0f / Total);

		for (size_t W = 0; W < 256 * Radius; W++)
			Galaxy_Particles.Particles.Spawn_Particle(Radius + RNG() * 0.1f, 6.28318f + (W + RNG() * 4.0f) * 3.14159f * 2.0f / (64 * Radius));
	}

	UI_Elements.push_back(new UI_Element(-1.0f, -1.0f, 1.0f, 1.0f)); // , Pull_Texture("Assets/UI/Title_Screen.png").Texture));
	UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;
	UI_Elements.back()->Flags[UF_FILL_SCREEN] = true;

	UI_Elements.push_back(new UI_Element(-0.9f, -0.98f, 2.1f, -0.48f, Pull_Texture("Assets/UI/Radiophobia_Title.png").Texture));
	UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;

	UI_Elements.push_back(new Button_Text_UI_Element(-0.9f, 0.5, 1.4f, 0.9f, Run_Engine_Loop, "Test.txt" /*"Drücken Sie diesen Knopf, um anzufangen"*/, true, glm::vec4(1.0f), &Font_Console, 0.1f, 0.01f));
	// UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;
	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;
	// UI_Elements.back()->Flags[UF_FILL_SCREEN] = true;
	
	// UI_Elements.back()->Image = Pull_Texture("Assets/Textures/Floor_Tiles.png").Texture;

	// UI_Elements.back()->Flags[UF_IMAGE] = true;

	//

	Place_Language_Buttons();

	// UI_Elements.push_back(new Textbox_UI_Element(-1.0f, -0.25f, 1.0f, 0.4f, 0.1f, 0.0f, &Font_Console));

	Cursor_Reset = false;

	Player_Camera.Position = glm::vec3(
		2.872747f, -1.759029f, 9.186198f
	);

	Player_Camera.Orientation = glm::vec3(
		616.330139f, -40.649986f, 0.000000f
	);

	Title_Screen_Loop();
}

void Title_Screen_Loop()
{
	while (!glfwWindowShouldClose(Window))
	{
		double Current_Time = glfwGetTime();
		Tick = Current_Time - Last_Time;
		Last_Time = Current_Time;

		glBindFramebuffer(GL_FRAMEBUFFER, 0u);


		glDepthMask(GL_TRUE);

		glClearColor(0.05f, 0.05f, 0.05f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Receive_Inputs();

		Audio::Handle_Audio(Player_Camera);

		Player_Camera.Set_Projection_Matrix();
		Player_Camera.Set_Audio_Observer();

		{
			Galaxy_Particles.Update();
			
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE); // We'll be using purely additive blending for the galaxy particles
			Galaxy_Particles.Shader.Activate();
			Galaxy_Particles.Render();
		}

		Handle_UI();

		Handle_Deletions();

		End_Of_Frame();
	}
}

#endif

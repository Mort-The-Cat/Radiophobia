#ifndef RADIOPHOBIA_ITEMS_DECLARATIONS
#define RADIOPHOBIA_ITEMS_DECLARATIONS

#include "..\OpenGL_Declarations.h"
#include "..\Model_Declarations.h"

namespace Player_Ammo_Count
{
	size_t Makarov_Ammo;
}

class Item
{
public:
	std::string Name;
	std::vector<Model> Viewmodel_Meshes; // This probably won't be added to the list of scene models, we'll just render them directly from the item. Less worries- less complicated
};

class Pistol : public Item
{
public:
	Mesh_Animator Draw; // This can be played forwards/backwards when putting away the pistol haha
	Mesh_Animator Shoot;
	Mesh_Animator Last_Shot; // This is when the player fires their last shot
	Mesh_Animator Reload;

	enum Pistol_State {
		Drawing,
		Idling,
		Shooting,
		Firing_Last,
		Reloading,
		Holstering
	} Current_State;

	// We'll just use a simple switch-statement to handle what the pistol does haha
	// Easy as pie

	Pistol() // To create the pistol object, we'll need to create its models, receive its name, receive its animations, etc
	{

	}

	// It can be hard coded that this uses makarov ani
};
#endif
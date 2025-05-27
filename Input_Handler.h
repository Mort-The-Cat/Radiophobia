#ifndef INPUT_HANDLER_VISION
#define INPUT_HANDLER_VISION

// In this, we'll really want a system that allows the player to re-map inputs etc

// We may have to refactor this in order to get the linker to behave

#include "OpenGL_Declarations.h"
#include "Job_System.h"
#include "Asset_Loading_Cache.h"
#include "Physics_Engine.h"

#include "Audio_Declarations.h"
#include "Audio_Handler_Declarations.h"

#include "Particle_System_Declarations.h"

#include "Game_Scenes/Menus_Declarations.h"

Physics::Physics_Object Player_Physics_Object;

#define Player_Movement_Revoke_Flag 0u
#define Player_Direction_Revoke_Flag 1u
#define Player_Items_Revoke_Flag 2u

bool Player_Flags[3];

bool Using_Controller = false;

void Handle_Player_Items();
void Delete_All();
void Setup_Intro_Level();

namespace Collision_Test
{
	bool Not_Against_Player_Compare(Hitbox* A, Hitbox* B)
	{
		return B != Player_Physics_Object.Object->Hitboxes[0];
	}

	bool Not_Wall_Return(Collision_Info Info)
	{
		return fabsf(Info.Collision_Normal.y) > 0.6f;
	}
}

void UI_Loop();

bool Inputs[12];

std::array<uint16_t, 12> Inputs_Keycode = // The values of these keycodes can be changed during runtime if the user wishes to adjust the controls for whatever reason
{
	GLFW_KEY_W, // Forwards
	GLFW_KEY_S, // backwards
	GLFW_KEY_A, // left
	GLFW_KEY_D, // right

	GLFW_KEY_SPACE,			// up 
	GLFW_KEY_LEFT_SHIFT,	// down

	GLFW_KEY_ESCAPE,		// Pause

	GLFW_KEY_Q,				// Lean left
	GLFW_KEY_E,				// Lean right

	GLFW_KEY_F, // Use

	GLFW_KEY_R, // Reload / auxilliary

	//

	GLFW_KEY_1 // Item 1
};

GLFWgamepadstate Controller_Inputs;

// Note that these only apply to keyboard usage!

namespace Gamepad_Controls
{
	uint8_t Pause = GLFW_GAMEPAD_BUTTON_START;
	uint8_t Use = GLFW_GAMEPAD_BUTTON_SQUARE;
	uint8_t Attack = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER;

	uint8_t Up = GLFW_GAMEPAD_BUTTON_CROSS;

	uint8_t Forward_Back = GLFW_GAMEPAD_AXIS_LEFT_Y; // These are the axes on the stick
	uint8_t Left_Right = GLFW_GAMEPAD_AXIS_LEFT_X;

	uint8_t Auxilliary = 4u;

	uint8_t Item_1 = GLFW_GAMEPAD_BUTTON_DPAD_LEFT;

	uint8_t Aim = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;

	uint8_t Look_Left_Right = GLFW_GAMEPAD_AXIS_RIGHT_X;
	uint8_t Look_Up_Down = GLFW_GAMEPAD_AXIS_RIGHT_Y;
}

namespace Controls // These are the indices for every user input. This may need some extra work if the user wishes to *type* something into an in-engine text box.
{
	const uint8_t Forwards = 0;
	const uint8_t Backwards = 1;
	const uint8_t Left = 2;
	const uint8_t Right = 3;

	const uint8_t Up = 4;
	const uint8_t Down = 5;

	const uint8_t Pause = 6;

	const uint8_t Lean_Left = 7;
	const uint8_t Lean_Right = 8;

	const uint8_t Use = 9;
	const uint8_t Auxilliary = 10;

	const uint8_t Item_1 = 11;
};

float Mouse_Sensitivity = 0.5;

bool Cursor_Reset = true;

glm::vec2 Cursor;

bool Mouse_Inputs[2]; // Left and right respectively

bool Mouse_Last_Clicked_Flags[2]; // This states whether or not the mouse was clicked on the previous frame or not

bool Mouse_Fresh_Click(bool Left_Or_Right_Mouse_Button)
{
	return Mouse_Inputs[Left_Or_Right_Mouse_Button] && !Mouse_Last_Clicked_Flags[Left_Or_Right_Mouse_Button];
}

bool Mouse_Unclick(bool Left_Or_Right_Mouse_Button)
{
	return !Mouse_Inputs[Left_Or_Right_Mouse_Button] && Mouse_Last_Clicked_Flags[Left_Or_Right_Mouse_Button];
}

//

#define Controller_Smoothing(Magnitude) (std::copysign(std::fmaxf(0, fabsf(Magnitude) * 1.2f - 0.2f), Magnitude))

void Apply_Controller_Inputs()
{
	double Cursor_X, Cursor_Y;

	Using_Controller = glfwGetGamepadState(GLFW_JOYSTICK_1, &Controller_Inputs);	// If there is NO controller connected, we stop using controller

	glfwGetCursorPos(Window, &Cursor_X, &Cursor_Y);

	glfwSetCursorPos(Window, 
		Cursor_X + Controller_Smoothing(Controller_Inputs.axes[Gamepad_Controls::Look_Left_Right]) * Tick * (float)Window_Height * 3.0f,
		Cursor_Y + Controller_Smoothing(Controller_Inputs.axes[Gamepad_Controls::Look_Up_Down]) * Tick * (float)Window_Height * 3.0f);

	Inputs[Controls::Pause] = Controller_Inputs.buttons[Gamepad_Controls::Pause];
	Inputs[Controls::Up] = Controller_Inputs.buttons[Gamepad_Controls::Up];
	Inputs[Controls::Auxilliary] = Controller_Inputs.buttons[Gamepad_Controls::Auxilliary];
	Inputs[Controls::Item_1] = Controller_Inputs.buttons[Gamepad_Controls::Item_1];
	Inputs[Controls::Use] = Controller_Inputs.buttons[Gamepad_Controls::Use];

	Mouse_Inputs[0] = Controller_Inputs.axes[Gamepad_Controls::Attack] > 0.35f; // If trigger is pulled, that's a left-click
	Inputs[Controls::Down] = Controller_Inputs.axes[Gamepad_Controls::Aim] > 0.35f;
}

void Receive_Inputs() // This sets all of the bits in "inputs", ready to be processed
{
	for (uint8_t W = 0; W < Inputs_Keycode.size(); W++)
		Inputs[W] = glfwGetKey(Window, Inputs_Keycode[W]) == GLFW_PRESS; // The inputs can be handled later

	memcpy(Mouse_Last_Clicked_Flags, Mouse_Inputs, sizeof(Mouse_Inputs)); // This copies the previously updated mouse inputs to the previous frame's input buffer

	Mouse_Inputs[0] = glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;		// The mouse buttons can't be remapped in-engine, but they can be used for whatever you want
	Mouse_Inputs[1] = glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

	double X, Y;

	glfwGetCursorPos(Window, &X, &Y);

	if (Cursor_Reset) glfwSetCursorPos(Window, Window_Width >> 1, Window_Height >> 1); // Middle of the screen

	if (Using_Controller)
		Apply_Controller_Inputs();

	glfwSetInputMode(Window, GLFW_CURSOR, Cursor_Reset ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

	Cursor = { X - (Window_Width >> 1), Y - (Window_Height >> 1) }; // This gets the cursor position! Very simple ^^

	Cursor *= glm::vec2(2.0f / Window_Width, -2.0f / Window_Height);
}

void Close_Game()
{
	glfwSetWindowShouldClose(Window, 1);
}

float Total_Timer = 0;

uint32_t Frames = 0;

//

float Player_Object_Spawn_Timer = 0;

void Spawn_Test_Object()
{
	if(Player_Object_Spawn_Timer < 0.0f)
	{
		Player_Object_Spawn_Timer = 0.05f;
		Scene_Models.push_back(new Model({ MF_ACTIVE, MF_PHYSICS_TEST, MF_SOLID, MF_CAST_SHADOWS }, Object_Material::Wood));
		Scene_Models.back()->Position = Player_Camera.Position + glm::vec3(RNG() * 1 - .5, RNG() * 1 - .5, RNG() * 1 - .5);

		// Create_Model(Pull_Mesh("Assets/Models/Particle_Test.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Smoke.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Physics_Object_Controller(), Generate_Sphere_Hitbox(*Pull_Mesh("Assets/Models/Particle_Test.obj").Mesh));
		
		Create_Model(Pull_Mesh("Assets/Models/Cardboard_Box.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Shelf_things.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Physics_Object_Controller(), { Generate_Mesh_Hitbox(*Pull_Mesh("Assets/Models/Cube.obj").Mesh) });
		static_cast<Physics_Object_Controller*>(Scene_Models.back()->Control)->Physics_Info->Elasticity *= 0.25;
		static_cast<Physics_Object_Controller*>(Scene_Models.back()->Control)->Time = 60;

		//Create_Model(Pull_Mesh("Assets/Models/Particle_Test.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/Smoke.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Physics_Object_Controller(), Generate_Sphere_Hitbox(*Pull_Mesh("Assets/Models/Particle_Test.obj").Mesh));
		
		//Create_Model(Pull_Mesh("Assets/Models/Mesh_Hitbox.obj").Vertex_Buffer, Pull_Texture("Assets/Textures/White.png").Texture, Pull_Texture("Black").Texture, Scene_Models.back(), new Physics_Object_Controller(), Generate_Mesh_Hitbox(*Pull_Mesh("Assets/Models/Mesh_Hitbox.obj").Mesh));
	}
}

void Shoot_Fire(float Angle)
{
	glm::vec3 Raycast_Velocity(-sin(Angle) * cos(DTR * Player_Camera.Orientation.y), -sin(DTR * Player_Camera.Orientation.y), -cos(Angle) * cos(DTR * Player_Camera.Orientation.y));

	Hitbox* Target = nullptr;
	Collision_Info Info = Collision_Test::Raycast(Player_Camera.Position, Raycast_Velocity * glm::vec3(0.05), 500, Collision_Test::Not_Against_Player_Compare, &Target);

	if (Target != nullptr)
	{
		// Fire_Sound->setVolume(1);

		Scene_Lights.push_back(new Lightsource(Info.Collision_Position - Info.Collision_Normal * glm::vec3(0.3), glm::vec3(RNG() * 1 + 2, RNG() + 1, RNG()), Info.Collision_Normal, 360, 1, 0.35f));
		Scene_Lights.back()->Flags[LF_TO_BE_DELETED] = true;
		Scene_Lights.back()->Flags[LF_CAST_SHADOWS] = true;
		
		//if(RNG() < 0.75)

		Billboard_Fire_Particles.Particles.Spawn_Particle(Info.Collision_Position + glm::vec3(0.1 * RNG() - 0.05, 0.1 * RNG() - 0.05, 0.1 * RNG() - 0.05), glm::vec3(-6)* Info.Collision_Normal + glm::vec3(.5 * RNG() - 0.25, .5 * RNG() - 0.25, .5 * RNG() - 0.25));

		//if(RNG() < 0.25)
		//	Billboard_Smoke_Particles.Particles.Spawn_Particle(Info.Collision_Position + glm::vec3(0.1 * RNG() - 0.05, 0.1 * RNG() - 0.05, 0.1 * RNG() - 0.05), glm::vec3(-4) * Info.Collision_Normal + glm::vec3(.25 * RNG() - 0.125, .25 * RNG() - 0.125, .25 * RNG() - 0.125));

		//	Smoke_Particles.Particles.Spawn_Particle(Info.Collision_Position + glm::vec3(0.1 * RNG() - 0.05, 0.1 * RNG() - 0.05, 0.1 * RNG() - 0.05), glm::vec3(-2) * Info.Collision_Normal + glm::vec3(.5 * RNG() - 0.25, .5 * RNG() - 0.25, .5 * RNG() - 0.25));

		if (Target->Object->Flags[MF_PHYSICS_TEST]) // If the object is a physics object
		{
			Physics_Object_Controller* Control = (Physics_Object_Controller*)Target->Object->Control;

			Control->Time = -1;

			Sound_Engine->play2D(Pull_Audio("Assets/Audio/Makarov.wav").Source);
		}
	}
}

bool Check_Feet_Touching_Ground(glm::vec3 Forward_Vector, glm::vec3* Perpendicular_Forward, glm::vec3* Perpendicular_Right)
{
	float Y_Position = ((AABB_Hitbox*)Player_Physics_Object.Object->Hitboxes[0])->B.y;
	float Radius = ((AABB_Hitbox*)Player_Physics_Object.Object->Hitboxes[0])->B.x - 0.005f;

	glm::vec3 Position = Player_Physics_Object.Object->Position;

	AABB_Hitbox Foot_Hitbox;
	Foot_Hitbox.Position = &Position;
	Foot_Hitbox.A = glm::vec3(-Radius, Y_Position + 0.01f, -Radius);
	Foot_Hitbox.B = glm::vec3(Radius, Y_Position + 0.01f, Radius);

	Hitbox* Collided_Hitbox;

	Collision_Info Collision = Collision_Test::Find_Collision(&Foot_Hitbox, Collision_Test::Not_Against_Player_Compare, &Collided_Hitbox);

	if (Collided_Hitbox != nullptr)
		if(Collision.Collision_Normal.y > 0.3)
		{
			*Perpendicular_Right = glm::normalize(glm::cross(Collision.Collision_Normal, Forward_Vector));

			*Perpendicular_Forward = glm::normalize(glm::cross(Collision.Collision_Normal, *Perpendicular_Right));

			return true;
		}

	return false;
}

float Jump_Timer = 0.0f;
#define Time_Between_Jumps 0.1f

bool Can_Player_Uncrouch()
{
	glm::vec3 Position = Player_Physics_Object.Object->Position;
	AABB_Hitbox Head_Hitbox;

	Head_Hitbox.Position = &Position;
	Head_Hitbox.A = reinterpret_cast<AABB_Hitbox*>(Player_Physics_Object.Object->Hitboxes[0])->A - glm::vec3(0.0f, 0.25f, 0.0f);
	Head_Hitbox.B = reinterpret_cast<AABB_Hitbox*>(Player_Physics_Object.Object->Hitboxes[0])->B - glm::vec3(0.0f, 0.25f, 0.0f);

	Hitbox* Collided_Hitbox;
	
	Collision_Test::Find_Collision(&Head_Hitbox, Collision_Test::Not_Against_Player_Compare, &Collided_Hitbox, Collision_Test::Not_Wall_Return);

	return Collided_Hitbox == nullptr;
}
/*
void Controller_Player_Movement()
{
	Player_Camera.Position = Player_Physics_Object.Object->Position;

	Player_Object_Spawn_Timer -= Tick;

	glfwGetGamepadState(GLFW_JOYSTICK_1, &Controller_Inputs);

	if (Controller_Inputs.buttons[Gamepad_Controls::Pause] || Window_Width == 0)
	{
		Open_Pause_Menu();
	}

	float Speed = -18.5 * Tick;

	float Angle = -DTR * Player_Camera.Orientation.x;

	float Movement_X = sin(Angle) * Speed;
	float Movement_Z = cos(Angle) * Speed;

	glm::vec3 Forward_Vector, Right_Vector;

	bool Feet_Touching_Ground = Check_Feet_Touching_Ground(glm::vec3(Movement_X, 0, Movement_Z), &Forward_Vector, &Right_Vector);

	if (Feet_Touching_Ground)
	{
		Forward_Vector *= Speed;
		Right_Vector *= Speed;

		Player_Physics_Object.Forces -= Forward_Vector * Controller_Inputs.axes[Gamepad_Controls::Forward_Back];
		Player_Physics_Object.Forces += Right_Vector * Controller_Inputs.axes[Gamepad_Controls::Left_Right];

		Jump_Timer += Tick;
	}

	if (Feet_Touching_Ground && Controller_Inputs.buttons[Gamepad_Controls::Up] && Jump_Timer > Time_Between_Jumps)
	{
		Player_Physics_Object.Forces.y -= 3.0f;

		Player_Physics_Object.Forces += Forward_Vector * (1 / Speed) * Controller_Inputs.axes[Gamepad_Controls::Forward_Back];

		Jump_Timer = 0.0f;
	}

	Cursor.x += Controller_Inputs.axes[2] * 0.3f;
	Cursor.y -= Controller_Inputs.axes[3] * 0.3f;

	Player_Camera.Orientation.x += Cursor.x * 90 * Mouse_Sensitivity;
	Player_Camera.Orientation.y += Cursor.y * 90 * Mouse_Sensitivity;

	Player_Camera.Orientation.y = std::max(Player_Camera.Orientation.y, -90.0f);
	Player_Camera.Orientation.y = std::min(Player_Camera.Orientation.y, 90.0f);
}
*/

inline void WASD_And_Analogue_Move(glm::vec3& Forward_Vector, glm::vec3& Right_Vector)
{
	if (Using_Controller)
	{
		Player_Physics_Object.Forces -= Forward_Vector * glm::vec3(Controller_Inputs.axes[Gamepad_Controls::Forward_Back]);
		Player_Physics_Object.Forces += Right_Vector * glm::vec3(Controller_Inputs.axes[Gamepad_Controls::Left_Right]);
		Player_Camera.Orientation.z += Tick * 30.0f * Controller_Inputs.axes[Gamepad_Controls::Left_Right];
	}
	else
	{
		if (Inputs[Controls::Forwards])
		{
			Player_Physics_Object.Forces += Forward_Vector;
		}
		if (Inputs[Controls::Backwards])
		{
			Player_Physics_Object.Forces -= Forward_Vector;
		}
		if (Inputs[Controls::Left])
		{
			Player_Physics_Object.Forces -= Right_Vector;

			Player_Camera.Orientation.z += Tick * 30.0f;
		}
		if (Inputs[Controls::Right])
		{
			Player_Physics_Object.Forces += Right_Vector;

			Player_Camera.Orientation.z -= Tick * 30.0f;
		}
	}
}

void Player_Movement()
{
	Player_Camera.Position = Player_Physics_Object.Object->Position;

	//

	Player_Object_Spawn_Timer -= Tick;

	if (Inputs[Controls::Pause] && Cursor_Reset || Window_Width == 0)
	{
		Open_Pause_Menu();
	}

	float Speed = -18.5 * Tick;

	if (Inputs[Controls::Down] || (reinterpret_cast<AABB_Hitbox*>(Player_Physics_Object.Object->Hitboxes[0])->B.y < 0.6f && !Can_Player_Uncrouch()))
	{
		Speed *= 0.75f; // You move 25% slower when crouched
		reinterpret_cast<AABB_Hitbox*>(Player_Physics_Object.Object->Hitboxes[0])->B.y = Fast::Approach(reinterpret_cast<AABB_Hitbox*>(Player_Physics_Object.Object->Hitboxes[0])->B.y, 0.5f, 1.25f * Tick);
	}
	else
		reinterpret_cast<AABB_Hitbox*>(Player_Physics_Object.Object->Hitboxes[0])->B.y = Fast::Approach(reinterpret_cast<AABB_Hitbox*>(Player_Physics_Object.Object->Hitboxes[0])->B.y, 0.9f, 0.75f * Tick);

	float Angle = -DTR * Player_Camera.Orientation.x;

	float Movement_X = sin(Angle) * Speed;
	float Movement_Z = cos(Angle) * Speed;

	glm::vec3 Forward_Vector, Right_Vector;

	bool Feet_Touching_Ground = Check_Feet_Touching_Ground(glm::vec3(Movement_X, 0, Movement_Z), &Forward_Vector, &Right_Vector);

	if(Feet_Touching_Ground && !Player_Flags[Player_Movement_Revoke_Flag])
	{
		Jump_Timer += Tick;

		Forward_Vector *= Speed;
		Right_Vector *= Speed;

		WASD_And_Analogue_Move(Forward_Vector, Right_Vector);
	}

	Player_Camera.Orientation.z *= powf(0.01f, Tick);

	if (Feet_Touching_Ground && Inputs[Controls::Up] && Jump_Timer > Time_Between_Jumps)
	{
		Player_Physics_Object.Forces.y -= 3.0f;

		Player_Physics_Object.Forces += Forward_Vector * (-1 / Speed);

		Jump_Timer = 0.0f;
	}

	if (!Player_Flags[Player_Direction_Revoke_Flag])
	{
		Player_Camera.Orientation.x += Cursor.x * 90 * Mouse_Sensitivity;
		Player_Camera.Orientation.y += Cursor.y * 90 * Mouse_Sensitivity;
	}

	Player_Camera.Orientation.y = std::max(Player_Camera.Orientation.y, -90.0f);
	Player_Camera.Orientation.y = std::min(Player_Camera.Orientation.y, 90.0f);

	//

	Handle_Player_Items();

	// This is used for getting lighting BVH corrections

	/*if (Inputs[Controls::Auxilliary])
	{
		printf(" >> Get_BVH_Node(X, Z) result:\n	 >> Node: %d\n	 >> [x,z]: (%f, %f)\n",
			Lighting_BVH::Get_BVH_Node(Player_Camera.Position.x, Player_Camera.Position.z),
			Player_Camera.Position.x, Player_Camera.Position.z);

		std::string Buffer =
			Encoder::Float_To_Characters(Player_Camera.Position.x) +
			Encoder::Float_To_Characters(Player_Camera.Position.z);

		printf(" >> Current x/z position as a float-to-text encoded string:\n	[%s]\n", Buffer.c_str());
	}*/

	/*if (Inputs[Controls::Auxilliary])
	{
		Engine_Continue_Looping = false;

		Wait_On_Physics();

		New_Scene_Loading_Buffer.Loading_Function = Delete_All;
		New_Scene_Loading_Buffer.Scene_Setup_Functions.push_back(Setup_Intro_Level);
	}*/

	//if (Inputs[Controls::Auxilliary])
	//{
	//	printf(" >> Camera info:\n%f, %f, %f\n%f, %f, %f\n", Player_Camera.Position.x, Player_Camera.Position.y, Player_Camera.Position.z, Player_Camera.Orientation.x, Player_Camera.Orientation.y, Player_Camera.Orientation.z);
	//}
}

#endif
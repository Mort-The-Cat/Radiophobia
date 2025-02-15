#ifndef OPENGL_DECLARATIONS_VISION
#define OPENGL_DECLARATIONS_VISION

#include "Fast_Maths.h"

#include "irrKlang.h"
#include "Audio_Declarations.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"

#include "glm/gtx/hash.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<array>
#include<vector>
#include<istream>
#include<fstream>
#include<iostream>
#include<functional>
#include<unordered_map>
#include<algorithm>
#include<mutex>

float Time_Elapsed_Since_FPS_Update = 0;

bool Is_Deleted(void* Pointer)
{
	return Pointer == nullptr;
}

struct Camera_Uniform_Location_Object
{
	int Projection_Matrix;
	int Camera_Position;
	int Camera_Up_Direction;
	int Camera_Direction;
} Camera_Uniform_Location;

const unsigned int Unassigned_Bit_Mask = 0xFFFFFFFF;

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define TINYGLTFLOADER_IMPLEMENTATION
#include <tiny_gltf_loader.h>

#include<random>

float Tick;

double Last_Time;

size_t Frame_Counter = 0;

void Throw_Error(const char* Error_Message)
{
	printf("%s", Error_Message);
	std::cin.get();
}

GLFWwindow* Window = nullptr;
// int Window_Width = 1600, Window_Height = 800;

int Window_Width = 1600, Window_Height = 800;

float Window_Aspect_Ratio = 0.5f; // Height / width

glm::mat4 Projection_Matrix; // This is the projection matrix of the current camera!
glm::vec3 Camera_Up_Direction;
glm::vec3 Camera_Direction;

namespace Context_Interface
{
	size_t Loading_Progress, Loading_Progress_Total;

	std::mutex Context_Mutex;

	void Assume_Context()
	{
		glfwMakeContextCurrent(Window);
	}

	void Request_Context()
	{
		Context_Mutex.lock();
		
		glfwMakeContextCurrent(Window);
	}

	void Free_Context()
	{
		glfwMakeContextCurrent(NULL);

		Context_Mutex.unlock();
	}
}

glm::vec3 Random_Perpendicular_Vector(glm::vec3 Vector)
{
	glm::vec3 Random_Vector = glm::vec3(RNG() - 0.5f, RNG() - 0.5f, RNG() - 0.5f);

	Random_Vector += Vector;

	glm::vec3 Perpendicular_Vector = glm::normalize(glm::cross(Vector, Random_Vector));

	return Perpendicular_Vector;
}

glm::mat3 Random_Direction_Matrix(glm::vec3 Vector)
{
	glm::vec3 Perpendicular_Vector = Random_Perpendicular_Vector(Vector);
	glm::vec3 Tangent_Vector = glm::cross(Perpendicular_Vector, Vector);

	return glm::mat3(Perpendicular_Vector, Vector, Tangent_Vector);
}

glm::vec3 Calculate_Surface_Normal(glm::vec3 A, glm::vec3 B, glm::vec3 C)
{
	// We'll need to calculate the surface normal of these three vectors

	glm::vec3 Normal = glm::normalize(glm::cross(A - C, B - C));

	return Normal;
}

class Camera
{
public:
	glm::vec3 Position;
	glm::vec3 Orientation; // This is in Euler angles

	float FOV;

	Camera() {}
	Camera(glm::vec3 Positionp, glm::vec3 Orientationp, float FOVp)
	{
		Position = Positionp;
		Orientation = Orientationp;
		FOV = FOVp;
	}

	void Set_Audio_Observer()
	{
		// Sound_Engine->setListenerPosition(Get_Klang_Vector(Position), Get_Klang_Vector(Camera_Direction), Get_Klang_Vector(glm::vec3(0)), Get_Klang_Vector(Camera_Up_Direction));

		Current_Listener_Right_Ear_Vector = -glm::cross(Camera_Direction, Camera_Up_Direction);
	}

	void Set_Projection_Matrix()
	{
		glm::mat4 View = glm::mat4(1.0f);

		View = glm::rotate(View, DTR * Orientation.z, glm::vec3(0, 0, 1));
		View = glm::rotate(View, DTR * Orientation.y, glm::vec3(1, 0, 0));
		View = glm::rotate(View, DTR * Orientation.x, glm::vec3(0, 1, 0));

		View = glm::translate(View, -Position);

		Projection_Matrix = glm::perspective(Fast::To_Radians(FOV), (float)Window_Width / (float)Window_Height, 0.01f, 100.0f);

		Projection_Matrix[1][1] *= -1;

		Projection_Matrix = Projection_Matrix * View;

		//

		glm::vec3 Direction(0, 0, 0);

		Direction.y = sinf(-Orientation.y * DTR);
		Direction.z = -sqrtf(1 - Direction.y * Direction.y);

		Direction.x = sinf(-Orientation.x * DTR) * Direction.z;

		Direction.z *= cosf(-Orientation.x * DTR);

		Camera_Direction = Direction;

		Camera_Up_Direction = glm::vec3(View[0][1], View[1][1], View[2][1]);
	}

	void Bind_Buffers(Camera_Uniform_Location_Object Location);
};

Camera Player_Camera(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), 90.0f);

std::string Get_File_Contents(const char* Directory) // IF YOU ARE LOADING TEXT WITH FOREIGN CHARACTERS, MAKE SURE THE FILE IS SAVED USING ANSI ENCODING
{
	std::ifstream File(Directory);
	if (File)
	{
		std::string Contents;

		File.seekg(0, std::ios::end);
		Contents.resize(File.tellg());

		File.seekg(0, std::ios::beg);

		File.read(&Contents[0], Contents.size());

		File.close();

		return Contents;
	}
	Throw_Error(" >> Unable to load file!");
}

class Shader
{
private:
	void Check_Shader_Errors(unsigned int Shader, const char* Type, const char* Name)
	{
		GLint Success;
		char Info_Log[1024];
		if (Type != "PROGRAM")
		{
			// We're checking either the vertex or fragment shader or the geometry shader

			glGetShaderiv(Shader, GL_COMPILE_STATUS, &Success);

			if (Success == GL_FALSE)
			{
				glGetShaderInfoLog(Shader, 1024, NULL, Info_Log);
				printf("\n >> SHADER COMPILATION ERROR! %s\n%s\n%s\n", Type, Info_Log, Name);
			}
		}
		else
		{
			// We know we're checking the *whole* shader program

			glGetProgramiv(Shader, GL_LINK_STATUS, &Success);

			if (Success == GL_FALSE)
			{
				glGetProgramInfoLog(Shader, 1024, NULL, Info_Log);
				printf("\n >> SHADER COMPILATION ERROR! %s\n%s\n%s\n", Type, Info_Log, Name);
			}
		}
	}
public:
	unsigned int Program_ID;

	void Activate()
	{
		glUseProgram(Program_ID);
	}

	void Create_Shader(const char* Vertex_File, const char* Fragment_File, const char* Geometry_File)
	{
		unsigned int Vertex_Shader, Fragment_Shader, Geometry_Shader;

		glUseProgram(0);

		Vertex_Shader = glCreateShader(GL_VERTEX_SHADER);
		Fragment_Shader = glCreateShader(GL_FRAGMENT_SHADER);

		if (Geometry_File != nullptr)
			Geometry_Shader = glCreateShader(GL_GEOMETRY_SHADER);

		std::string Vertex_Code = Get_File_Contents(Vertex_File);
		std::string Fragment_Code = Get_File_Contents(Fragment_File);
		std::string Geometry_Code;

		if(Geometry_File != nullptr)
			Geometry_Code = Get_File_Contents(Geometry_File);
		
		const char* Vertex_Source = Vertex_Code.c_str();
		const char* Fragment_Source = Fragment_Code.c_str();
		const char* Geometry_Source = Geometry_Code.c_str();

		glShaderSource(Vertex_Shader, 1, &Vertex_Source, NULL);
		glCompileShader(Vertex_Shader);

		Check_Shader_Errors(Vertex_Shader, "VERTEX", Vertex_File);

		glShaderSource(Fragment_Shader, 1, &Fragment_Source, NULL);
		glCompileShader(Fragment_Shader);

		Check_Shader_Errors(Fragment_Shader, "FRAGMENT", Fragment_File);

		if (Geometry_File != nullptr)
		{
			glShaderSource(Geometry_Shader, 1, &Geometry_Source, NULL);
			glCompileShader(Geometry_Shader);
			Check_Shader_Errors(Geometry_Shader, "GEOMETRY", Geometry_File);
		}

		Program_ID = glCreateProgram();
		glUseProgram(Program_ID);

		glAttachShader(Program_ID, Vertex_Shader);
		glAttachShader(Program_ID, Fragment_Shader);
		
		if (Geometry_File != nullptr)
			glAttachShader(Program_ID, Geometry_Shader);

		glLinkProgram(Program_ID);

		Check_Shader_Errors(Program_ID, "PROGRAM", "");

		glDeleteShader(Vertex_Shader);
		glDeleteShader(Fragment_Shader);

		if (Geometry_File != nullptr)
			glDeleteShader(Geometry_Shader);
	}
	
};

void Bind_Screen_Dimensions(Shader Shader_Program)
{
	glUniform2f(glGetUniformLocation(Shader_Program.Program_ID, "Screen_Dimensions"), 1.0f / Window_Width, 1.0f / Window_Height);
}

Shader Scene_Object_Shader;
Shader Loading_Screen_Spiral_Shader;
Shader Loading_Screen_Background_Shader;

bool Fullscreen = false;

namespace Post_Processor
{
	void Delete_Buffers();
	void Create_Buffers();
}

void Update_Window_Width_Height()
{
	const GLFWvidmode* Screen_Information = glfwGetVideoMode(glfwGetPrimaryMonitor());

	Window_Aspect_Ratio = (static_cast<float>(Screen_Information->height) / static_cast<float>(Screen_Information->width));

	// Window_Width = Screen_Information->width;

	Window_Height = Window_Aspect_Ratio * Window_Width;
}

void Framebuffer_Resize_Callback(GLFWwindow* Window, int Width, int Height)
{
	Window_Aspect_Ratio = static_cast<float>(Height) / static_cast<float>(Width);

	printf(" >> Window resized!!\n	w: %d\n	h: %d\n", Width, Height);

	glViewport(0, 0, Width, Height);

	Window_Width = Width;
	Window_Height = Height;

	Post_Processor::Delete_Buffers();
	Post_Processor::Create_Buffers();
}

void Initialise_OpenGL_Window()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	printf(" >> Open in fullscreen? [Y/N]\n\n >> ");

	char User_Input;

	std::cin >> User_Input;

	Fullscreen = std::tolower(User_Input) == 'y';

	if (Fullscreen)
	{
		Window_Width = 1400;

		Update_Window_Width_Height();

		Window = glfwCreateWindow(Window_Width, Window_Height, "Vision engine test!", glfwGetPrimaryMonitor(), NULL);

		glfwGetWindowSize(Window, &Window_Width, &Window_Height);

		// Update_Window_Width_Height(); // This sets the parameters for window creation
	}
	else
		Window = glfwCreateWindow(Window_Width, Window_Height, "Vision engine test!", NULL, NULL);

	/*printf(" >> Open in fullscreen? [Y/N]\n\n >> ");

	char Fullscreen;

	std::cin >> Fullscreen;

	if(std::tolower(Fullscreen) == 'y')
		Window = glfwCreateWindow(Window_Width, Window_Height, "Vision engine test!", glfwGetPrimaryMonitor(), NULL);
	else*/

	/*

	Window_Width = 1400;
		
	Window = glfwCreateWindow(Window_Width, Window_Height, "Vision engine test!", glfwGetPrimaryMonitor(), NULL);

	const GLFWvidmode* Screen_Information = glfwGetVideoMode(glfwGetPrimaryMonitor());

	Window_Aspect_Ratio = ((float)Screen_Information->height / (float)Screen_Information->width);

	Window_Height = Window_Aspect_Ratio * Window_Width;

	*/

	if (Window == NULL)
		Throw_Error(" >> Failed to create OpenGL Window!\n");

	glfwMakeContextCurrent(Window);

	glfwSwapInterval(0); // Disables Vsync

	gladLoadGL();


	//if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	//	Throw_Error(" >> Failed to initialise glad!\n");

	glfwSetWindowSizeCallback(Window, Framebuffer_Resize_Callback);

	stbi_set_flip_vertically_on_load(true);
}

void Close_Program()
{
	glfwTerminate();
}

#endif

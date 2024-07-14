#ifndef VISION_ENGINE_UI_RENDERER_DECLARATIONS
#define VISION_ENGINE_UI_RENDERER_DECLARATIONS

#include "OpenGL_Declarations.h"
#include "Vertex_Buffer_Declarations.h"
#include "Texture_Declarations.h"
#include "Asset_Loading_Cache.h"

namespace Font_Table
{
	uint8_t Table[] = { 67, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 68, 0, 0, 0, 0, 0, 69, 0, 0, 63, 0, 0, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 0, 67, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 68, 0, 0, 0, 0, 0, 69, 0, 0, 63, 0, 0, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 77, 76, 0, 0, 0, 0, 0, 70, 71, 0, 0, 75, 0, 74, 0, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 72, 73, 0, 0, 0, 78, 0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25 };

	uint8_t Get_Font_Index(char Input)
	{
		return Table[Input + 124];
	}

	void Generate_Text_Indices(const char* Input_String, std::vector<uint32_t>* Indices)
	{
		size_t W = 0;
		
		while(Input_String[W] != '\0')
		{
			if (Input_String[W] == -61) // If it's a non-important character, 
				W++;							// we just skip it

			Indices->push_back(Get_Font_Index(Input_String[W]));

			W++;
		}
	}

	// abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 �������():;.,"!?

	// Here is the specific order of the font characters used in this table

	Texture Font; // This is the texture for the font we're using. Every character has the same height and width so getting each character is analogous to array-indexing.

	const size_t Character_Width = 155u;
	const size_t Character_Height = 492u;

	const size_t Font_Grid_Width = 16u;
	const size_t Font_Grid_Height = 5u;

	constexpr float Character_Aspect_Ratio = static_cast<float>(Character_Height) / static_cast<float>(Character_Width);

	void Create_Font_Texture(std::vector<std::string> Directories)
	{
		Font.Create_Texture();

		stbi_uc* Pixels = new stbi_uc[Font_Grid_Width * Font_Grid_Height * (Character_Width * Character_Height * 4u)];

		for (size_t W = 0; W < Directories.size(); W++)
		{
			std::string Directory = "Assets/Font/" + Directories[W] + ".png";

			int Width, Height, Components;

			stbi_uc* Graphic = stbi_load(Directory.c_str(), &Width, &Height, &Components, 0u);

			size_t X_Offset = W % Font_Grid_Width;
			size_t Y_Offset = W / Font_Grid_Width;

			//memcpy(&Pixels[Character_Width * Character_Height * 4u * W], Graphic, Character_Width * Character_Height * 4u);

			for (size_t Y = 0; Y < Character_Height; Y++)
				memcpy(&Pixels[X_Offset * Character_Width * 4u + (Character_Width * 4u * Font_Grid_Width) * (Y + Y_Offset * Character_Height)], &Graphic[Y * Character_Width * 4u], Character_Width * 4u);

			stbi_image_free(Graphic);
		}

		glTexImage2D(GL_TEXTURE_2D, 0u, GL_RGBA, Character_Width * Font_Grid_Width, Character_Height * Font_Grid_Height, 0u, GL_RGBA, GL_UNSIGNED_BYTE, Pixels);

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(Pixels);
	}

	void Initialise_Font_Texture()
	{

		Create_Font_Texture(
			{
				"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",

				"Capital_A", "Capital_B", "Capital_C", "Capital_D", "Capital_E", "Capital_F", "Capital_G", "Capital_H", "Capital_I", "Capital_J", "Capital_K",
				"Capital_L", "Capital_M", "Capital_N", "Capital_O", "Capital_P", "Capital_Q", "Capital_R", "Capital_S", "Capital_T", "Capital_U", "Capital_V",
				"Capital_W", "Capital_X", "Capital_Y", "Capital_Z",

				"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
				"Space",
				"Eszett", "�", "�", "�", "Capital_�", "Capital_�", "Capital_�",

				"Open_Bracket", "Close_Bracket", "Colon", "Semicolon", "Fullstop", "Comma", "Quote", "Exclamation", "Question"
			});
	}
}

#define UF_TO_BE_DELETED 0u
#define UF_RENDER_BORDER 1u

bool UI_Continue_Looping = false;

Shader UI_Shader;
Shader Text_Shader;

void Initialise_UI_Shaders()
{
	UI_Shader.Create_Shader("Shader_Code/UI_Shader.vert", "Shader_Code/UI_Shader.frag", nullptr);

	Text_Shader.Create_Shader("Shader_Code/Text_Renderer.vert", "Shader_Code/Text_Renderer.frag", nullptr);
}

void Bind_UI_Uniforms(Shader UI_Shader, Texture Image, glm::vec4 Colour)
{
	glUniform4f(glGetUniformLocation(UI_Shader.Program_ID, "Colour"), Colour.x, Colour.y, Colour.z, Colour.w);

	//Font_Table::Font.Parse_Texture(UI_Shader, "Albedo", 0u);
	//Font_Table::Font.Bind_Texture();

	Image.Parse_Texture(UI_Shader, "Albedo", 0u);
	Image.Bind_Texture();
}

void Render_Screen_Sprite(float X1, float Y1, float X2, float Y2, glm::vec2 TL_UV, glm::vec2 TR_UV, glm::vec2 BL_UV, glm::vec2 BR_UV)
{
	// Make vertex buffer for this

	Billboard_Vertex_Buffer Vertex_Buffer(X1, Y1, X2, Y2, TL_UV, TR_UV, BL_UV, BR_UV); // This draws the image within the UI border

	// Vertex_Buffer.Bind_Buffer();

	glDrawElements(GL_TRIANGLES, Vertex_Buffer.Indices_Count, GL_UNSIGNED_INT, 0u);

	//

	Vertex_Buffer.Delete_Buffer(); // After we use it, we can just delete it

	// Might optimise this later but I don't care that much for the UI
}

/*

[-1, -1]-------[1,-1]
	|			|
	|			|
	|			|
	|			|
[-1, 1]--------[1, 1]


// This is how the coordinates for the UI are layed out.

X1,Y1 is equal to the top-left coords

X2,Y2 is equal to the bottom-right coords

*/

// From there, we will also handle aspect-ratio issues

// We want the vertical axis to remain between -1 and 1. The horizontal axis can change accordingly

class UI_Transformed_Coordinates
{
public:
	float X1, Y1, X2, Y2; // Bounds

	float X1o, Y1o, X2o, Y2o; // Offset
	UI_Transformed_Coordinates(float X1p, float Y1p, float X2p, float Y2p, float UI_Border_Size)
	{
		X1 = X1p;
		X1o = X1p + UI_Border_Size;

		Y1 = -Y2p;
		Y1o = UI_Border_Size - Y2p;

		X2 = X2p;
		X2o = X2p - UI_Border_Size;

		Y2 = -Y1p;
		Y2o = -UI_Border_Size - Y1p;

		//

		X1 *= Window_Aspect_Ratio;
		X1o *= Window_Aspect_Ratio;
		X2 *= Window_Aspect_Ratio;
		X2o *= Window_Aspect_Ratio;
	}
};

class UI_Element // The subclasses hereof will handle things like text, buttons, etc
{
public:
	float X1, Y1, X2, Y2;

	bool Flags[2] = { false, true };

	float UI_Border_Size = 1.0f / 20.0f;

	float Inv_UI_Border_Size = 20.0f;

	glm::vec4 Colour = glm::vec4(1, 1, 1, 1.0f);

	// X1 and Y1 are the top-left corner of the screen

	// X2 and Y2 are the bottom-right corner of the screen

	UI_Element() {}

	UI_Element(float X1p, float Y1p, float X2p, float Y2p)
	{
		X1 = X1p;
		Y1 = Y1p;
		X2 = X2p;
		Y2 = Y2p;
	}

	bool Button_Hover(UI_Transformed_Coordinates Coords)
	{
		return Coords.X1 <= Cursor.x && Coords.X2 >= Cursor.x && Coords.Y1 <= Cursor.y && Coords.Y2 >= Cursor.y;
	}

	virtual void Render_Border(UI_Transformed_Coordinates Coords)
	{
		Bind_UI_Uniforms(UI_Shader, Pull_Texture("Assets/UI/UI_Corner.png").Texture, Colour);

		Render_Screen_Sprite(Coords.X1, Coords.Y1, Coords.X1o, Coords.Y1o,
			{ 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 });

		Render_Screen_Sprite(Coords.X2o, Coords.Y1, 
			Coords.X2, Coords.Y1o,
			{ 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 });

		Render_Screen_Sprite(Coords.X1, Coords.Y2o, 
			Coords.X1o, Coords.Y2,
			{ 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 });

		Render_Screen_Sprite(Coords.X2o, Coords.Y2o, 
			Coords.X2, Coords.Y2,
			{ 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 });

		//

		Bind_UI_Uniforms(UI_Shader, Pull_Texture("Assets/UI/UI_Line.png").Texture, Colour);

		float Delta_X_UV = (Coords.X2 - Coords.X1) * Inv_UI_Border_Size - 2;
		float Delta_Y_UV = (Coords.Y2 - Coords.Y1) * Inv_UI_Border_Size - 2;

		Render_Screen_Sprite(Coords.X1o, Coords.Y1,
			Coords.X2o, Coords.Y1o, 
			{ 0, 0 }, { Delta_X_UV, 0 }, { 0, 1 }, { Delta_X_UV, 1 });

		Render_Screen_Sprite(Coords.X1o, Coords.Y2o,
			Coords.X2o, Coords.Y2,
			{ 0, 0 }, { Delta_X_UV, 0 }, { 0, 1 }, { Delta_X_UV, 1 });

		//

		Render_Screen_Sprite(Coords.X1, Coords.Y1o, Coords.X1o, Coords.Y2o,
			{ 0, 0 }, { 0, 1 }, { Delta_Y_UV, 0 }, { Delta_Y_UV, 1 });

		Render_Screen_Sprite(Coords.X2o, Coords.Y1o, Coords.X2, Coords.Y2o,
			{ 0, 0 }, { 0, 1 }, { Delta_Y_UV, 0 }, { Delta_Y_UV, 1 });
	}

	virtual void Render(UI_Transformed_Coordinates Coords)
	{
		UI_Shader.Activate();
		
		if (Flags[UF_RENDER_BORDER])
			Render_Border(Coords);

		Render_Screen_Sprite(Coords.X1o, Coords.Y1o, Coords.X2o, Coords.Y2o,
			{ 0, 1 }, { 1, 1 }, { 0, 0 }, { 1, 0 });
	}

	virtual void Update_UI()
	{
		UI_Transformed_Coordinates Coords(X1, Y1, X2, Y2, UI_Border_Size);

		Render(Coords);

		// To handle this UI, there's nothing else we have to do
	}
};

class Text_UI_Element : public UI_Element
{
	std::vector<uint32_t> Character_Indices; // These are the indices of the letters in the font

	float Size;

	bool Centered_X = false, Centered_Y = false;

public:
	Text_UI_Element(float X1p, float Y1p, float X2p, float Y2p, std::string Textp, float Sizep = 1.0f / 15.0f)
	{
		X1 = X1p;
		Y1 = Y1p;
		X2 = X2p;
		Y2 = Y2p;

		Font_Table::Generate_Text_Indices(Textp.c_str(), &Character_Indices);

		Size = Sizep;
	}

	virtual void Render_Text(UI_Transformed_Coordinates Coords)
	{


		Billboard_Vertex_Buffer Letter(
			Coords.X1o + Size * Window_Aspect_Ratio,
			Coords.Y2o - Size,
			Coords.X1o + Size * 2 * Window_Aspect_Ratio,
			Coords.Y2o - Size * (1 + Font_Table::Character_Aspect_Ratio),

			// glm::vec2(0, 0.2f), glm::vec2(1.0f/16.0f, 0.2f),
			// glm::vec2(0, 0.0f), glm::vec2(1.0f/16.0f, 0.0f)

			glm::vec2(0.03f / 16.0f, 0.2f), glm::vec2(0.97f / 16.0f, 0.2f),
			glm::vec2(0.03f / 16.0f, 0.0f), glm::vec2(0.97f / 16.0f, 0.0f)
		);

		//Billboard_Vertex_Buffer Letter(Coords.X1o, Coords.Y1o, Coords.X2o, Coords.Y2o);

		glUniform1f(glGetUniformLocation(Text_Shader.Program_ID, "Size_Of_Letter"), (Size + 0.01f) * Window_Aspect_Ratio);

		glUniform1uiv(glGetUniformLocation(Text_Shader.Program_ID, "Character_Indices"), Character_Indices.size(), Character_Indices.data());
		
		glDrawElementsInstanced(GL_TRIANGLES, Letter.Indices_Count, GL_UNSIGNED_INT, 0u, Character_Indices.size());

		Letter.Delete_Buffer();
	}

	virtual void Render(UI_Transformed_Coordinates Coords) override
	{
		UI_Shader.Activate();

		if (Flags[UF_RENDER_BORDER])
			Render_Border(Coords);

		// Bind_UI_Uniforms(Pull_Texture("Assets/Font/Eszett.png").Texture, Colour);

		Render_Screen_Sprite(Coords.X1o, Coords.Y1o, Coords.X2o, Coords.Y2o,
			{ 0, 1 }, { 1, 1 }, { 0, 0 }, { 1, 0 });

		Text_Shader.Activate();

		Bind_UI_Uniforms(Text_Shader, Font_Table::Font, Colour);

		Render_Text(Coords);
	}
};

class Button_UI_Element : public UI_Element
{
public:
	void (*Button_Function)(UI_Element*);

	Button_UI_Element(float X1p, float Y1p, float X2p, float Y2p, void (*Button_Functionp)(UI_Element*))
	{
		X1 = X1p;
		Y1 = Y1p;
		X2 = X2p;
		Y2 = Y2p;

		Button_Function = Button_Functionp;
	}

	virtual void Update_UI() override
	{
		UI_Transformed_Coordinates Coords(X1, Y1, X2, Y2, UI_Border_Size);

		bool Hovering = Button_Hover(Coords);

		if (Hovering)
			Colour.b = 0.5f;
		else
			Colour.b = 1.0f;

		Render(Coords);

		if (Hovering && Mouse_Inputs[0]) // If we're hovering over the button and the mouse is clicking, 
			Button_Function(this);		// run the button-function
	}
};

void Return_To_Game_Loop(UI_Element* Element)
{
	UI_Continue_Looping = false;
	Cursor_Reset = true;
}

std::vector<UI_Element*> UI_Elements;

// Will need some hash tables and functions for handling text rendering

// Get a conversion thingy from character to index into font

void Handle_UI() // If there are transparent UI elements (pretty likely), then we're gonna want to render this in a final transparent rendering pass
{
	glDisable(GL_CULL_FACE);

	glDisable(GL_DEPTH_TEST);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (size_t W = 0; W < UI_Elements.size(); W++)
		UI_Elements[W]->Update_UI();
}

#endif
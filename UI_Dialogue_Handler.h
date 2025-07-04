#ifndef VISION_UI_DIALOGUE_HANDLER
#define VISION_UI_DIALOGUE_HANDLER

#include "UI_Renderer_Declarations.h"
#include "Input_Handler.h"

void Dialogue_Master_Next(UI_Element* Element);
void Dialogue_Master_Response(UI_Element* Element);
void Dialogue_Master_End(UI_Element* Element);

UI_Element* Current_Dialogue_Master;

std::string Attach_Dialogue_Cursor(std::string Text, float Timer)
{
	int Index = Text.size() - 1;
	while (Index > 0)
	{
		if (Text[Index - 1] != '\n')
		{
			Text[Index] = fmodf(Timer, 0.8f) < 0.4f ? '_' : ' ';
			break;
		}

		Index--;
	}

	return Text;
}

class UI_Computer_Flicker : public UI_Controller
{
public:
	float Timer = 0.0f;

	virtual void Control_Function(UI_Element* Element) override
	{
		Timer += Tick;

#define COMPUTER_ZOOM 2.5f

		float Scalar = (1 + floor(16.0f * MIN(Timer, 1)) / 16.0f) * 0.5f;

		Element->X1 = Scalar * COMPUTER_ZOOM * -1.7777f;
		Element->Y1 = Scalar * COMPUTER_ZOOM * -1.0f;
		Element->X2 = Scalar * COMPUTER_ZOOM * 1.7777f;
		Element->Y2 = Scalar * COMPUTER_ZOOM * 1.0f;

		while (Timer > 2.2f)
		{
			Timer -= 0.2f;

			if (rand() & 1u)
				Element->Colour = glm::vec4(1.0f, 0.8f, 1.0f, 1.0f);
			else
				Element->Colour = glm::vec4(1.0f);
		}
	}
};

class UI_Dialogue_Master : public UI_Controller
{
public:
	float Timer;							// This counts the delay for the pause instruction and text scrolling
	uint16_t Instruction_Index = 0;			// This is the instruction that the DM is currently on
	uint16_t Text_Index = 0;				// This is the next line of text to be read
	uint32_t Character_Index = 0;			// This is the index of the current character on screen
	std::vector<std::string> Text;			// This is the text data
	std::vector<unsigned char> Instructions;	// This is the instruction data

	std::string Onscreen_Text;				// This is all of the text that is on screen currently (can be cleared and added to)

	uint16_t Last_Frame_Pause_Was_Pressed = false;
	uint16_t Toggle_Fast_Scroll = 1;

#define TIME_BETWEEN_CHARACTERS 0.0075f

	virtual void Control_Function(UI_Element* Element) override
	{
		Timer += Tick * (float)Toggle_Fast_Scroll;

		if (Last_Frame_Pause_Was_Pressed && !Inputs[Controls::Pause])
			UI_Continue_Looping = false;

		Last_Frame_Pause_Was_Pressed = Inputs[Controls::Pause];

		((Text_UI_Element*)Element)->Text = Attach_Dialogue_Cursor(Onscreen_Text, Timer);// +(fmodf(Timer, 0.8f) < 0.4 ? "_" : Block);

		if(Text_Index < Text.size())
			if (Text[Text_Index][Character_Index])	// If the string is NOT terminated just yet,
			{
				while (TIME_BETWEEN_CHARACTERS < Timer)
				{
					Timer -= TIME_BETWEEN_CHARACTERS;
					if (Text[Text_Index][Character_Index])
						Onscreen_Text += Text[Text_Index][Character_Index];
					else
						break;
					Character_Index++;
				}
			}
			else // If we've finished writing to the screen, handle instructions
			{
				if (Instruction_Index < Instructions.size()) // Just double check that there *is* an instruction to do
				{
					Current_Dialogue_Master = Element;

					switch (Instructions[Instruction_Index])
					{
					case 'f':
						Toggle_Fast_Scroll = 3u - Toggle_Fast_Scroll;
						Character_Index = 0;
						Instruction_Index++;
						Text_Index++;
						break;

					case 'p':
						if (Timer > 0.5f)
						{
							Text_Index++;
							Character_Index = 0;
							Instruction_Index++;
							Timer = 0.0f;
						}
						break;

					case 'c':
						Onscreen_Text.clear();
						Instruction_Index++;
						Character_Index = 0;
						Text_Index++;
						break;

					case 'n':
						// Handle 'next' stuff
						if (Timer < 1000)
						{
							// Create the UI button that says "next"

							UI_Elements.push_back(new Button_Text_UI_Element(-0.3f, 0.65f, 0.9f, 0.9f, Dialogue_Master_Next, "NEXT ->", false, 
								glm::vec4(1.0f, 1.0f, 1.0f, 2.0f), &Font_Console, 0.075f));

							UI_Elements.back()->Colour.w = 0.5f;

							UI_Elements.back()->Flags[UF_IMAGE] = true;

							//UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;
							UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
							UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = true;

							UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;

							UI_Elements.back()->Flags[UF_AUTO_RESIZE_TO_TEXT] = true;
							UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;

							Timer += 1000;
						}
						break;

					case 'r':
						// Basically identical to 'n'

						if (Timer < 1000)
						{
							// Create the UI button with the text in Text[Text_Index + 1]

							UI_Elements.push_back(new Button_Text_UI_Element(-0.7f, 0.65f, 0.9f, 0.9f, Dialogue_Master_Response, Text[Text_Index + 1], false,
								glm::vec4(1.0f, 1.0f, 1.0f, 2.0f), &Font_Console, 0.075f));

							UI_Elements.back()->Colour.w = 0.5f;

							UI_Elements.back()->Flags[UF_IMAGE] = true;

							//UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;
							UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
							UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = true;

							UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;

							UI_Elements.back()->Flags[UF_AUTO_RESIZE_TO_TEXT] = true;
							UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;

							Timer += 1000;
						}
						break;

					default:	// This is one of the clear cases
						size_t Count = Instructions[Instruction_Index];
						size_t Index = 0;
						while (Count && Index < Onscreen_Text.size())
						{
							Count -= Onscreen_Text[Index] == '\n';

							Index++;
						}

						Onscreen_Text.resize(Index);
						Instruction_Index++;
						Character_Index = 0;
						Text_Index++;
					}
				}
				else
				{
					// We've done all the instructions and all the text has been displayed successfully...

					if (Timer < 1000)
					{
						// Create the UI button that says "next"

						UI_Elements.push_back(new Button_Text_UI_Element(-0.3f, 0.65f, 0.9f, 0.9f, Dialogue_Master_End, "END ->", false,
							glm::vec4(1.0f, 1.0f, 1.0f, 2.0f), &Font_Console, 0.075f));

						UI_Elements.back()->Colour.w = 0.5f;

						UI_Elements.back()->Flags[UF_IMAGE] = true;

						//UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;
						UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
						UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = true;

						UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;

						UI_Elements.back()->Flags[UF_AUTO_RESIZE_TO_TEXT] = true;
						UI_Elements.back()->Flags[UF_CENTRE_TEXT] = true;

						Timer = 1001;
					}
				}
			}
	}
};

// For technical reasons, the player isn't able to change language during dialogue.
// Technically, they *could*, 
// but I'd just have to reload the whole dialogue with the new language settings

// This would mean that they'd have to restart the cutscene and whatnot.

void Dialogue_Master_End(UI_Element* Element)
{
	UI_Continue_Looping = false;

	Fade_From_Colour(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 0.8f);
}

void Dialogue_Master_Next(UI_Element* Element)
{
	((UI_Dialogue_Master*)Current_Dialogue_Master->Controller)->Instruction_Index++;
	((UI_Dialogue_Master*)Current_Dialogue_Master->Controller)->Text_Index++;
	((UI_Dialogue_Master*)Current_Dialogue_Master->Controller)->Character_Index = 0;

	((UI_Dialogue_Master*)Current_Dialogue_Master->Controller)->Timer = 0.0f;

	Element->Flags[UF_TO_BE_DELETED] = true; // We'll delete this object now!
}

void Dialogue_Master_Response(UI_Element* Element)
{
	((UI_Dialogue_Master*)Current_Dialogue_Master->Controller)->Instruction_Index++;
	((UI_Dialogue_Master*)Current_Dialogue_Master->Controller)->Text_Index+=2;
	((UI_Dialogue_Master*)Current_Dialogue_Master->Controller)->Character_Index = 0;

	((UI_Dialogue_Master*)Current_Dialogue_Master->Controller)->Timer = 0.0f;

	Element->Flags[UF_TO_BE_DELETED] = true; // We'll delete this object now!
}

void Load_Dialogue(std::string Directory, UI_Dialogue_Master* Target_DM)
{
	// This will read and handle the file line-by-line

	Target_DM->Instructions = { 'p', 'p', 'p' };
	Target_DM->Text = { "", "", "", "" };

	std::ifstream File(("Assets/Text/" + Current_Language_Setting + "/" + Directory).c_str());

	if (File.is_open())
	{
		std::string Line_Input;

		while (std::getline(File, Line_Input))
		{
			if (Line_Input[0] == '^') // This is an instruction
			{
				if(Target_DM->Text.size())
					Target_DM->Text.push_back(""); // This stops us from writing to the previous line, giving us a new line to append to

				Target_DM->Instructions.push_back(Line_Input[1]); // Pushes instruction code

				if (Line_Input[1] == 'r')
				{
					std::getline(File, Line_Input);
					Target_DM->Text.back() = Line_Input;
					Target_DM->Text.push_back("");
				}

				if (Line_Input[1] == 's')
				{
					Target_DM->Instructions.back() = Line_Input[2];
					Target_DM->Instructions.back() -= 47u;
				}
			}
			else
				Target_DM->Text.back() += Line_Input + "\n"; // This appends the newest line of text to the current string
		}

		File.close();
	}
	else
	{
		Throw_Error(" >> Failed to load dialogue file!\n");
	}
}

void Setup_Intro_Dialogue()
{
	UI_Elements.push_back(new UI_Element(COMPUTER_ZOOM * -1.7777f, COMPUTER_ZOOM * -1.0f, COMPUTER_ZOOM * 1.7777f, COMPUTER_ZOOM * 1.0f, Pull_Texture("Assets/UI/Computer_Edited.png").Texture, new UI_Computer_Flicker()));
	// UI_Elements.back()->Flags[UF_FILL_SCREEN] = true;
	UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;
	UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = true;
	UI_Elements.back()->UI_Border_Size = 0.0f;
	UI_Elements.back()->Inv_UI_Border_Size = 0.0f;

	// UI_Elements.back()->Colour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	/*UI_Elements.push_back(new Text_UI_Element(-0.7f, -0.9f, 2.4f, 1.0f, "", false, glm::vec4(1.0f), &Font_Console));
	// UI_Elements.back()->Flags[UF_FILL_SCREEN] = true;
	UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;
	UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;

	UI_Elements.back()->Flags[UF_CLAMP_TO_SIDE] = true;
	UI_Elements.back()->Flags[UF_CLAMP_RIGHT] = false;*/

	UI_Elements.push_back(new Text_UI_Element(-1.2f, -0.8f, 1.55f, 1.0f, "", false, glm::vec4(1.0f), &Font_Console, 0.06f));
	// UI_Elements.back()->Flags[UF_FILL_SCREEN] = true;
	UI_Elements.back()->Flags[UF_RENDER_CONTENTS] = false;
	UI_Elements.back()->Flags[UF_RENDER_BORDER] = false;

	((Text_UI_Element*)UI_Elements.back())->Text_Colour = glm::vec4(0.1f, 0.75f, 0.1f, 0.75f);
	UI_Elements.back()->Controller = new UI_Dialogue_Master();

	Load_Dialogue("Dialogue/Intro.txt", (UI_Dialogue_Master*)UI_Elements.back()->Controller);

	UI_Loop();

	Cache::Remove_From_Texture_Cache("Assets/UI/Computer_Edited.png"); // This line might be redundant - just clearing the cache slightly

	// Especially since this image is like a 1920x1040 PNG... That expands to a lot of pixels

	Delete_All_UI();
}

#endif
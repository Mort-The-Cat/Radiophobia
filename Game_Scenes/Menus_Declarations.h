#ifndef MENUS_DECLARATIONS_RADIOPHOBIA
#define MENUS_DECLARATIONS_RADIOPHOBIA

class UI_Element;

void Open_Pause_Menu(UI_Element* Element = nullptr);

void Open_Settings_Menu(UI_Element* Element = nullptr);

enum class Difficulty
{
	Debug_Mode = 0,
	God_Mode = 1,
	Easy = 2,
	Medium = 3,
	Hard = 4,
	No_Clip = 5			// I'll implement all of these things later
};

Difficulty Selected_Difficulty = Difficulty::Medium;

#endif
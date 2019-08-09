#include "console.h"

#ifdef _WIN32
// WINDOWS WRAPPER //

namespace console
{
	// Static variables
	HANDLE out;
	HWND window;

	CONSOLE_CURSOR_INFO cur;
	CONSOLE_SCREEN_BUFFER_INFO cbsi;

	unsigned int original_col;

	// Color const variables
	const unsigned int red = 12;
	const unsigned int green = 10;
	const unsigned int blue = 9;
	const unsigned int white = 7;

	// Initializes wrapper with console title
	void init(const std::string& title)
	{
		window = GetConsoleWindow();
		out = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTitle(title.c_str());

		CONSOLE_SCREEN_BUFFER_INFO con_info;
		GetConsoleScreenBufferInfo(out, &con_info);
		original_col = con_info.wAttributes;
	}

	// Sets color of font
	void color(const int& c)
	{
		SetConsoleTextAttribute(out, c);
	}

	// Resets color to its original state
	void reset()
	{
		color(original_col);
	}

	// Gets x position of cursor on screen
	int getx()
	{
		GetConsoleScreenBufferInfo(out, &cbsi);
		return cbsi.dwCursorPosition.X;
	}

}

#elif __linux__
// LINUX WRAPPER //
// TODO: Check if works

namespace console
{
	const unsigned int red = 31;
	const unsigned int green = 32;
	const unsigned int blue = 34;
	const unsigned int white = 37;

	// Initializes wrapper with console title
	void init(const std::string& title)
	{
		std::cout << "\033]0;" << title << "\007";
	}

	// Sets color of font
	void color(const int& c)
	{
		std::cout << "\033[" + std::to_string(c) + "m";
	}

	// Resets color to its original state
	void reset()
	{
		std::cout << "\033[0m";
	}

	// Gets x position of cursor on screen
	int getx()
	{
		// TODO: Find a reliable way to do this on linux
	}

}

#endif
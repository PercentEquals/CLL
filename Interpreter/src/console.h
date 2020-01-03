#pragma once

// Simple console wrapper that provides additional console manipulation

#ifdef _WIN32

#define _WIN32_WINNT 0x0500
#include <windows.h>

#endif

#include <iostream>
#include <string>

namespace console
{
	// Color const variables
	extern const unsigned int red;
	extern const unsigned int green;
	extern const unsigned int blue;
	extern const unsigned int white;

	void init(const std::string& title); // Initializes wrapper with console title
	void color(const int& c); // Sets color of font
	void reset(); // Resets color to its original state
	int getx(); // Gets x position of cursor on screen
};
#include <iostream>

#include "CLL.h"

int main()
{
	// Creating CLL variable
	cll::var x("\"Hello\"");

	// Outputing CLL variable
	std::cout << x.getString() << ' ';

	// Creating interpreter (using c++11 pointers)
	// It is recommended to create them on heap
	std::unique_ptr<cll::Interpreter> interpreter = std::make_unique<cll::Interpreter>();

	// Enabling CLL interpreter output
	interpreter->enableIO();

	// Executing line
	interpreter->readLine("cout \"World!\"");

	return 0;
}
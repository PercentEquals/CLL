#include <iostream>

// Author: Bartosz Niciak

#include "console.h"
#include "CLL.h"

// TODO in rework branch:
// - rework char to work as an int
// - rework lexer to detect character escaping
// - rework parser to work after math
// - repair parser and rework function checking
// - check addFunction problem

// This main file acts as a real-time console interpreter 
// and can be used to type code on the fly
// or to execute a file by passing its path by arguments with cmd or terminal

void errorLog(const std::unique_ptr<cll::Interpreter>& i)
{
	console::color(console::red);
	if (console::getx() != 0) std::cout << '\n';
	if (i->getFilename() != "") std::cout << "Error in file '" + i->getFilename() + "' on line " + std::to_string(i->getLine()) + ":" << '\n';
	std::cout << i->getError();
	console::color(console::white);
}

int main(int argc, char* argv[])
{
	// Initializes console with title name passed as argument, also a simple unit test
	console::init(cll::var("CLL Interpreter").getValue()); 

	// Interprets passed arguments as filepaths and executes them
	if (argc > 1)
	{
		// Passes parameters from terminal/cmd to interpreter
		cll::var params("[]");

		// Forces path to be represented as string (cmd ignores quotation marks)
		cll::var path = std::string(argv[1]);
		if (path.type != "STRING") path.setValue("\"" + path.getString() + "\"");
		
		params = params + cll::var(path.getValue());

		for (int i = 2; i < argc; ++i) params = params + cll::var(std::string(argv[i]));

		// Creates runtime interpreter 
		std::unique_ptr<cll::Interpreter> runtime = std::make_unique<cll::Interpreter>();
		runtime->enableDebug();
		runtime->disableLogging();
		
		if (!runtime->readLine("params = " + params.getValue())) errorLog(runtime);
		else if (!runtime->readFile(path.getString())) errorLog(runtime);
	}

	// If there are no passed arguments (except for program executable filepath) acts as a local real-time interpreter
	if (argc <= 1)
	{
		std::unique_ptr<cll::Interpreter> local = std::make_unique<cll::Interpreter>();
		local->enableDebug();
		local->disableLogging();

		std::string input = "";

		std::cout << cll::var("CLL Interpreter [0.2.0] - Bartosz Niciak");

		while (input.substr(0, 6) != "return")
		{
			if (console::getx() != 0) std::cout << '\n';
			std::cout << "> ";
			std::getline(std::cin, input);
			if (!local->readLine(input)) errorLog(local);
			local->clearError();
		}
	}

	console::reset();
	return 0;
}
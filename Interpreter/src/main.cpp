#include <iostream>
#include <memory>

// Author: Bartosz Niciak

#include "console.hpp"
#include "CLL.hpp"

// This main file acts as a real-time console interpreter 
// and can be used to type code on the fly
// or to execute a file by passing its path by arguments with cmd or terminal

#ifdef _WIN64
#define PLATFORM "x64"
#else
#define PLATFORM ""
#endif

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
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(NULL);

	// Initializes console with title name passed as argument, also a simple unit test
	console::init(cll::var("CLL Interpreter ").getValue() + PLATFORM); 

	// Interprets passed arguments as filepaths and executes them
	if (argc > 1)
	{
		// Passes parameters from terminal/cmd to interpreter
		cll::var params("[]");

		// Forces path to be represented as string (cmd ignores quotation marks)
		cll::var path = std::string(argv[1]);
		if (path.type != cll::Type::STRING) path.setValue("\"" + path.getString() + "\"");
		
		params += path;

		for (int i = 2; i < argc; ++i) params += std::string(argv[i]);

		// Creates runtime interpreter 
		std::unique_ptr<cll::Interpreter> runtime = std::make_unique<cll::Interpreter>();
		//runtime->enableDebug();
		runtime->enableIO();
		
		if (!runtime->readLine("argv = " + params.getValue())) errorLog(runtime);
		else if (!runtime->readFile(path.getString())) errorLog(runtime);
	}

	// If there are no passed arguments (except for program executable filepath) acts as a local real-time interpreter
	if (argc <= 1)
	{
		std::unique_ptr<cll::Interpreter> local = std::make_unique<cll::Interpreter>();
		//local->enableDebug();
		local->enableOutput();
		local->enableIO();

		std::string input("");

		std::cout << cll::var("CLL Interpreter [" + local->getVersion() + "] - Bartosz Niciak");

		do 
		{
			if (console::getx() != 0) std::cout << '\n';
			
			// Prompt
			if (local->getScope()) std::cout << "^ ";
			else std::cout << "> ";

			std::getline(std::cin, input);
			if (!local->readLine(input)) errorLog(local);
			else if (console::getx() == 0) std::cout << local->getOutput();

			local->clearError();
			local->clearOutput();
		} 
		while (local->getReturned().getValue() == "");
	}

	console::reset();
	return 0;
}
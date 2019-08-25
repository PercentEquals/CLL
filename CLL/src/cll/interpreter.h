#pragma once

// Author: Bartosz Niciak

#include "var.h"
#include "functions.h"

#include <string>
#include <vector>

namespace cll
{
	class Interpreter
	{
		std::vector<var> vars; // Interpreter variables
		std::string error; // Holds errors
		std::string filename; // Holds filename

		// FUNCTIONS
		Functions functions;

		// SCOPE SPECIFIC VARIABLES //
		std::vector<var> previous_scope_action; // Holds previous flow managed bare word (if, while, ...)
		std::vector<var> scope_action; // Holds actual flow managed bare word (if, while, ...)

		std::vector<std::string> scope_lines; // Holds actual scope - to be executed after closing bracket
		unsigned int scope; // Holds actual scope number

		// OTHER VARIABLES
		unsigned int line; // Actual line number of file - to show where error took place
		bool returned; // If true - closes program execution (when using 'return' command)
		bool log; // Determines wheter to output errors or not
		bool debug; // Determines wheter to output additional debug information about tokens
		bool enabledIO;
		bool flow; // Determines wheter this interpreter is an scope with flow (usefull to know wheter to return, continue or break)

		// PRIVATE METHODS //
		inline void write(const std::string& s) { if (enabledIO) std::cout << s; };
		bool errorLog(); // Returns false if there is an error and prints them with std::cout (if logging is enabled)
		bool newInterpreter(const std::vector<var>& v); // Creates new instance of interpreter - for file in file execution
		bool newScope(const std::vector<std::string>& l); // Creates new instance of interpreter - for scope execution
		bool parse(const std::vector<var>& v); // Checks line syntax
		bool bare(const std::vector<var>& v); // Procesess bare words and also some spiecial tokens
		std::vector<var> math(const std::vector<var>& v); // Procesess math equations

	public:

		// CONSTRUCTORS //
		Interpreter();
		Interpreter(const std::vector<var>& v);
		Interpreter(const std::string& f);

		// METHODS INTERPRETING THEIR PARAMETERS //
		bool readLine(const std::string& l); // Interpretes only one line
		bool readFile(const std::string& f); // Interpretes file by path

		// INTERPRETER VARIABLES ACCESSING METHODS //
		void setVar(const var& v); // Sets or adds variable to interpreter by var abstract
		inline void setVar(const std::string& n, const var& v) { setVar(var(n, v)); };
		inline void setVar(const std::string& n, const std::string& v) { setVar(var(n, v)); };
		inline void setVar(const std::string& n, const std::string& v, const std::string& t) { setVar(var(n, v, t)); };

		var getVar(const std::string& n); // Returns variable by its name
		void deleteVar(const std::string& n); // Deletes variable by its name

		// FUNCTIONS
		inline void addFunction(const function& f) { functions.add(f); };
		inline void addFunction(const std::string& n, var(*f)(const std::vector<var>&)) { addFunction(function(n, f)); };
		inline void deleteFunction(const std::string& n) { functions.del(n); };

		// METHODS THAT CHANGE BEHAVIOUR OF INTERPRETER //
		inline void enableLogging()  { log = true; };
		inline void disableLogging() { log = false; };
		inline void toggleLogging()  { log = !log; };

		inline void enableDebug()  { debug = true; };
		inline void disableDebug() { debug = false; };
		inline void toggleDebug()  { debug = !debug; };

		inline void enableIO()  { enabledIO = true; };
		inline void disableIO() { enabledIO = false; };
		inline void toggleIO()  { enabledIO = !enabledIO; };

		// OTHER PUBLIC METHODS //
		inline void clearError() { error = ""; }; // Clears error
		inline bool getReturned() { return returned; }; 
		inline unsigned int getLine() { return line; }; // Returns actual line number
		inline std::string getFilename() { return filename; }; // Returns non-empty string if interpreter interpretes a file
		inline std::string getError() { return error; }; // Returns non-empty string if some error is present (useful if error logging is disabled)
	};
}
#pragma once

// Author: Bartosz Niciak

#include "var.h"

#include <random>
#include <string>
#include <vector>
#include <chrono>

namespace cll
{
	class Interpreter
	{
		std::vector<var> vars; // Interpreter variables
		std::string error; // Holds errors
		std::string filename; // Holds filename

		// CLOCK //
		std::chrono::high_resolution_clock::time_point start; // Holds start point of program execution 

		// RANDOM //
		std::random_device rd;
		std::mt19937 random_engine;

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

		// PRIVATE METHODS //
		var function(const std::string& fun, const std::vector<var>& args);
		bool errorLog(); // Returns false if there is an error and prints them with std::cout (if logging is enabled)
		bool newInterpreter(const std::vector<var>& v); // Creates new instance of interpreter - for file in file execution
		bool newScope(const std::vector<std::string>& l); // Creates new instance of interpreter - for scope execution
		bool parse(const std::vector<var>& v); // Checks line syntax
		bool bare(const std::vector<var>& v); // Procesess bare words and also some spiecial tokens
		std::vector<var> math(const std::vector<var>& v); // Procesess math equations
		size_t searchVar(const std::string& name, const size_t& l, const size_t& r);

	public:

		// CONSTRUCTORS //
		Interpreter()
		{
			error = "";
			filename = "";
			scope = 0;
			line = 0;
			returned = false;
			log = true;
			debug = false;
			random_engine.seed(rd());

			// SETS KEYWORDS
			vars.emplace_back(var("and", "&&"));
			vars.emplace_back(var("not", "!"));
			vars.emplace_back(var("xor", "^"));
			vars.emplace_back(var("or", "||"));
			vars.emplace_back(var("is", "=="));
			vars.emplace_back(var("true", "1"));
			vars.emplace_back(var("false", "0"));
			std::sort(vars.begin(), vars.end(), [](var a, var b) { return a.name < b.name; });

			start = std::chrono::high_resolution_clock::now();
		};

		// Constructor with already declared variables
		Interpreter(const std::vector<var>& v)
		{
			Interpreter();
			vars = v;
		};

		// Constructor with file execution
		Interpreter(const std::string& f)
		{
			Interpreter();
			readFile(f);
		};

		// METHODS INTERPRETING THEIR PARAMETERS //
		bool readLine(const std::string& l); // Interpretes only one line
		bool readFile(const std::string& f); // Interpretes file by path

		// INTERPRETER VARIABLES ACCESSING METHODS //
		void setVar(const var& v); // Sets or adds variable to interpreter by var abstract
		void setVar(const std::string& n, const var& v) { setVar(var(n, v)); }; // Sets or adds variable to interpreter by var abstract and custom name
		void setVar(const std::string& n, const std::string& v) { setVar(var(n, v)); }; // Sets or adds variable to interpreter by 2 strings - name, value
		void setVar(const std::string& n, const std::string& v, const std::string& t) { setVar(var(n, v, t)); }; // Sets or adds variable to interpreter by 3 strings - name, forced value, forced type
		var getVar(const std::string& n); // Returns variable by its name
		void deleteVar(const std::string& n); // Deletes variable by its name

		// METHODS THAT CHANGE BEHAVIOUR OF INTERPRETER //
		void enableLogging() { log = true; };
		void disableLogging() { log = false; };
		void toggleLogging() { log = !log; };

		void enableDebug() { debug = true; };
		void disableDebug() { debug = false; };
		void toggleDebug() { debug = !debug; };

		// OTHER PUBLIC METHODS //
		void clearError() { error = ""; }; // Clears error
		unsigned int getLine() { return line; }; // Returns actual line number
		std::string getFilename() { return filename; }; // Returns non-empty string if interpreter interpretes a file
		std::string getError() { return error; }; // Returns non-empty string if some error is present (useful if error logging is disabled)
	};
}
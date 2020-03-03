#pragma once

// Author: Bartosz Niciak

#include "var.hpp"
#include "functions.hpp"
#include "defined.hpp"

#include <string>
#include <vector>

namespace cll
{
	class Interpreter
	{
		static const std::string version;

		std::vector<var> vars; // Interpreter variables
		std::string error; // Holds errors
		std::string filename; // Holds filename
		std::string output; // Holds output - usefull for terminal applications

		// FUNCTIONS
		Functions functions;
		Defined dfunctions;

		// SCOPE SPECIFIC VARIABLES //
		std::vector<var> previous_action; // Holds previous flow managed bare word (if, while, ...)
		std::vector<var> action; // Holds actual flow managed bare word (if, while, ...)
		std::vector<std::string> lines; // Holds actual scope - to be executed after closing bracket
		unsigned int scope; // Holds actual scope number

		// OTHER VARIABLES
		unsigned int line; // Actual line number of file - to show where error took place
		var returned;  // Hold execution state of 'return' statement 
		bool continued; // Hold execution state of 'continue' statement 
		bool broke; // Hold execution state of 'break' statement 
		bool log; // Determines whether to output errors or not
		bool debug; // Determines whether to output additional debug information about tokens
		bool enabledIO; // Determines whether to give ability to use 'cin' and 'cout' statements
		bool enabledOutput; // Determines whether to output additional info to "output" variable

		// PRIVATE METHODS //
		inline virtual void write(const std::string& s) { if (enabledIO) std::cout << s; };
		bool errorLog(); // Returns false if there is an error and prints them with std::cout (if logging is enabled)
		bool newInterpreter(const std::vector<var>& v); // Creates new instance of interpreter - for file in file execution
		var newFunction(const std::vector<var>& args, const std::vector<std::string>& l); // Function that creates new scope
		bool newScope(const std::vector<std::string>& l, const std::vector<var>& action = { var("1") }, const size_t& id = 0); // Creates new instance of interpreter - for scope execution
		bool parse(const std::vector<var>& v); // Checks line syntax
		bool bare(const std::vector<var>& v); // Procesess bare words and also some spiecial tokens
		bool readScope(const std::vector<var>& v);
		std::vector<var> math(const std::vector<var>& v, const bool& comma = true); // Procesess math equations
		bool afterparse(const std::vector<var>& v);

	public:

		// CONSTRUCTORS //
		Interpreter() : error(""), filename(""), output(""), scope(0), line(0), returned(""), continued(false), 
						broke(false), log(false), debug(false), enabledIO(false), enabledOutput(false)
		{
			vars.reserve(100);
			output.reserve(20);

			previous_action.reserve(15);
			action.reserve(15);
			lines.reserve(50);

			vars =
			{
				var("and", "&&"),
				var("endl", "'\\n'"),
				var("false", "0"),
				var("is", "=="),
				var("not", "!"),
				var("or", "||"),
				var("true", "1"),
				var("xor", "^")
			};
		};

		Interpreter(const std::vector<var>& v);
		Interpreter(const std::string& f);

		// METHODS INTERPRETING THEIR PARAMETERS //
		bool readVector(const std::vector<std::string>& v);
		bool readLine(const std::string& l); // Interpretes only one line
		bool readFile(const std::string& f); // Interpretes file by path

		// INTERPRETER VARIABLES ACCESSING METHODS //
		bool setVar(const var& v); // Sets or adds variable to interpreter by var abstract
		inline bool setVar(const std::string& n, const var& v) { return setVar(var(n, v)); };
		inline bool setVar(const std::string& n, const std::string& v) { return setVar(var(n, v)); };

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

		inline void enableOutput() { enabledOutput = true; };
		inline void disableOutput() { enabledOutput = false; };
		inline void toggleOutput() { enabledOutput = !enabledOutput; };

		// OTHER PUBLIC METHODS //
		inline void clearError() { error.clear(); };
		inline void clearOutput() { output.clear(); };
		inline var getReturned() const { return returned; }; 
		inline unsigned int getScope() const { return scope; };
		inline unsigned int getLine() const { return line; }; // Returns actual line number
		inline std::string getError() const { return error; }; // Returns non-empty string if some error is present (useful if error logging is disabled)
		inline std::string getFilename() const { return filename; }; // Returns non-empty string if interpreter interpretes a file
		inline std::string getVersion() const { return version; };
		inline std::string getOutput() const { return output; };
	};
}
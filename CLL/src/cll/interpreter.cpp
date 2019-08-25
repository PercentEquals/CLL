#include "interpreter.h"

// Author: Bartosz Niciak

#include "utils/search.h"
#include "lexer.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <thread>

namespace cll
{
	// CONSTRUCTORS //

	Interpreter::Interpreter()
	{
		error = "";
		filename = "";
		scope = 0;
		line = 0;
		returned = false;
		log = false;
		debug = false;
		enabledIO = false;

		// SETS KEYWORDS
		vars.emplace_back(var("and", "&&"));
		vars.emplace_back(var("not", "!"));
		vars.emplace_back(var("xor", "^"));
		vars.emplace_back(var("or", "||"));
		vars.emplace_back(var("is", "=="));
		vars.emplace_back(var("true", "1"));
		vars.emplace_back(var("false", "0"));
		vars.emplace_back(var("endl", "'\n'"));
		std::sort(vars.begin(), vars.end(), [](var a, var b) { return a.name < b.name; });
	}

	// Constructor with already declared variables
	Interpreter::Interpreter(const std::vector<var>& v)
	{
		Interpreter();
		vars = v;
	}

	// Constructor with file execution
	Interpreter::Interpreter(const std::string& f)
	{
		Interpreter();
		readFile(f);
	}

	// Function that checks for errors and logs them on console screen
	bool Interpreter::errorLog()
	{
		if (error == "") return true;

		scope_lines.clear();
		scope_action.clear();

		if (log)
		{
			write("\nERROR: ");
			if (filename != "") write("Error in file '" + filename + "' on line " + std::to_string(line) + ":\n");
			write(error);
		}

		return false;
	}

	// Function that creates new interpreter (i.e. interpretes another file)
	bool Interpreter::newInterpreter(const std::vector<var>& v)
	{
		std::string params = "[";
		for (size_t i = 1; i < v.size(); ++i) params += v[i].value + ',';
		params.pop_back();
		params += "]";

		std::unique_ptr<Interpreter> nested = std::make_unique<Interpreter>();
		nested->log = log;
		nested->debug = debug;
		nested->setVar("params", params);

		bool state = nested->readFile(v[1].getString());
		if (!state) error = nested->error;
		return state;
	}

	// Function that creates new scope that haves its own variables and also variables from one scope higher
	bool Interpreter::newScope(const std::vector<std::string>& l)
	{
		std::unique_ptr<Interpreter> nested = std::make_unique<Interpreter>(vars);
		nested->log = log;
		nested->debug = debug;
		nested->filename = filename;
		nested->enabledIO = enabledIO;
		nested->line = (line >= l.size()) ? (line - l.size()) : 0;

		for (size_t i = 0; i < l.size(); ++i)
		{
			if (!nested->readLine(l[i]))
			{
				error = nested->error;
				return false;
			}

			if (filename != "") nested->line++;
		}
	
		// SETTING PREVIOUS (THIS) SCOPE VARIABLES
		for (size_t i = 0; i < nested->vars.size(); ++i)
		{
			var buff = this->getVar(nested->vars[i].name);

			if (buff.type != "UNDEFINED") this->setVar(nested->vars[i]);
		}

		return true;
	}

	// Function that checks line syntax
	bool Interpreter::parse(const std::vector<var>& v)
	{
		if (v.empty()) return true;

		if (v[0].type == "SYMBOL" && v[0].value == "}" && !scope) error = "Unexpected symbol '}' - nothing to close!";
		if (error != "") return false;

		if (v[0].type == "BARE")
		{
			// CHECKS FOR MULTIPLE BARE WORDS
			for (size_t i = 0; i < v.size(); ++i)
			{
				if (v[i].type == "BARE" && i > 0)
				{
					if (v[0].value == "cout") continue;
					if (i == 1 && v[0].value == "else" && v[1].value == "if") continue;

					error = "Unexpected '" + v[i].value + "' after '" + v[0].value + "' statement!";
					return false;
				}
			}

			// CHECKS FOR BARE WORD UNIQUE SYNTAX
			if (v[0].value == "cout" && v.size() < 2) error = "Command 'cout' got too few arguments!";
			else if (v[0].value == "delete")
			{
				if (v.size() < 2) error = "Command 'delete' got too few arguments!";

				for (size_t i = 1; i < v.size(); ++i)
				{
					if (v[i].type == "SYMBOL" && v[i].value != ",") error = "Unexpected symbol '" + v[i].value + "' after 'delete' statement!";
					else if (getVar(v[i].value).type == "UNDEFINED") error = "Undefined name '" + v[i].value + "' after 'delete' statement!";

					if (error != "") break;
				}
			}
			else if (v[0].value == "cll" && v.size() < 2) error = "Command 'cll' got too few arguments!";
			else if ((v[0].value == "if" || v[0].value == "while") && v.size() < 2) error = "Command '" + v[0].value + "' got too few arguments!";
			else if (v[0].value == "else")
			{
				if (previous_scope_action.empty()) error = "Unexpected command 'else'!";
				else if (previous_scope_action[0].value != "if" && previous_scope_action[0].value != "else") error = "Unexpected command 'else'!";
				else if (previous_scope_action[0].value == "else" && previous_scope_action.size() == 1) error = "Unexpected command 'else'!";

				if (v.size() > 1)
				{
					if (v[1].value != "if") error = "Unexpected name '" + v[1].value + "' after 'else' statement!";
					else if (v[1].value == "if" && v.size() < 3) error = "Command 'else if' got too few arguments!";
				}
			}
		}

		if (error != "") return false;

		// CHECKS FOR UNDEFINED NAMES
		for (size_t i = 0; i < v.size(); ++i)
		{
			if (i == 0 && v[i].type == "BARE") continue;
			if (i == 1 && v[0].type == "BARE" && v[0].value == "else" && v[1].value == "if") continue;

			if (i > 0 && v[i].type == "SYMBOL" && v[i].value != "-" && v[i].value != "!")
			{
				// TERNARY CHECK
				if (v[i].value == "?")
				{
					if (i + 2 < v.size() && v[i + 2].type != "SYMBOL" && v[i + 2].value != ":")
					{
						error = "Expected symbol ':' after a ternary operator!"; break;
					}
					else if (i + 2 >= v.size())
					{
						error = "Ternary operator got too few arguments!"; break;
					}
				}

				if (v[i - 1].type == "BARE")
				{
					error = "Unexpected symbol '" + v[i].value + "' after '" + v[i - 1].value + "' command!"; break;
				}

				if (v[i - 1].type == "SYMBOL")
				{
					error = "Unexpected symbol '" + v[i].value + "' after '" + v[i - 1].value + "' symbol!"; break;
				}

				if (v[i].value != ";" && v[i].value != "{" && v[i].value != "}" && i + 1 >= v.size())
				{
					error = "Expected something after '" + v[i].value + "' symbol!"; break;
				}
			}

			if (v[i].type == "ARRAY" || v[i].type == "PARENTHESIS")
			{
				std::vector<var> buff;

				buff = lexer(v[i].value.substr(1, v[i].value.length() - 2));

				for (size_t ii = 0; ii < buff.size(); ++ii)
				{
					if (buff[ii].type == "BARE" || (buff[ii].type == "SYMBOL" && (buff[ii].value == "{" || buff[ii].value == "}")))
					{
						error = (buff[ii].type == "BARE") ? "Unexpected name '" : "Unexpected symbol '";
						error += buff[ii].value + "' in ";
						error += (v[i].type == "ARRAY") ? "array!" : "parenthesis!";
						break;
					}
				}

				if (error != "") return false;
				if (!parse(buff)) return false;

				return true;
			}
			else if (v[i].type == "UNDEFINED")
			{
				if (i + 1 < v.size() && v[i].value.find_first_of("[]") == std::string::npos)
				{
					if (v[i + 1].type == "SYMBOL" && v[i + 1].value == "=") continue;
				}

				if (v[i].value.find("(") == std::string::npos && getVar(v[i].value).type == "UNDEFINED")
				{
					error = "Name '" + v[i].value + "' not recognized!"; break;
				}
			}
			else if (i + 1 < v.size() && v[i + 1].type == "SYMBOL" && v[i + 1].value == "=")
			{
				error = "Unexpected symbol '=' after '" + v[i].value + "'!"; break;
			}
		}

		if (error != "") return false;
		return true;
	}

	bool Interpreter::bare(const std::vector<var>& v)
	{
		if (v[0].type == "BARE")
		{
			if (v[0].value == "return")
			{
				returned = true;
				return false;
			}
			else if (v[0].value == "pause")
			{
				std::string buff;
				std::getline(std::cin, buff);
			}
			else if (v[0].value == "cout")
			{
				for (size_t i = 1; i < v.size(); ++i)
				{
					if (v[i].type == "CHAR") write(std::string(1, char(v[i].getInt())));
					else write(v[i].getString());
				}
			}
			else if (enabledIO && v[0].value == "cin")
			{
				for (size_t i = 1; i < v.size(); ++i)
				{
					std::string buff;
					std::getline(std::cin, buff);
					var test(buff);
					if (test.type == "UNDEFINED" || test.type == "BARE" || test.type == "FUNCTION") buff = "\"" + buff + "\"";
					setVar(v[i].name, buff);
				}
			}
			else if (v[0].value == "if" || v[0].value == "while" || v[0].value == "else")
			{
				for(size_t i = 0; i < v.size(); ++i) scope_action.emplace_back(v[i]);
			}
			else if (v[0].value == "delete")
			{
				for (size_t i = 1; i < v.size(); ++i) if (v[i].type != "SYMBOL") deleteVar(v[i].name);
			}
			else if (v[0].value == "cll") return newInterpreter(v);
		}
		else if (v[0].value == "{" && v[0].type == "SYMBOL") scope = 1;
		else if (v.size() == 1 && v[0].type != "UNDEFINED") write(v[0].value + " " + v[0].type);

		if (error != "") return false;
		return true;
	}

	std::vector<var> Interpreter::math(const std::vector<var>& v)
	{
		std::vector<var> vec;

		for (size_t i = 0; i < v.size(); ++i)
		{
			if (v[i].type == "PARENTHESIS")
			{
				std::vector<var> buff = math(lexer(v[i].value.substr(1, v[i].value.length() - 2)));
				vec.insert(std::end(vec), std::begin(buff), std::end(buff));
			}
			else if (v[i].type == "ARRAY")
			{
				std::vector<var> buff = math(lexer(v[i].value.substr(1, v[i].value.length() - 2)));
				std::string arr = "[";
				for (size_t i = 0; i < buff.size(); ++i) arr += buff[i].value;
				arr += "]";
				vec.emplace_back(arr);
			}
			else if(v[i].type == "UNDEFINED")
			{
				if (v[i].value.find("(") != std::string::npos && v[i].value[v[i].value.length() - 1] == ')')
				{
					std::string fun = v[i].value.substr(0, v[i].value.find("("));
					std::vector<var> args = math(lexer(v[i].value.substr(fun.length() + 1, v[i].value.length() - fun.length() - 2)));
					function buff = functions.get(fun);

					if (buff.name != "" && parse({ var(v[i].value.substr(fun.length(), v[i].value.length() - fun.length())) }))
						vec.emplace_back(buff.fun(args));

					else vec.emplace_back(v[i]);
				}
				else
				{
					var buff = getVar(v[i].value);
					if (buff.type != "UNDEFINED") vec.emplace_back(buff);
					else vec.emplace_back(v[i]);
				}
			}
			else vec.emplace_back(v[i]);
		}

		// MATH WITH OPERATOR PRECEDENCE
		bool assignment = false;

		for (unsigned char step = 0; step < 13; ++step)
		{
			for (size_t i = 0; i < vec.size(); ++i)
			{
				if (i > 0 && step == 0)
				{
					// PREFIX OPERATORS

					if (vec[i - 1].type == "SYMBOL")
					{
						if (vec[i - 1].value == "!" || vec[i - 1].value == "~" || vec[i - 1].value == "-")
						{
							if (vec[i].type == "SYMBOL" || vec[i].type == "UNDEFINED") continue;

							var ins;

							if (vec[i - 1].value == "!") ins = !vec[i];
							else if (vec[i - 1].value == "~") ins = ~vec[i];
							else if (vec[i - 1].value == "-")
							{
								if (i > 1 && vec[i - 2].type != "SYMBOL") continue;
								ins = var("0") - vec[i];
							}

							vec.erase(vec.begin() + (i - 1), vec.begin() + i + 1);
							vec.insert(vec.begin() + (i - 1), ins);
							i -= 2;
						}
					}
				}
				else if (i > 1 && step < 11)
				{
					if (vec[i - 1].type == "SYMBOL")
					{
						if (vec[i - 2].type == "SYMBOL" || vec[i - 2].type == "UNDEFINED") continue;
						if (vec[i].type == "SYMBOL" || vec[i].type == "UNDEFINED") continue;
						if (vec[i - 2].type == "SYMBOL" && vec[i - 1].value != "-") continue;
						var ins;

						if (step == 1 && vec[i - 1].value == "**") ins = vec[i - 2].pow(vec[i]);
						else if (step == 2)
						{
							if (vec[i - 1].value == "*") ins = vec[i - 2] * vec[i];
							else if (vec[i - 1].value == "/") ins = vec[i - 2] / vec[i];
							else if (vec[i - 1].value == "%") ins = vec[i - 2] % vec[i];
						}
						else if (step == 3)
						{
							if (vec[i - 1].value == "+") ins = vec[i - 2] + vec[i];
							else if (vec[i - 1].value == "-") ins = vec[i - 2] - vec[i];
						}
						else if (step == 4)
						{
							if (vec[i - 1].value == "<<") ins = vec[i - 2] << vec[i];
							else if (vec[i - 1].value == ">>") ins = vec[i - 2] >> vec[i];
						}
						else if (step == 5)
						{
							if (vec[i - 1].value == "<=") ins = vec[i - 2] <= vec[i];
							else if (vec[i - 1].value == ">=") ins = vec[i - 2] >= vec[i];
							else if (vec[i - 1].value == "<") ins = vec[i - 2] < vec[i];
							else if (vec[i - 1].value == ">") ins = vec[i - 2] > vec[i];
						}
						else if (step == 6)
						{
							if (vec[i - 1].value == "==") ins = vec[i - 2] == vec[i];
							else if (vec[i - 1].value == "!=") ins = vec[i - 2] != vec[i];
							else if (vec[i - 1].value == "===") ins = var(std::to_string((vec[i - 2] == vec[i]).getBool() && vec[i - 2].type == vec[i].type));
							else if (vec[i - 1].value == "!==") ins = var(std::to_string((vec[i - 2] != vec[i]).getBool() || vec[i - 2].type != vec[i].type));
						}
						else if (step == 7 && vec[i - 1].value == "&") ins = vec[i - 2] & vec[i];
						else if (step == 8 && vec[i - 1].value == "^") ins = vec[i - 2] ^ vec[i];
						else if (step == 9 && vec[i - 1].value == "|") ins = vec[i - 2] | vec[i];
						else if (step == 10)
						{
							if (vec[i - 1].value == "&&") ins = vec[i - 2] && vec[i];
							else if (vec[i - 1].value == "||") ins = vec[i - 2] || vec[i];
						}

						if (ins.value != "")
						{
							vec.erase(vec.begin() + i - 2, vec.begin() + i + 1);
							vec.insert(vec.begin() + (i - 2), ins);
							i -= 2;
						}
					}
				}
				else if (i > 3 && step == 11)
				{
					// TERNARY OPERATOR

					if (vec[i - 4].type == "SYMBOL" || vec[i - 4].type == "UNDEFINED") continue;
					if (vec[i].type == "SYMBOL" || vec[i].type == "UNDEFINED") continue;

					if (vec[i - 1].type == "SYMBOL" && vec[i - 1].value == ":")
					{
						if (vec[i - 3].type == "SYMBOL" && vec[i - 3].value == "?")
						{
							var ins;

							if (vec[i - 4].getBool()) ins = vec[i - 2];
							else ins = vec[i];

							vec.erase(vec.begin() + (i - 4), vec.begin() + i + 1);
							vec.insert(vec.begin() + (i - 4), ins);
							i -= 4;
						}
					}
				}
				else if (i > 1 && step == 12)
				{
					// ASSIGNMENT OPERATORS

					var lvar = vec[(vec.size() - 1) - i + 2];
					var symb = vec[(vec.size() - 1) - i + 1];
					var fvar = vec[(vec.size() - 1) - i];
					var ins;

					if (lvar.type == "SYMBOL" || lvar.type == "BARE") continue;
					if (fvar.type == "SYMBOL" || fvar.type == "BARE") continue;

					if (lvar.name != "") lvar = getVar(lvar.name);
					if (fvar.name != "") fvar = getVar(fvar.name);

					if (lvar.type == "UNDEFINED") continue;

					if (symb.value == "=") ins = lvar.value;
					else if (symb.value == "+=") ins = fvar + lvar;
					else if (symb.value == "-=") ins = fvar - lvar;
					else if (symb.value == "/=") ins = fvar / lvar;
					else if (symb.value == "*=") ins = fvar * lvar;
					else if (symb.value == "%=") ins = fvar % lvar;
					else if (symb.value == "&=") ins = fvar & lvar;
					else if (symb.value == "|=") ins = fvar | lvar;
					else if (symb.value == "^=") ins = fvar ^ lvar;
					else if (symb.value == "<<=") ins = fvar << lvar;
					else if (symb.value == ">>=") ins = fvar >> lvar;
					else continue;

					ins.setName((fvar.name == "") ? fvar.value : fvar.name);
					
					setVar(ins);

					assignment = true;

					vec.erase(vec.end() - i - 1, vec.end() - i + 2);
					vec.insert(vec.end() - (i - 2), ins);
					i -= 2;
				}
			}
		}

		if (assignment && vec.size() > 1) vec = math(vec);

		return vec;
	}

	// Function that interpretes one line
	bool Interpreter::readLine(const std::string& l)
	{
		// SKIPS EMPTY LINES
		if (l == "")  return true;
		if (l == ";") return true;

		// SEPARATES LINE BY ARGUMENTS
		std::vector<var> args_line = lexer(l); // Holds raw line tokens

		// CHECKS FOR LINE BREAK (SEMICOLON) AND FOR BRACKETS
		std::vector<var> args;
		std::string newline = "";
		bool multiline = false;
			
		for (size_t i = 0; i < args_line.size(); ++i)
		{	
			if (!multiline && (args_line[i].value == ";" || args_line[i].value == "{" || args_line[i].value == "}") && args_line[i].type == "SYMBOL")
			{
				multiline = true;

				if(args_line[i].value == ";") continue;
				else if (i == 0)
				{
					args.emplace_back(args_line[i]);
					continue;
				}
			}
			if (multiline) newline += args_line[i].value + " ";
			else args.emplace_back(args_line[i]);
		}

		if (args.empty())
		{
			if (!args_line.empty() && !readLine(newline)) return errorLog();
			return true;
		}

		// SCOPING - i.e. CODE IN BRACKETS
		if (scope)
		{
			if (args[0].value == "{" && args[0].type == "SYMBOL") scope++;
			if (args[0].value == "}" && args[0].type == "SYMBOL") scope--;

			if (scope)
			{
				std::string buff = "";
				for (size_t i = 0; i < args.size(); ++i) buff += args[i].value + " ";
				scope_lines.emplace_back(buff);
				if (newline != "" && !readLine(newline)) return errorLog();
				return true;
			}
			else
			{
				bool state = true;

				if (scope_action.empty()) state = newScope(scope_lines);
				else if (scope_action[0].value == "if" && scope_action[1].getBool()) state = newScope(scope_lines);
				else if (scope_action[0].value == "while")
				{
					while (math(scope_action)[1].getBool()) state = newScope(scope_lines);
				}
				else if (scope_action[0].value == "else")
				{
					if (scope_action.size() <= 1 && !previous_scope_action[previous_scope_action.size() - 1].getBool()) state = newScope(scope_lines);
					else if (!previous_scope_action[previous_scope_action.size() - 1].getBool() && scope_action[2].getBool()) state = newScope(scope_lines);
				}

				previous_scope_action = scope_action;
				scope_action.clear();
				scope_lines.clear();
				if (newline != "" && !readLine(newline)) return errorLog();
				return (!state) ? errorLog() : true;
			}
		} 

		// PRINTS DEBUG MODE ADDITIONAL INFORMATION (before math)
		if (debug)
		{
			write("\nDEBUG: ");

			for (size_t i = 0; i < args.size(); ++i)
			{
				write(args[i].value + " " + args[i].type);
				if (i != args.size() - 1) write(" | ");
			}

			if (args.size() != 0) write("\n");
		}

		// PARSER
		if (!parse(args)) return errorLog();

		// APPLIES MATH TO TOKENS
		if (args[0].value != "while" && args[0].type != "BARE") args = math(args);
		if (error != "") return errorLog();
		if (args.empty()) return true;

		// PRINTS DEBUG MODE ADDITIONAL INFORMATION (after math)
		if (debug)
		{
			write("DEBUG: ");

			for (size_t i = 0; i < args.size(); ++i)
			{
				write(args[i].value + " " + args[i].type);
				if (i != args.size() - 1) write(" | ");
			}

			if (args.size() != 0) write("\n");
		}

		if(args[0].value != "while" && args[0].type != "BARE")
		for (size_t i = 0; i < args.size(); ++i)
		{
			if (args[i].type == "UNDEFINED")
			{
				error = "Name '" + args[i].value + "' not recognized!";
				return errorLog();
			}
		}

		// INTERPRETS ARGUMENTS
		if (!bare(args)) return errorLog();


		if (newline != "" && !readLine(newline)) return errorLog();
		return true;
	}

	// Function that interpretes whole file line by line
	bool Interpreter::readFile(const std::string& f)
	{
		filename = f;

		std::fstream file(filename, std::ios::in);
		std::string l = "";

		if (file.good())
		{
			while (std::getline(file, l))
			{
				line++;
				if (!readLine(l))
				{
					file.close();

					if (returned) return true;
					else return false;
				}
			}
		}
		else error = "File '" + f + "' could not be opened!";

		line = 0;
		filename = "";
		file.close();

		return errorLog();
	}

	// Function that returns declared var by it's name - or 'undefined' if var is not declared
	var Interpreter::getVar(const std::string& n)
	{
		if (n.find_first_of("[]") != std::string::npos && n.length() > 1)
		{
			if (n[n.length() - 1] == ']')
			{
				bool literal = false;
				std::string name = n.substr(0, n.rfind("["));
				if (name == "" || name == "()" || name == "[]") return var(n, "UNDEFINED");

				std::string buff = n.substr(name.length());
				std::string raw = "";

				for (size_t i = 0; i < buff.length(); ++i)
				{
					if (buff[i] != '[' && buff[i] != ']') raw += buff[i];
					if (buff[i] == ']') raw += ' ';
				}

				std::vector<var> elem = math(lexer(raw));
				if (elem.empty()) return var(n, "UNDEFINED");

				var ret = getVar(name);

				if (ret.type == "UNDEFINED")
				{
					var test(name);

					if (test.type != "BARE" && test.type != "UNDEFINED")
					{
						ret = math(lexer(name))[0].getElement((size_t)elem[0].getInt()); // String literals, arrays and parenthesis
						literal = true;
					}
				}
				else ret = ret.getElement((size_t)elem[0].getInt());

				if (ret.type == "CHAR" && ret.getInt() == 0) return var(n, "UNDEFINED");
				else if (ret.value == "") return var(n, "UNDEFINED");
				else if (!literal)
				{
					ret.name = name;
					for (size_t i = 0; i < elem.size(); ++i) ret.name += "[" + elem[i].getString() + "]";
				}

				return ret;
			}
		}

		size_t index = search(vars, n, 0, vars.size() - 1);
		if (index < vars.size()) return vars[index];
		else return var(n, "UNDEFINED");
	}

	// Function that changes defined var value or creates new var if one does not exist
	void Interpreter::setVar(const var& v)
	{
		var ins = v;

		if (v.name.find_first_of("[]") != std::string::npos && v.name.length() > 1)
		{
			if (v.name[v.name.length() - 1] == ']')
			{
				std::string name = v.name.substr(0, v.name.rfind("["));
				if (name == "" || name == "()" || name == "[]") return;

				std::string buff = v.name.substr(name.length());
				std::string raw = "";

				for (size_t i = 0; i < buff.length(); ++i)
				{
					if (buff[i] != '[' && buff[i] != ']') raw += buff[i];
					if (buff[i] == ']') raw += ' ';
				}

				std::vector<var> elem = math(lexer(raw));
				if (elem.empty()) return;

				var ret = getVar(name);

				if (ret.type == "UNDEFINED") return;
				ret.setElement((size_t)elem[0].getInt(), v.value);

				setVar(ret);

				return;
			}
		}

		if (ins.getError() != "" || ins.name == "") return;

		size_t index = search(vars, ins.name, 0, vars.size() - 1);
		if (index < vars.size()) vars[index] = ins;
		else if (ins.name != "") vars.emplace_back(v);
		std::sort(vars.begin(), vars.end(), [](var a, var b) { return a.name < b.name; });
	}

	void Interpreter::deleteVar(const std::string& n)
	{
		if (n.find_first_of("[]") != std::string::npos && n.length() > 1)
		{
			if (n[n.length() - 1] == ']') setVar(n, "");

			var buff = getVar(n.substr(0, n.find("[")));
			if (buff.getSize() == 0 && buff.type != "STRING" && buff.type != "ARRAY") deleteVar(n.substr(0, n.find("[")));

			return;
		}

		size_t index = search(vars, n, 0, vars.size() - 1);
		if (index < vars.size()) vars.erase(vars.begin() + index);
	}
}
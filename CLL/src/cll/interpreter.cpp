#include "interpreter.h"

// Author: Bartosz Niciak

#include "utils/search.h"
#include "lexer.h"

#include <conio.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <thread>

namespace cll
{
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

	size_t Interpreter::getSubscript(const var& v)
	{
		size_t nests = 0, ii = v.value.length() - 1;
		bool string = false, chars = false;
		for (ii; ii != 0; --ii)
		{
			if (!chars && string && v.value[ii] == '"')
			{
				if (ii != 0 && v.value[ii - 1] != '\\') string = false;
			}
			else if (!chars && !string && v.value[ii] == '"') string = true;

			if (!string && chars && v.value[ii] == '\'')
			{
				if (ii != 0 && v.value[ii - 1] != '\\') chars = false;
			}
			else if (!string && !chars && v.value[ii] == '\'') chars = true;

			if (string || chars) continue;

			if (v.value[ii] == ']') nests++;
			if (v.value[ii] == '[') nests--;
			if (nests == 0) break;
		}

		return ii;
	}

	// Function that checks for errors and logs them on console screen
	bool Interpreter::errorLog()
	{
		if (error == "") return true;

		lines.clear();
		action.clear();

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
		var params("[]");
		for (size_t i = 1; i < v.size(); ++i) params += v[i];

		std::unique_ptr<Interpreter> nested = std::make_unique<Interpreter>();
		nested->log = log;
		nested->debug = debug;
		nested->enabledIO = enabledIO;
		nested->setVar("params", params);

		bool state = nested->readFile(v[1].getString());
		if (!state)
		{
			if (nested->line != 0) error = "Error in file '" + nested->filename + "' on line " + std::to_string(nested->line) + ":\n";
			error += nested->error;
		}
		return state;
	}

	var Interpreter::newFunction(const std::vector<var>& args, const std::vector<std::string>& l)
	{
		var params("[]");
		for (size_t i = 0; i < args.size(); ++i)
		{
			if (args[i].getValue() != ",") params += args[i];
		}

		std::unique_ptr<Interpreter> nested = std::make_unique<Interpreter>();
		nested->log = log;
		nested->debug = debug;
		nested->enabledIO = enabledIO;
		nested->functions = functions;
		nested->dfunctions = dfunctions;
		nested->setVar("params", params);

		for (size_t i = 0; i < l.size(); ++i)
		{
			if (!nested->readLine(l[i]))
			{
				error = nested->error;
				return var("0");
			}

			if (nested->returned.value != "") return nested->returned;
		}

		return var("");
	}

	// Function that creates new scope that haves its own variables and also variables from one scope higher
	bool Interpreter::newScope(const std::vector<std::string>& l)
	{
		std::unique_ptr<Interpreter> nested = std::make_unique<Interpreter>(vars);
		nested->log = log;
		nested->debug = debug;
		nested->filename = filename;
		nested->enabledIO = enabledIO;
		nested->functions = functions;
		nested->dfunctions = dfunctions;
		nested->line = (line >= (unsigned int)l.size()) ? (line - (unsigned int)l.size()) : 0;

		for (size_t i = 0; i < l.size(); ++i)
		{
			nested->continued = false;

			if (!nested->readLine(l[i]))
			{
				error = nested->error;
				return false;
			}

			if (filename != "") nested->line++;
			if (nested->continued || nested->broke) break;
		}
	
		continued = nested->continued;
		broke = nested->broke;

		// SETTING PREVIOUS (THIS) SCOPE VARIABLES
		for (size_t i = 0; i < nested->vars.size(); ++i)
		{
			var buff = this->getVar(nested->vars[i].name);
			if (buff.type != Type::UNDEFINED) this->setVar(nested->vars[i]);
		}

		returned = nested->returned;
		if (returned.value != "") return false;
		return true;
	}

	// Function that checks line syntax
	bool Interpreter::parse(const std::vector<var>& v)
	{
		if (v.empty()) return true;

		if (v[0].type == Type::SYMBOL && v[0].value == "}" && !scope) error = "Unexpected symbol '}' - nothing to close!";
		if (v[0].type == Type::SYMBOL && v[0].value != "{" && v.size() == 1) error = "Unexpected symbol '" + v[0].value + "'!";
		if (v[0].type == Type::SYMBOL && v[0].value != "-" && v[0].value != "~" && v.size() > 1) error = "Unexpected symbol '" + v[0].value + "'!";
		if (error != "") return false;

		// CHECKS FOR MULTIPLE BARE WORDS
		for (size_t i = 0; i < v.size(); ++i)
		{
			if (v[i].type == Type::BARE && i > 0)
			{
				if (v[0].value == "cout") continue;
				if (i == 1 && v[0].value == "else" && v[1].value == "if") continue;
				if (i == 1 && v[0].value == "do" && v[1].value == "while") continue;

				error = "Unexpected '" + v[i].value + "' after '" + v[0].value + "' statement!";
				return false;
			}
		}

		if (error != "") return false;

		if (v[0].type == Type::BARE)
		{
			// CHECKS FOR BARE WORD UNIQUE SYNTAX
			if (v[0].value == "cout" && v.size() < 2) error = "Statement 'cout' got too few arguments!";
			else if (v[0].value == "cin" && v.size() < 2) error = "Statement 'cin' got too few arguments!";
			else if (v[0].value == "delete")
			{
				if (v.size() < 2) error = "Statement 'delete' got too few arguments!";

				for (size_t i = 1; i < v.size(); ++i)
				{
					if (v[i].type == Type::SYMBOL && v[i].value != ",") error = "Unexpected symbol '" + v[i].value + "' after 'delete' statement!";
					else if (getVar(v[i].value).type == Type::UNDEFINED) error = "Undefined name '" + v[i].value + "' after 'delete' statement!";

					if (error != "") break;
				}
			}
			else if (v[0].value == "continue" && v.size() > 1) error = "Statement 'continue' got too much arguments!";
			else if (v[0].value == "break" && v.size() > 1) error = "Statement 'break' got too much arguments!";
			else if (v[0].value == "cll" && v.size() < 2) error = "Statement 'cll' got too few arguments!";
			else if (v[0].value == "include" && v.size() < 2) error = "Statement 'include' got too few arguments!";
			else if ((v[0].value == "if" || v[0].value == "while") && v.size() < 2) error = "Statement '" + v[0].value + "' got too few arguments!";
			else if (v[0].value == "do" && v.size() < 3) error = "Statement 'do while' got too few arguments!";
			else if (v[0].value == "for")
			{
				unsigned char commas = 0;
				for (size_t i = 1; i < v.size(); ++i) if (v[i].type == Type::SYMBOL && v[i].value == ",") commas++;

				if (commas < 2) error = "Statement 'for' got too few arguments!";
				else if (commas > 2) error = "Statement 'for' got too many arguments!";
			}
			else if (v[0].value == "function")
			{
				if (v.size() < 2) error = "Statement 'function' got too few arguments!";
				else if (v.size() > 2) error = "Statement 'function' got too many arguments!";
				else if (v[1].type != Type::UNDEFINED) error = "Illegal name '" + v[1].value + "' after 'function' statement!";
				else if (var(v[1].value, "").getError() != "") error = "Illegal name '" + v[1].value + "' after 'function' statement!";
			}
			else if (v[0].value == "else")
			{
				if (previous_action.empty()) error = "Unexpected statement 'else'!";
				else if (previous_action[0].value != "if" && previous_action[0].value != "else") error = "Unexpected statement 'else'!";
				else if (previous_action[0].value == "else" && previous_action.size() == 1) error = "Unexpected statement 'else'!";

				if (v.size() > 1)
				{
					if (v[1].value != "if") error = "Unexpected name '" + v[1].value + "' after 'else' statement!";
					else if (v[1].value == "if" && v.size() < 3) error = "Statement 'else if' got too few arguments!";
				}
			}
		}

		if (error != "") return false;

		std::vector<std::string> defined; // Holds variables defined in actual line

		for (size_t i = 0; i < v.size(); ++i)
		{
			if (i == 0 && v[i].type == Type::BARE) continue;
			if (i == 1 && v[0].type == Type::BARE && v[0].value == "else" && v[1].value == "if") continue;

			if (i > 0 && v[i].type == Type::SYMBOL)
			{
				// TERNARY CHECK
				if (v[i].value == "?")
				{
					bool tererror = true;
					for (size_t ii = 0; ii < v.size(); ++ii)
					{
						if (v[ii].type == Type::SYMBOL && v[ii].value == ":")
						{
							tererror = false;
							break;
						}
					}

					if (tererror)
					{
						error = "Expected symbol ':' after a ternary operator!"; break;
					}

					if (i + 2 >= v.size())
					{
						error = "Ternary operator got too few arguments!"; break;
					}
				}

				if (v[i - 1].type == Type::BARE && v[i].value != "-" && v[i].value != "!" && v[i].value != "~")
				{
					error = "Unexpected symbol '" + v[i].value + "' after '" + v[i - 1].value + "' statement!"; break;
				}

				if (v[i - 1].type == Type::SYMBOL && v[i].value != "-" && v[i].value != "!" && v[i].value != "~")
				{
					error = "Unexpected symbol '" + v[i].value + "' after '" + v[i - 1].value + "' symbol!"; break;
				}

				if (v[i].value != ";" && v[i].value != "{" && v[i].value != "}" && i + 1 >= v.size())
				{
					error = "Expected something after '" + v[i].value + "' symbol!"; break;
				}
			}

			if (error != "") return false;

			if (v[i].type == Type::ARRAY || v[i].type == Type::PARENTHESIS || v[i].value[v[i].value.length() - 1] == ']')
			{
				std::vector<var> buff;

				if (v[i].type == Type::ARRAY || v[i].type == Type::PARENTHESIS) buff = lexer(v[i].value.substr(1, v[i].value.length() - 2));
				else
				{
					size_t ii = getSubscript(v[i]);
					buff = lexer(v[i].value.substr(ii + 1, v[i].value.length() - ii - 2));
				}

				for (size_t ii = 0; ii < buff.size(); ++ii)
				{
					if (buff[ii].type == Type::BARE || (buff[ii].type == Type::SYMBOL && (buff[ii].value == "{" || buff[ii].value == "}")))
					{
						error = (buff[ii].type == Type::BARE) ? "Unexpected name '" : "Unexpected symbol '";
						error += buff[ii].value + "' in ";
						if (v[i].type == Type::PARENTHESIS) error += "parenthesis!";
						else if (v[i].type == Type::ARRAY) error += "array!";
						else error += "subscript!";

						break;
					}
				}

				if (error != "") return false;
				if (!parse(buff)) return false;

				continue;
			}
			else if (v[i].type == Type::UNDEFINED && v[0].value != "function" && v[0].value != "cin")
			{
				if (i + 1 < v.size() && v[i].value.find_first_of("[]") == std::string::npos)
				{
					if (v[i + 1].type == Type::SYMBOL && v[i + 1].value == "=")
					{
						defined.emplace_back(v[i].value);
						continue;
					}
				}

				if (getVar(v[i].value).type == Type::UNDEFINED && (v[i].value.find("(") == std::string::npos || v[i].value[0] == '('))
				{
					if (std::find(defined.begin(), defined.end(), v[i].value) == defined.end())
					{
						error = "Name '" + v[i].value + "' not recognized!"; break;
					}
				} 
			}
			else if (i + 1 < v.size() && v[i + 1].type == Type::SYMBOL && v[i + 1].value == "=")
			{
				error = "Unexpected symbol '=' after '" + v[i].value + "'!"; break;
			}
		}

		if (error != "") return false;
		return true;
	}

	bool Interpreter::bare(const std::vector<var>& v)
	{
		if (v[0].type == Type::BARE)
		{
			if (v[0].value == "return")
			{
				if (v.size() < 2) returned = var("1");
				else returned = v[1];
				return false;
			}
			else if (enabledIO && v[0].value == "pause")
			{
				while (!_kbhit()) std::this_thread::sleep_for(std::chrono::milliseconds(10));
				_getch();
			}
			else if (enabledIO && v[0].value == "cout")
			{
				for (size_t i = 1; i < v.size(); ++i)
				{
					if (v[i].type == Type::CHAR) write(std::string(1, char(v[i].getInt())));
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
					if (test.type == Type::UNDEFINED || test.type == Type::BARE) buff = "\"" + buff + "\"";
					setVar((v[i].name != "") ? v[i].name : v[i].value, buff);
				}
			}
			else if (v[0].value == "if" || v[0].value == "while" || v[0].value == "for" || v[0].value == "else" || v[0].value == "do" || v[0].value == "function")
			{
				for (size_t i = 0; i < v.size(); ++i) action.emplace_back(v[i]);
			}
			else if (v[0].value == "delete")
			{
				for (size_t i = 1; i < v.size(); ++i) if (v[i].type != Type::SYMBOL) deleteVar(v[i].name);
			}
			else if (v[0].value == "cll")
			{
				bool state = newInterpreter(v);
				if (!state) return errorLog();
			}
			else if (v[0].value == "continue") continued = true;
			else if (v[0].value == "break") broke = true;
			else if (v[0].value == "include")
			{
				std::fstream buff(v[1].getString(), std::ios::in, std::ios::binary);
				std::string l;

				if (buff.good())
				{
					while (getline(buff, l))
					{
						if (!readLine(l)) return errorLog();
					}
				}
				else error = "File '" + v[1].getString() + "' could not be included!";
			}
		}
		else if (v[0].value == "{" && v[0].type == Type::SYMBOL) scope = 1;
		else if (v.size() == 1 && v[0].type != Type::UNDEFINED) output = v[0].value + " " + v[0].getType();

		if (error != "") return false;
		return true;
	}

	std::vector<var> Interpreter::math(const std::vector<var>& v)
	{
		std::vector<var> vec;
		vec.reserve(v.size());

		for (size_t i = 0; i < v.size(); ++i)
		{
			if (v[i].type == Type::PARENTHESIS)
			{
				std::vector<var> buff = math(lexer(v[i].value.substr(1, v[i].value.length() - 2)));
				var errflag("");

				for (size_t i = 0; i < buff.size(); ++i)
				{
					if (buff[i].type == Type::UNDEFINED)
					{
						errflag = buff[i]; break;
					}

					if (!(i % 2 == 0) && buff[i].type != Type::SYMBOL && buff[i].value != ",")
					{
						errflag = var("UNDEFINED"); break;
					}
				}

				if (errflag.value == "") vec.insert(std::end(vec), std::begin(buff), std::end(buff));
				else vec.emplace_back(errflag);
			}
			else if (v[i].type == Type::ARRAY)
			{
				std::vector<var> buff = math(lexer(v[i].value.substr(1, v[i].value.length() - 2)));
				var errflag("");
				var arr("[]");
				for (size_t i = 0; i < buff.size(); ++i)
				{
					if (buff[i].type == Type::UNDEFINED)
					{
						errflag = buff[i]; break;
					}

					if (!(i % 2 == 0) && buff[i].type != Type::SYMBOL && buff[i].value != ",")
					{
						var err(buff[i].value + " " + buff[i - 1].value);
						err.type = Type::UNDEFINED;
						errflag = err; break;
					}
					
					if (i % 2 == 0) arr += buff[i].value;
				}

				if (errflag.value == "") vec.emplace_back(arr);
				else vec.emplace_back(errflag);
			}
			else if(v[i].type == Type::UNDEFINED)
			{
				if (v[i].value.find("(") != std::string::npos && v[i].value[v[i].value.length() - 1] == ')')
				{
					std::string fun = v[i].value.substr(0, v[i].value.find("("));
					std::vector<var> args = math(lexer(v[i].value.substr(fun.length(), v[i].value.length() - fun.length())));
					function buff = functions.get(fun);
					defined dbuff = dfunctions.get(fun);
					bool errflag = false;

					for (size_t i = 0; i < args.size(); ++i)
					{
						if (args[i].type == Type::UNDEFINED) errflag = true;
					}

					if (errflag) vec.emplace_back(v[i]);
					else if (dbuff.name != "" && parse({ v[i].value.substr(fun.length(), v[i].value.length() - fun.length()) }))
					{
						var ret = newFunction(args, dbuff.lines);
						if (ret.value != "") vec.emplace_back(ret);
					}
					else if (buff.name != "" && parse({ v[i].value.substr(fun.length(), v[i].value.length() - fun.length()) }))
					{
						vec.emplace_back(buff.exec(args));
					}
					else vec.emplace_back(v[i]);
				}
				else
				{
					var buff = getVar(v[i].value);
					if (buff.type != Type::UNDEFINED) vec.emplace_back(buff);
					else vec.emplace_back(v[i]);
				}
			}
			else vec.emplace_back(v[i]);
		}

		// MATH WITH OPERATOR PRECEDENCE
		bool assignment = false;

		for (unsigned char step = 0; step < 14; ++step)
		{
			for (size_t i = 0; i < vec.size(); ++i)
			{
				if (i > 0 && step == 0)
				{
					// PREFIX OPERATORS

					if (vec[i - 1].type == Type::SYMBOL)
					{
						if (vec[i - 1].value == "!" || vec[i - 1].value == "~" || vec[i - 1].value == "-")
						{
							if (vec[i].type == Type::SYMBOL || vec[i].type == Type::UNDEFINED) continue;

							var ins;

							if (vec[i - 1].value == "!") ins = !vec[i];
							else if (vec[i - 1].value == "~") ins = ~vec[i];
							else if (vec[i - 1].value == "-")
							{
								if (i > 1 && (vec[i - 2].type != Type::SYMBOL && vec[i - 2].type != Type::BARE)) continue;
								ins = var("0") - vec[i];
							}

							vec.erase(vec.begin() + (i - 1), vec.begin() + i + 1);
							vec.insert(vec.begin() + (i - 1), ins);
							i -= 2;
						}
					}
				}
				else if (i > 1 && step < 12)
				{
					if (vec[i - 1].type == Type::SYMBOL)
					{
						if (vec[i - 2].type == Type::SYMBOL || vec[i - 2].type == Type::UNDEFINED) continue;
						if (vec[i].type == Type::SYMBOL || vec[i].type == Type::UNDEFINED) continue;
						if (vec[i - 2].type == Type::SYMBOL && vec[i - 1].value != "-") continue;
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
							else if (vec[i - 1].value == "===") ins = std::to_string((vec[i - 2] == vec[i]).getBool() && vec[i - 2].type == vec[i].type);
							else if (vec[i - 1].value == "!==") ins = std::to_string((vec[i - 2] != vec[i]).getBool() || vec[i - 2].type != vec[i].type);
						}
						else if (step == 7 && vec[i - 1].value == "&") ins = vec[i - 2] & vec[i];
						else if (step == 8 && vec[i - 1].value == "^") ins = vec[i - 2] ^ vec[i];
						else if (step == 9 && vec[i - 1].value == "|") ins = vec[i - 2] | vec[i];
						else if (step == 10 && vec[i - 1].value == "&&") ins = vec[i - 2] && vec[i];
						else if (step == 11 && vec[i - 1].value == "||") ins = vec[i - 2] || vec[i];

						if (ins.value != "")
						{
							vec.erase(vec.begin() + i - 2, vec.begin() + i + 1);
							vec.insert(vec.begin() + (i - 2), ins);
							i -= 2;
						}
					}
				}
				else if (i > 0 && step == 12)
				{
					if (vec[i].type == Type::SYMBOL && vec[i].value == "?")
					{
						if (vec[i - 1].type == Type::SYMBOL || vec[i - 1].type == Type::UNDEFINED) continue;
						std::vector<var> ins;
						ins.reserve(10);
						bool state = vec[i - 1].getBool();
						bool buff = false;

						for (size_t ii = i + 1; ii < vec.size(); ++ii)
						{
							if (vec[ii].type == Type::SYMBOL && vec[ii].value == ":") buff = true;
							else if (!buff && state) ins.push_back(vec[ii]);
							else if (buff && !state) ins.push_back(vec[ii]);
						}

						if (!ins.empty())
						{
							vec.erase(vec.begin() + i - 1, vec.end());
							for (size_t ii = 0; ii < ins.size(); ++ii) vec.insert(vec.begin() + (i - 1) + ii, ins[ii]);
							i -= 1;
						}
					}
				}
				else if (i > 1 && step == 13)
				{
					// ASSIGNMENT OPERATORS

					var lvar = vec[(vec.size() - 1) - i + 2];
					var symb = vec[(vec.size() - 1) - i + 1];
					var fvar = vec[(vec.size() - 1) - i];
					var ins;

					if (lvar.type == Type::SYMBOL || lvar.type == Type::BARE) continue;
					if (fvar.type == Type::SYMBOL || fvar.type == Type::BARE) continue;

					if (lvar.name != "") lvar = getVar(lvar.name);
					if (fvar.name != "") fvar = getVar(fvar.name);

					if (lvar.type == Type::UNDEFINED) continue;

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

					ins.name = (fvar.name == "") ? fvar.value : fvar.name;
					
					setVar(ins);

					assignment = true;

					vec.erase(vec.end() - i - 1, vec.end() - i + 2);

					if (getVar(ins.name).value != "") vec.insert(vec.end() - (i - 2), ins);
					else vec.insert(vec.end() - (i - 2), fvar);

					i -= 2;
				}
			}
		}

		if (assignment && vec.size() > 1) vec = math(vec);

		return vec;
	}

	bool Interpreter::readScope(const std::vector<var>& v)
	{
		if (v[0].value == "{" && v[0].type == Type::SYMBOL) ++scope;
		if (v[0].value == "}" && v[0].type == Type::SYMBOL) --scope;

		if (scope)
		{
			std::string buff("");
			for (size_t i = 0; i < v.size(); ++i) buff += v[i].value + " ";
			lines.emplace_back(buff);
			return true;
		}
		else
		{
			bool state = true;

			if (action.empty()) state = newScope(lines);
			else if (action[0].value == "function")
			{
				dfunctions.add(defined(action[1].value, lines));
			}
			else if (action[0].value == "if" && action[1].getBool()) state = newScope(lines);
			else if (action[0].value == "else")
			{
				if (action.size() <= 1 && !previous_action[previous_action.size() - 1].getBool()) state = newScope(lines); // ELSE IF
				else if (!previous_action[previous_action.size() - 1].getBool() && action[2].getBool())	state = newScope(lines); // ELSE
				else action = previous_action;
			}
			else if (action[0].value == "while")
			{
				while (math(action)[1].getBool())
				{
					state = newScope(lines);

					if (continued) continue;
					if (broke || !state) break;
				}
			}
			else if (action[0].value == "for")
			{
				std::vector<var> name;
				std::vector<var> loop;
				std::vector<var> incr;
				unsigned char commas = 0;

				for (size_t i = 1; i < action.size(); ++i)
				{
					if (action[i].type == Type::SYMBOL && action[i].value == ",")
					{
						++commas;
						continue;
					}
					
					if (commas == 0) name.emplace_back(action[i]);
					else if (commas == 1) loop.emplace_back(action[i]);
					else if (commas == 2) incr.emplace_back(action[i]);
				}

				std::string buff = (name.size() > 1) ? math(name)[0].name : "";

				while (math(loop)[0].getBool())
				{
					state = newScope(lines);
					math(incr);

					if (continued) continue;
					if (broke || !state) break;
				}

				if (name.size() > 1) deleteVar(buff);
			}
			else if (action[0].value == "do")
			{
				state = newScope(lines);
				if (state)
				{
					while (math(action)[2].getBool())
					{
						state = newScope(lines);

						if (continued) continue;
						if (broke || !state) break;
					}
				}
			}

			previous_action = action;
			action.clear();
			lines.clear();
			if (!state) return errorLog();
			return true;
		}
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
		args.reserve(args_line.size());
		std::string newline("");
		bool multiline = false;
			
		for (size_t i = 0; i < args_line.size(); ++i)
		{	
			if (!multiline && args_line[i].type == Type::SYMBOL && (args_line[i].value == ";" || args_line[i].value == "{" || args_line[i].value == "}"))
			{
				multiline = true;

				if (args_line[i].value == ";") continue;
				else if (i == 0)
				{
					args.emplace_back(args_line[i]);
					continue;
				}
			}

			if (multiline) newline += args_line[i].value + " ";
			else args.emplace_back(args_line[i]);
		}

		if (!action.empty() && !scope && args[0].value != "{")
		{
			scope = 1;
			newline += "}";
		}

		if (args.empty())
		{
			if (!args_line.empty() && !readLine(newline)) return errorLog();
			return true;
		}

		// SCOPING - i.e. CODE IN BRACKETS
		if (scope)
		{
			if (!readScope(args)) return errorLog();
			if (newline != "" && !readLine(newline)) return errorLog();
			return true;
		} 

		// PRINTS DEBUG MODE ADDITIONAL INFORMATION (before math)
		if (debug)
		{
			write("\nDEBUG: ");

			for (size_t i = 0; i < args.size(); ++i)
			{
				write(args[i].value + " " + args[i].getType());
				if (i != args.size() - 1) write(" | ");
			}

			if (args.size() != 0) write("\n");
		}

		// PARSER
		if (!parse(args)) return errorLog();

		std::vector<var> old_args = args;

		// APPLIES MATH TO TOKENS
		args = math(args);

		if (error != "") return errorLog();
		if (args.empty()) return true;

		// PRINTS DEBUG MODE ADDITIONAL INFORMATION (after math)
		if (debug)
		{
			write("DEBUG: ");

			for (size_t i = 0; i < args.size(); ++i)
			{
				write(args[i].value + " " + args[i].getType());
				if (i != args.size() - 1) write(" | ");
			}

			if (args.size() != 0) write("\n");
		}

		if (args[0].value != "function" && args[0].value != "cin")
		{
			for (size_t i = 0; i < args.size(); ++i)
			{
				if (args[i].type == Type::UNDEFINED)
				{
					error = "Name '" + args[i].value + "' not recognized!";
					return errorLog();
				}
			}
		}

		// INTERPRETS ARGUMENTS
		if (args[0].value == "do" || args[0].value == "while" || args[0].value == "for")
		{
			if (!bare(old_args)) return errorLog();
		}
		else if (!bare(args)) return errorLog();

		if (newline != "" && !readLine(newline)) return errorLog();
		return true;
	}

	bool Interpreter::readVector(const std::vector<std::string>& v)
	{
		for (size_t i = 0; i < v.size(); ++i)
		{
			if (!readLine(v[i]))
			{
				if (error != "") return false;
				else if (returned.value == "") return true;
				else return false;
			}
		}

		return errorLog();
	}

	// Function that interpretes whole file line by line
	bool Interpreter::readFile(const std::string& f)
	{
		filename = f;

		std::fstream file(filename, std::ios::in);
		std::string l("");

		if (file.good())
		{
			while (std::getline(file, l))
			{
				line++;
				if (!readLine(l))
				{
					file.close();

					if (error != "") return false;
					else if (returned.value == "") return true;
					else return false;
				}
			}
		}
		else error = "File '" + f + "' could not be opened!";

		line = 0;
		filename.clear();
		file.close();

		return errorLog();
	}

	// Function that returns declared var by it's name - or 'undefined' if var is not declared
	var Interpreter::getVar(const std::string& n)
	{
		if (n.length() > 1 && n.find_first_of("[]") != std::string::npos)
		{
			if (n[n.length() - 1] == ']')
			{
				bool literal = false;

				size_t ii = getSubscript(n);
				std::string buff = n.substr(ii + 1, n.length() - ii - 2);
				std::string name = n.substr(0, ii);
				if (name == "" || name == "()" || name == "[]") return var(n, "");

				std::vector<var> elem = math(lexer(buff));

				if (elem.empty()) return var(n, "");
				if (elem[0].type == Type::UNDEFINED) return var(n, "");
				if (elem.size() > 1) return var(n, "");

				var ret = getVar(name);

				if (ret.type == Type::UNDEFINED)
				{
					var test(name);

					if (test.type != Type::BARE && test.type != Type::UNDEFINED)
					{
						ret = math(lexer(name))[0].getElement(size_t(elem[0].getInt())); // String literals, arrays and parenthesis
						literal = true;
					}
				}
				else ret = ret.getElement((size_t)elem[0].getInt());

				if (ret.type == Type::CHAR && ret.getInt() == 0) return var(n, "");
				else if (ret.value == "") return var(n, "");
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
		else return var(n, "");
	}

	// Function that changes defined var value or creates new var if one does not exist
	void Interpreter::setVar(const var& v)
	{
		var ins = v;

		if (v.name.length() > 1 && v.name.find_first_of("[]") != std::string::npos)
		{
			if (v.name[v.name.length() - 1] == ']')
			{
				size_t ii = getSubscript(v);
				std::string buff = v.name.substr(ii + 1, v.name.length() - ii - 2);
				std::string name = v.name.substr(0, ii);
				if (name == "" || name == "()" || name == "[]") return;

				std::vector<var> elem = math(lexer(buff));
				if (elem.empty()) return;

				var ret = getVar(name);

				if (ret.type == Type::UNDEFINED) return;
				if (elem[0].type == Type::UNDEFINED) return;
				if (elem.size() > 1) return;

				ret.setElement((size_t)elem[0].getInt(), v.value);

				setVar(ret);

				return;
			}
		}

		if (ins.getError() != "" || ins.name == "") return;

		size_t index = search(vars, ins.name, 0, vars.size() - 1);
		if (index < vars.size()) vars[index] = ins;
		else if (ins.name != "") vars.insert(std::upper_bound(vars.begin(), vars.end(), ins, [](var a, var b) { return a.name < b.name; }), ins);
	}

	void Interpreter::deleteVar(const std::string& n)
	{
		if (n.length() > 1 && n.find_first_of("[]") != std::string::npos)
		{
			if (n[n.length() - 1] == ']') setVar(n, "");

			var buff = getVar(n.substr(0, n.find("[")));
			if (buff.getSize() == 0 && buff.type != Type::STRING && buff.type != Type::ARRAY) deleteVar(n.substr(0, n.find("[")));

			return;
		}

		size_t index = search(vars, n, 0, vars.size() - 1);
		if (index < vars.size()) vars.erase(vars.begin() + index);
	}
}
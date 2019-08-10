#include "CLL.h"

// Author: Bartosz Niciak

#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <thread>
#include <cmath>

namespace cll
{
	// STATIC VARIABLES FOR METHODS AND FUNCTIONS //

	// Vector of bare words
	const std::vector<std::string> barewords =
	{
		"cll",
		"new", "delete",
		"return", "pause",
		"cin", "cout", "endl",
		"do", "while",
		"else", "if",
		"{", "}", ";"
	};

	// Vector of restricted names
	const std::vector<std::string> rnames =
	{
		"INVALID_VALUE", "INVALID_NAME", "UNDEFINED", "inf", "-inf"
	};

	// Vector of function names
	const std::vector<std::string> functions =
	{
		"len",
		"typeof",
		"sleep",
		"fopen", "fwrite",
		"sqrt", "abs", "floor", "ceil", "round",
		"rand", "time",
		"float", "double", "str", "char", "int"
	};

	// Vector of multiple char operators
	const std::vector<std::string> multi_symbols =
	{
		"==", "!=", ">=", "<=", 
		"**", "&&", "||", "<<", ">>",
		"+=", "-=", "/=", "*=", "%=",
		"|=", "&=", "^="
	};

	// Vector of math operators
	const std::vector<std::string> math_symbols =
	{
		">", "<", "+", "-", "*", "/", "%", "^", "&", "|",
		"==", "!=", ">=", "<=", "**", "&&", "||"
	};

	// Symbols that are illegal in variable name
	const std::string symbols = "~{}/,.<>\\|&*!@#$%^&*()+-=;':\"?";

	// Symbols that create new token
	const std::string lexer_symbols = " ~{}/,<>\\|&*!@#$%^&*+-=;:\?";

	//// VARIABLE ABSTRACT ////

	// SET METHODS //
	void var::setName(const std::string& n)
	{
		name = n;

		if (n.length() > 0)
		{
			if (isdigit(n[0])) name = "INVALID_NAME"; // Checks for illegal first digit
		}
		
		if (n.find_first_of(symbols) != std::string::npos) name = "INVALID_NAME";
		else if (std::find(barewords.begin(), barewords.end(), n) != barewords.end()) name = "INVALID_NAME";
		else if (std::find(rnames.begin(), rnames.end(), n) != rnames.end()) name = "INVALID_NAME";
		else if (std::find(functions.begin(), functions.end(), n) != functions.end()) name = "INVALID_NAME";
	}

	void var::setType(const std::string& v)
	{
		type = "UNDEFINED";
		if (v == "") return;

		if (v == "inf" || v == "-inf" || v == "-nan(ind)") type = "DOUBLE";
		
		if (v.find_first_not_of("-.0123456789f") == std::string::npos) // Checks for numerical
		{
			if (v == "-" || v == "-=") type = "SYMBOL";
			else if (v.find('.') != std::string::npos)
			{
				if (v[v.length() - 1] == 'f') type = "FLOAT";
				else type = "DOUBLE";

				if (v == ".") type = "SYMBOL";
			}
			else if (v.find("f") == std::string::npos) type = "INT";
		}

		if (v.length() > 0) // Checks for string, char or array
		{
			if (v[0] == '"' && v[v.length() - 1] == '"') type = "STRING";
			else if (v[0] == '\'' && v[v.length() - 1] == '\'') type = "CHAR";
			else if (v[0] == '[' && v[v.length() - 1] == ']')
			{
				if (v.find("][") == std::string::npos) type = "ARRAY";
			}
			else if (v[0] == '(' && v[v.length() - 1] == ')')
			{
				if (v.find(")(") == std::string::npos) type = "PARENTHESIS";
			}
		}

		if (type == "UNDEFINED")
		{
			if (v.find_first_of(symbols) != std::string::npos && v.length() == 1) type = "SYMBOL"; // Checks for symbols
			else if (std::find(multi_symbols.begin(), multi_symbols.end(), v) != multi_symbols.end()) type = "SYMBOL"; // Check for multiple symbols (==, !=, ...)
			else if (std::find(barewords.begin(), barewords.end(), v) != barewords.end()) type = "BARE"; // Checks for bare words
			else if(v.find("(") != std::string::npos && v.find(")") != std::string::npos) // Checks for functions
			{
				for (size_t i = 0; i < functions.size(); i++)
				{
					if (v.length() >= functions[i].length())
					{
						if (v.substr(0, functions[i].length()) == functions[i])
						{
							if (v[v.length() - 1] == ')' && v.substr(functions[i].length(), 1) == "(")
							{
								type = "FUNCTION";
								break;
							}
						}
					}
				}
			}
		}
	}

	void var::setValue(const std::string& v)
	{
		setType(v);

		if (type == "STRING" || type == "CHAR") value = v;
		else if (type == "INT" || type == "FLOAT" || type == "DOUBLE")
		{
			try 
			{ 
				if (type == "INT") value = std::to_string(std::stoll(v));
				else if(type == "FLOAT") value = std::to_string(std::stof(v));
				else if (type == "DOUBLE")
				{
					if (v == "-nan(ind)") value = "-inf";
					else value = std::to_string(std::stod(v));
				}
			}
			catch (const std::invalid_argument&) { value = "INVALID_VALUE"; type = "UNDEFINED"; }
			catch (const std::out_of_range&) { value = "inf"; type = "DOUBLE"; }
		}
		else value = v;
	}

	void var::setElement(const size_t& n, const var& v)
	{
		if (value.length() < 1) return;

		size_t actual_element = 0;
		std::string ins = "";
		if(value[0] == '[' || value[0] == '"' || value[0] == '\'') ins = std::string(1, value[0]);

		if (type == "ARRAY")
		{
			std::vector<var> buff = interpret(value.substr(1, value.length() - 2));

			for (size_t i = 0; i < buff.size(); i++)
			{
				if (buff[i].type != "SYMBOL" && buff[i].value != ",")
				{
					if (actual_element == n)
					{
						ins += v.getString(); 
						continue;
					}
				}
				else actual_element++;

				if (actual_element == n && v.value == "") continue; // TODO: test deleting
					
				ins += buff[i].value;
			}
		}
		else
		{
			std::string buff = getString();

			for (size_t i = 0; i < buff.length(); i++)
			{
				actual_element++;

				if ((actual_element - 1) == n)
				{
					ins += v.getString();
					continue;
				}

				ins += buff[i];
			}
		}

		if (value[value.length() - 1] == ']' || value[value.length() - 1] == '"' || value[value.length() - 1] == '\'') ins += value[value.length() - 1];
		value = ins;
	}

	void var::deleteElement(const size_t& n) { setElement(n, var("")); }

	// GET METHODS //
	long long int var::getInt() const
	{ 
		if (type == "STRING" || type == "ARRAY") return getSize();
		else if (type == "CHAR")
		{
			if (value.length() == 3) return int(value[1]);
			else
			{
				if (value == "'\\0'")		return int('\0');
				else if (value == "'\\n'")	return int('\n');
				else if (value == "'\\t'")	return int('\t');
				else if (value == "'\\v'")	return int('\v');
				else if (value == "'\\b'")	return int('\b');
				else if (value == "'\\r'")	return int('\r');
				else if (value == "'\\f'")	return int('\f');
				else if (value == "'\\a'")	return int('\a');
				else if (value == "'\\\\'")	return int('\\');
				else if (value == "'\\?'")	return int('\?');
				else if (value == "'\\''")	return int('\'');
				else if (value == "'\\\"'") return int('\"');
				else return 0;
			}
		}
		else
		{
			try { return std::stoll(value); }
			catch (const std::invalid_argument&) { return 0; }
			catch (const std::out_of_range&) { return 0; }
		}
	}

	bool var::getBool() const
	{ 
		return (value == "0" || value == "" || getInt() == 0) ? false : true; 
	}

	char var::getChar(const size_t& n = 0) const
	{
		if (type == "CHAR" || type == "INT") return char((getInt() != 27) ? getInt() : ' ');
		else if (type == "STRING") return (n < value.length() - 2) ? value[n + 1] : '\0';
		else return '\0';
	}

	float var::getFloat() const
	{
		if (type == "STRING" || type == "ARRAY") return 0;
		else
		{
			try { return std::stof(value); }
			catch (const std::invalid_argument&) { return 0; }
			catch (const std::out_of_range&) { return 0; }
		}
	}

	double var::getDouble() const
	{
		if (type == "STRING" || type == "ARRAY") return 0;
		else
		{
			try { return std::stod(value); }
			catch (const std::invalid_argument&) { return 0; }
			catch (const std::out_of_range&) { return 0; }
		}
	}

	var var::getElement(const size_t& n = 0) const
	{
		if (type == "STRING") return var("'" + std::string(1, getChar(n)) + "'");
		else if (type == "CHAR" && n < getSize()) return var("'" + std::string(1, getChar(n)) + "'");
		else if (type == "ARRAY")
		{
			std::vector<var> buff = interpret(value.substr(1, value.length() - 2));
			size_t actual_element = 0;

			for (size_t i = 0; i < buff.size(); i++)
			{
				if (buff[i].type != "SYMBOL" && buff[i].value != ",")
				{
					if (actual_element == n) return buff[i];
				}
				else actual_element++;
			}
		}
		else if(n < getSize()) return var("'" + std::string(1, value[n]) + "'");
		
		return var("");
	}

	std::string var::getString() const
	{
		if (type == "STRING" || type == "CHAR") return value.substr(1, value.length() - 2);
		else return value;
	}
	std::string var::getValue() const { return value; }

	std::string var::getError() const
	{ 
		if (name == "INVALID_NAME") return name;
		else if (value == "INVALID_VALUE") return value;
		else return "";
	}
	
	size_t var::getSize() const
	{
		if (type == "ARRAY")
		{
			std::vector<var> buff = interpret(value.substr(1, value.length() - 2));
			size_t size = 0;

			for (size_t i = 0; i < buff.size(); i++)
			{
				if (buff[i].type == "SYMBOL" && buff[i].value == ",") size++;
			}

			return (size + 1);
		}
		if (type == "STRING" || type == "CHAR") return (value.length() - 2);
		return value.length();
	}

	// OPERATORS //
	std::ostream& operator<<(std::ostream& out, const var& v)
	{
		return out << v.value;
	}

	std::istream& operator>>(std::istream& in, var& v)
	{
		std::string buff;
		in >> buff;
		v.setValue(buff);
		return in;
	}

	// BOOLEAN OPERATORS //
	var var::operator~() const
	{
		return var(std::to_string(~getInt()));
	}

	var var::operator!() const
	{
		return (getBool()) ? var("0") : var("1");
	}

	var var::operator==(const var& v) const
	{
		std::string val = value;

		if (getValue() == v.getValue()) val = "1";
		else if (getInt() == v.getInt() || getInt() == v.getFloat() || getInt() == v.getDouble()) val = "1";
		else if (getFloat() == v.getInt() || getFloat() == v.getFloat() || getFloat() == v.getDouble()) val = "1";
		else if (getDouble() == v.getInt() || getDouble() == v.getFloat() || getDouble() == v.getDouble()) val = "1";
		else val = "0";

		return var(val);
	}

	var var::operator!=(const var& v) const
	{
		std::string val = value;

		val = ((var(val) == v).value == "1") ? "0" : "1";

		return var(val);
	}

	var var::operator>(const var& v) const
	{
		std::string val = value;
		bool state = true;

		if (type == "STRING") state = (getString() > v.getString());
		else if (type == "INT" || type == "CHAR")
		{
			if (v.type == "DOUBLE") state = (getFloat() > v.getDouble());
			else if (v.type == "FLOAT") state = (getFloat() > v.getFloat());
			else state = (getInt() > v.getInt());
		}
		else if (type == "FLOAT")
		{
			if (v.type == "DOUBLE") state = (getFloat() > v.getDouble());
			else if (v.type == "FLOAT") state = (getFloat() > v.getFloat());
			else state = (getFloat() > v.getInt());
		}
		else if (type == "DOUBLE")
		{
			if (v.type == "DOUBLE") state = (getDouble() > v.getDouble());
			else if (v.type == "FLOAT") state = (getDouble() > v.getFloat());
			else state = (getDouble() > v.getInt());
		}

		val = (state) ? "1" : "0";
		return var(val);
	}

	var var::operator<(const var& v) const
	{
		std::string val = value;
		bool state = true;

		if (type == "STRING") state = (getString() < v.getString());
		else if (type == "INT" || type == "CHAR")
		{
			if (v.type == "DOUBLE") state = (getFloat() < v.getDouble());
			else if (v.type == "FLOAT") state = (getFloat() < v.getFloat());
			else state = (getInt() < v.getInt());
		}
		else if (type == "FLOAT")
		{
			if (v.type == "DOUBLE") state = (getFloat() < v.getDouble());
			else if (v.type == "FLOAT") state = (getFloat() < v.getFloat());
			else state = (getFloat() < v.getInt());
		}
		else if (type == "DOUBLE")
		{
			if (v.type == "DOUBLE") state = (getDouble() < v.getDouble());
			else if (v.type == "FLOAT") state = (getDouble() < v.getFloat());
			else state = (getDouble() < v.getInt());
		}

		val = (state) ? "1" : "0";
		return var(val);
	}

	var var::operator>=(const var& v) const
	{
		std::string val = value;

		val = ((var(val) > v).value == "1" || (var(val) == v).value == "1") ? "1" : "0";

		return var(val);
	}

	var var::operator<=(const var& v) const
	{
		std::string val = value;

		val = ((var(val) < v).value == "1" || (var(val) == v).value == "1") ? "1" : "0";

		return var(val);
	}	
	
	var var::operator&&(const var& v) const
	{
		std::string val = value;

		val = (getBool() && v.getBool()) ? "1" : "0";

		return var(val);
	}

	var var::operator||(const var& v) const
	{
		std::string val = value;

		val = (getBool() || v.getBool()) ? "1" : "0";

		return var(val);
	}

	// MATH OPERATORS //
	var var::operator+(const var& v) const
	{
		std::string val = value;

		if (type == "ARRAY" || v.type == "ARRAY")
		{
			if (val[val.length() - 1] == ']') val.pop_back();

			if (v.type == "ARRAY") val += "," + v.getString().substr(1);
			else val += "," + v.getValue() + "]";

			if (val[0] != '[') val = "[" + val;
		}
		else if (type == "STRING" || v.type == "STRING")
		{
			if(v.type == "CHAR") val = "\"" + getString() + v.getChar() + "\"";
			else val = "\"" + getString() + v.getString() + "\"";
		}
		else if (type == "INT" || type == "CHAR")
		{
			if(v.type == "DOUBLE") val = std::to_string(getFloat() + v.getDouble());
			else if(v.type == "FLOAT") val = std::to_string(getFloat() + v.getFloat());
			else val = std::to_string(getInt() + v.getInt());
		}
		else if (type == "FLOAT")
		{
			if (v.type == "DOUBLE") val = std::to_string(getFloat() + v.getDouble());
			else if (v.type == "FLOAT") val = std::to_string(getFloat() + v.getFloat());
			else val = std::to_string(getFloat() + v.getInt());

			if (val.find('.') == std::string::npos) val += ".0f";
			else val += "f";
		}
		else if (type == "DOUBLE")
		{
			if (v.type == "DOUBLE") val = std::to_string(getDouble() + v.getDouble());
			else if (v.type == "FLOAT") val = std::to_string(getDouble() + v.getFloat());
			else val = std::to_string(getDouble() + v.getInt());
		}

		return var(val);
	}

	var var::operator-(const var& v) const
	{
		std::string val = value;

		if (type == "STRING") val = getString();
		else if (type == "INT" || type == "CHAR")
		{
			if (v.type == "DOUBLE") val = std::to_string(getFloat() - v.getDouble());
			else if (v.type == "FLOAT") val = std::to_string(getFloat() - v.getFloat());
			else val = std::to_string(getInt() - v.getInt());
		}
		else if (type == "FLOAT")
		{
			if (v.type == "DOUBLE") val = std::to_string(getFloat() - v.getDouble());
			else if (v.type == "FLOAT") val = std::to_string(getFloat() - v.getFloat());
			else val = std::to_string(getFloat() - v.getInt());

			if (val.find('.') == std::string::npos) val += ".0f";
			else val += "f";
		}
		else if (type == "DOUBLE")
		{
			if (v.type == "DOUBLE") val = std::to_string(getDouble() - v.getDouble());
			else if (v.type == "FLOAT") val = std::to_string(getDouble() - v.getFloat());
			else val = std::to_string(getDouble() - v.getInt());
		}

		return var(val);
	}

	var var::operator*(const var& v) const
	{
		std::string val = value;

		if (type == "STRING")
		{
			val = '"' + getString();
			if (v.getInt() > 0) for (int i = 0; i < v.getInt(); i++) val += getString();
			val += '"';
		}
		else if (type == "INT" || type == "CHAR" || type == "FLOAT")
		{
			if (v.type == "DOUBLE") val = std::to_string(getFloat() * v.getDouble());
			else if (v.type == "FLOAT") val = std::to_string(getFloat() * v.getFloat());
			else val = std::to_string(getFloat() * v.getInt());

			if (val.find('.') == std::string::npos) val += ".0f";
			else val += "f";
		}
		else if (type == "DOUBLE")
		{
			if (v.type == "DOUBLE") val = std::to_string(getDouble() * v.getDouble());
			else if (v.type == "FLOAT") val = std::to_string(getDouble() * v.getFloat());
			else val = std::to_string(getDouble() * v.getInt());
		}

		return var(val);
	}

	var var::operator/(const var& v) const
	{
		std::string val = value;

		if (type == "STRING") val = getString();
		else if (type == "INT" || type == "CHAR" || type == "FLOAT")
		{
			if (v.type == "DOUBLE" && v.getDouble() != 0) val = std::to_string(getFloat() / v.getDouble());
			else if (v.type == "FLOAT" && v.getFloat() != 0) val = std::to_string(getFloat() / v.getFloat());
			else if (v.getFloat() != 0) val = std::to_string(getFloat() / v.getInt());
			else val = "inf";

			if (val != "inf")
			{
				if (val.find('.') == std::string::npos) val += ".0f";
				else val += "f";
			}
		}
		else if (type == "DOUBLE")
		{
			if (v.type == "DOUBLE" && v.getDouble() != 0) val = std::to_string(getDouble() / v.getDouble());
			else if (v.type == "FLOAT" && v.getFloat() != 0) val = std::to_string(getDouble() / v.getFloat());
			else if (v.getInt() != 0) val = std::to_string(getDouble() / v.getInt());
			else val = "inf";
		}

		return var(val);
	}

	var var::operator%(const var& v) const
	{
		std::string val = value;

		if (type == "INT" || type == "FLOAT" || type == "DOUBLE" || type == "CHAR")
		{
			if ((v.type == "INT" || v.type == "FLOAT" || v.type == "DOUBLE" || v.type == "CHAR") && v.getInt() != 0)
			{
				val = std::to_string(getInt() % v.getInt());
			}
			else val = "inf";
		} 
		else val = "0";

		return var(val);
	}

	var var::pow(const var& v) const
	{
		std::string val = value;

		if (type == "STRING") val = getString();
		else if (type == "INT" || type == "CHAR")
		{
			if (v.type == "DOUBLE") val = std::to_string(std::pow(getFloat(), v.getDouble()));
			else if (v.type == "FLOAT") val = std::to_string(std::powf(getFloat(), v.getFloat()));
			else val = std::to_string(std::pow(getInt(), v.getInt()));
		}
		else if (type == "FLOAT")
		{
			if (v.type == "DOUBLE") val = std::to_string(std::pow(getFloat(), v.getDouble()));
			else val = std::to_string(std::powf(getFloat(), v.getFloat()));

			if (val.find('.') == std::string::npos) val += ".0f";
			else val += "f";
		}
		else if (type == "DOUBLE")
		{
			if (v.type == "DOUBLE") val = std::to_string(std::pow(getDouble(), v.getDouble()));
			else if (v.type == "FLOAT") val = std::to_string(std::pow(getDouble(), v.getFloat()));
			else val = std::to_string(std::pow(getDouble(), v.getInt()));
		}

		return var(val);
	}

	// BITWISE OPERATORS //

	var var::operator^(const var& v) const
	{
		std::string val = value;

		if (type == "INT" || type == "FLOAT" || type == "DOUBLE" || type == "CHAR")
		{
			if (v.type == "INT" || v.type == "FLOAT" || v.type == "DOUBLE" || v.type == "CHAR")
			{
				val = std::to_string(getInt() ^ v.getInt());
			}
			else val = "inf";
		}
		else val = "0";

		return var(val);
	}

	var var::operator&(const var& v) const
	{
		std::string val = value;

		if (type == "INT" || type == "FLOAT" || type == "DOUBLE" || type == "CHAR")
		{
			if (v.type == "INT" || v.type == "FLOAT" || v.type == "DOUBLE" || v.type == "CHAR")
			{
				val = std::to_string(getInt() & v.getInt());
			}
			else val = "inf";
		}
		else val = "0";

		return var(val);
	}

	var var::operator|(const var& v) const
	{
		std::string val = value;

		if (type == "INT" || type == "FLOAT" || type == "DOUBLE" || type == "CHAR")
		{
			if (v.type == "INT" || v.type == "FLOAT" || v.type == "DOUBLE" || v.type == "CHAR")
			{
				val = std::to_string(getInt() | v.getInt());
			}
			else val = "inf";
		}
		else val = "0";

		return var(val);
	}

	var var::operator<<(const var& v) const
	{
		std::string val = value;

		if (type == "INT" || type == "FLOAT" || type == "DOUBLE" || type == "CHAR")
		{
			if (v.type == "INT" || v.type == "FLOAT" || v.type == "DOUBLE" || v.type == "CHAR")
			{
				val = std::to_string(getInt() << v.getInt());
			}
			else val = "inf";
		}
		else val = "0";

		return var(val);
	}

	var var::operator>>(const var& v) const
	{
		std::string val = value;

		if (type == "INT" || type == "FLOAT" || type == "DOUBLE" || type == "CHAR")
		{
			if (v.type == "INT" || v.type == "FLOAT" || v.type == "DOUBLE" || v.type == "CHAR")
			{
				val = std::to_string(getInt() >> v.getInt());
			}
			else val = "inf";
		}
		else val = "0";

		return var(val);
	}

	//// LEXER ////

	// Function that provides lexer capabilities (i.e. converts line to arguments/vars/tokens)
	std::vector<var> interpret(const std::string& l)
	{
		std::vector<var> args; // Holds vars (token) list
		std::string buff = ""; // Holds token in buffor before pushing it to token list
		bool push = false; // Holds wheter to push actual token to list
		bool string = false; // Holds wheter token is a string (helpful to ignore special chars that create new tokens)
		bool chars = false; // Holds wheter token is a char (helpful to ignore special chars that create new tokens)
		bool comment = false; // Holds wheter token is a comment (helpful to ingore all following tokens as comments)
		unsigned int parenthesis = 0; // Holds wheter token is a parenthesis (helpful to ignore special chars that create new tokens)
		unsigned int array = 0; // Holds wheter token is an array (helpful to ignore special chars that create new tokens)
		std::string special = ""; // Holds sepcial char as token to be pushed (works like this: new x=10 -> new x = 10)

		for (size_t i = 0; i <= l.length(); i++)
		{
			if (i != l.length())
			{
				// CHECKS FOR STRINGS
				if (string && l[i] == '"') string = false;
				else if (!string && l[i] == '"') string = true;

				if (chars && l[i] == '\'') chars = false;
				else if (!chars && l[i] == '\'') chars = true;

				// CHECKS FOR PARENTHESIS
				if (!string)
				{
					if (parenthesis && l[i] == ')') parenthesis--;
					else if (l[i] == '(') parenthesis++;

					if (array && l[i] == ']') array--;
					else if (l[i] == '[') array++;
				}

				// PUSHES TO BUFFOR
				if (string || parenthesis || array || chars) buff += l[i];
				else
				{
					// CHECKS FOR SPECIAL SYMBOLS
					if (std::string(1, l[i]).find_first_of(lexer_symbols) != std::string::npos || l[i] == '\t')
					{
						push = true;

						if (l[i] != '\t' && l[i] != ' ')
						{
							special = l[i];

							// CHECKS FOR DOUBLE SPECIALS CHARS (==, !=, ...)
							if (i != l.length() - 1)
							{
								for (size_t ii = 0; ii < multi_symbols.size(); ii++)
								{
									if (l[i] == multi_symbols[ii][0] && l[i + 1] == multi_symbols[ii][1])
									{
										special += l[i + 1];
										break;
									}
								}

								if (special.length() > 1) i++;
							}
						}
					}

					// CHECKS FOR COMMENTS
					if (i + 1 != l.length())
					{
						if (l[i] == '/' && l[i + 1] == '/')
						{
							push = true;
							comment = true;
						}
					}

					if (l[i] != '\t' && std::string(1, l[i]).find_first_of(lexer_symbols) == std::string::npos && !comment) buff += l[i];
					else push = true;
				}
			}
			else push = true;

			// PUSHES ARGUMENTS TO TOKEN
			if (push)
			{
				if (buff != "") args.emplace_back(var(buff));
				push = false;
				buff = "";

				// PUSHES SPECIAL CHARS TO THEIR OWN TOKEN
				if (comment) break;
				if (special != "") args.emplace_back(var(special));
				special = "";
			}
		}

		return args;
	}

	//// INTERPRETER ////

	// Function that interpretes functions

	//TODO: rework function, so that math is done in math method
	var Interpreter::function(const std::string& fun, const std::vector<var>& args)
	{
		std::string ret = "UNDEFINED";

		if (fun == "time")
		{
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();

			if (args.empty())
				duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
			else if (args[0].type == "STRING" && (args[0].getString() == "s" || args[0].getString() == "seconds"))
				duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - start).count();
			else if (args[0].type == "STRING" && (args[0].getString() == "us" || args[0].getString() == "microseconds"))
				duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
			else if (args[0].type == "STRING" && (args[0].getString() == "ns" || args[0].getString() == "nanoseconds"))
				duration = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count();

			ret = std::to_string(duration);
		}
		else if (fun == "len")
		{
			if (args.empty()) ret = "0";
			else ret = std::to_string(var(args[0]).getSize());
		}
		else if (fun == "sqrt")
		{
			if (args.empty()) ret = "0";
			else ret = std::to_string(sqrt(args[0].getDouble()));
		}
		else if (fun == "abs")
		{
			if (args.empty()) ret = "0";
			else ret = std::to_string(fabs(args[0].getDouble()));
		}		
		else if (fun == "floor")
		{
			if (args.empty()) ret = "0";
			else ret = std::to_string((int)floor(args[0].getDouble()));
		}		
		else if (fun == "ceil")
		{
			if (args.empty()) ret = "0";
			else ret = std::to_string((int)ceil(args[0].getDouble()));
		}		
		else if (fun == "round")
		{
			if (args.empty()) ret = "0";
			else ret = std::to_string((int)round(args[0].getDouble()));
		}
		else if (fun == "fopen")
		{
			std::fstream f(args[0].getString(), std::ios::in);
			std::string l;

			if (!f.good()) ret = "[]";
			else
			{
				ret = "[";
				while (std::getline(f, l)) ret += "\"" + l + "\",";
				ret.pop_back();
				ret += "]";
			}
		}
		else if (fun == "typeof")
		{
			if (!args.empty()) ret = "\"" + var(args[0]).type + "\"";
		}
		else if (fun == "int")
		{
			if (args.empty()) ret = "0";
			else ret = std::to_string(var(args[0]).getInt());
		}
		else if (fun == "float")
		{
			if (args.empty()) ret = "0.0f";
			else ret = std::to_string(var(args[0]).getFloat());
		}
		else if (fun == "double")
		{
			if (args.empty()) ret = "0.0";
			else ret = std::to_string(var(args[0]).getDouble());
		}
		else if (fun == "char")
		{
			if (args.empty()) ret = '\0';
			else ret = "'" + std::string(1, var(args[0]).getChar()) + "'";
		}
		else if (fun == "str")
		{
			if (args.empty()) ret = "\"\"";
			else ret = "\"" + var(args[0]).getString() + "\"";
		}
		else if (fun == "sleep")
		{
			if (!args.empty()) std::this_thread::sleep_for(std::chrono::milliseconds(var(args[0]).getInt()));
			ret = "";
		}
		else if (fun == "rand")
		{
			double low = 0.0, high = 1.0;

			if (args.size() == 1) high = args[0].getDouble();
			else if (args.size() >= 3)
			{
				low  = args[0].getDouble();
				high = args[2].getDouble();
			}

			std::uniform_real_distribution<double> dist(low, high);
			ret = std::to_string(dist(random_engine));
		}

		return var(ret);
	}

	// Function that checks for errors and logs them on console screen
	bool Interpreter::errorLog()
	{
		if (error == "") return true;
		
		if (log)
		{
			std::cout << '\n' << "ERROR: ";
			if (filename != "") std::cout << "Error in file '" + filename + "' on line " + std::to_string(line) + ":" << '\n'; 
			std::cout << error;
		}

		return false;
	}

	// Function that creates new interpreter (i.e. interpretes another file)
	bool Interpreter::newInterpreter(const std::vector<var>& v)
	{
		std::string params = "[";
		for (size_t i = 1; i < v.size(); i++) params += v[i].value + ',';
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
		nested->start = start;
		nested->filename = filename;
		nested->line = (line >= l.size()) ? (line - l.size()) : 0;

		for (size_t i = 0; i < l.size(); i++)
		{
			if (!nested->readLine(l[i]))
			{
				error = nested->error;
				return false;
			}

			if (filename != "") nested->line++;
		}

		// SETTING PREVIOUS (THIS) SCOPE VARIABLES
		for (size_t i = 0; i < nested->vars.size(); i++)
		{
			var buff = this->getVar(nested->vars[i].name);

			if (buff.type != "UNDEFINED") this->setVar(nested->vars[i]);
		}

		return true;
	}

	// Function that checks line syntax
	bool Interpreter::parse(const std::vector<var>& v)
	{
		size_t undefined_check = 0; // Holds from which token to check for undefined vars

		if (v.empty()) return true;

		if (v[0].type == "BARE")
		{
			undefined_check = 1;

			// CHECKS FOR MULTIPLE BARE WORDS
			if (v[0].value == "else" && v.size() > 1)
			{
				if (v[1].value != "if") 
				{ 
					error = "Unexpected '" + v[1].value + "' after '" + v[0].value + "' statement!"; 
					return false; 
				}
				else
				{
					undefined_check = 2;

					if (v.size() < 3)
					{
						error = "Command 'else if' got too few arguments!";
						return false;
					}
				}
			}

			for (size_t i = 0; i < v.size(); i++)
			{
				if (v[i].type == "BARE" && i > 0)
				{
					if (v[0].value == "cout" && v[i].value == "endl") continue;
					if (v[0].value == "else" && i == 1) continue;
					
					error = "Unexpected '" + v[i].value + "' after '" + v[0].value + "' statement!";
					return false;
				}
			}

			// CHECKS FOR BARE WORD UNIQUE SYNTAX
			if (v[0].value == "cout" && v.size() < 2) error = "Command 'cout' got too few arguments!";
			else if (v[0].value == "delete")
			{
				if (v.size() < 2) error = "Command 'delete' got too few arguments!";

				for (size_t i = 1; i < v.size(); i++)
				{
					if (v[i].type == "SYMBOL" && v[i].value != ",") error = "Unexpected symbol '" + v[i].value + "' after 'delete' statement!";
					else if (v[i].name == "") error = "Undefined name '" + v[i].value + "' after 'delete' statement!";
					else if (getVar(v[i].name).type == "UNDEFINED") error = "Undefined name '" + v[i].name + "' after 'delete' statement!";

					if (error != "") break;
				}
			}
			else if (v[0].value == "new")
			{
				if (v.size() < 2) error = "Command 'new' got too few arguments!";

				bool name = true;
				std::vector<std::string> duplicates;

				for (size_t i = 1; i < v.size(); i++)
				{
					if (error != "") break;
					if (v[i].value == "," && v[i].type == "SYMBOL")
					{
						name = true;
						continue;
					}
							
					if (v[i].value == "=" && v[i].type == "SYMBOL")
					{
						name = false;
						continue;
					}

					if (name)
					{
						if (v[i].type == "BARE") error = "Variable name '" + v[i].value + "' is illegal!";
						else if (v[i].name != "") error = "Variable '" + v[i].name + "' was already declared!";
						else if (v[i].type == "FUNCTION") error = "Variable '" + v[i].value + "' is an existing function!";
						//else if (v[i].type == "SYMBOL") error = "Unexpected symbol '" + v[i].value + "' after 'new' statement!";
						else if (v[i].type != "UNDEFINED") error = "Variable name '" + v[i].value + "' is illegal!";
						else if (v[i].value.find_first_of("[]") != std::string::npos) error = "Variable name '" + v[i].value + "' is illegal!";
						else if (var(v[i].value, "").name == "INVALID_NAME") error = "Variable name '" + v[i].value + "' is illegal!";
						else if (std::find(duplicates.begin(), duplicates.end(), v[i].value) != duplicates.end()) 
							error = "Variable '" + v[i].value + "' cannot be declared multiple times!";
						else duplicates.emplace_back(v[i].value);
					}
					else if (v[i].type == "UNDEFINED") error = "Name '" + v[i].value + "' not recognized!";
				}
			}
			else if (v[0].value == "cll" && v.size() < 2) error = "Command 'cll' got too few arguments!";
			else if (v[0].value == "system" && v.size() < 2) error = "Command 'system' got too few arguments!";
			else if ((v[0].value == "if" || v[0].value == "while") && v.size() < 2) error = "Command '" + v[0].value + "' got too few arguments!";
		}

		if (error != "") return false;

		// Checks for undefined names
		for (size_t i = undefined_check; i < v.size(); i++)
		{
			if (i > 0 && v[i].type == "SYMBOL")
			{
				if (v[i - 1].type == "SYMBOL")
				{
					error = "Unexpected symbol '" + v[i].value + "' after a '" + v[i - 1].value + "' symbol!";
					break;
				}

				if (v[i].value != ";" && v[i].value != "{" && v[i].value != "}" && i + 1 >= v.size())
				{
					error = "Expected something after symbol '" + v[i].value + "'!";
					break;
				}
			}

			if (v[i].type == "ARRAY" || v[i].type == "PARENTHESIS" || v[i].type == "FUNCTION")
			{

				std::vector<var> buff;
				if(v[i].type != "FUNCTION") buff = interpret(v[i].value.substr(1, v[i].value.length() - 2));
				else buff = interpret(v[i].value.substr(v[i].value.find("("), v[i].value.length()));

				for (size_t ii = 0; ii < buff.size(); ii++)
				{
					if (buff[ii].type == "BARE")
					{
						error = "Unexpected name '" + buff[ii].value + "' in ";
						error += (v[i].type == "ARRAY") ? "array!" : "parenthesis!";
						break;
					}
				}

				if (!parse(buff)) return false;
			}
			else if (v[i].type == "UNDEFINED" && v[0].value != "new") 
			{ 
				if (getVar(v[i].value).type == "UNDEFINED")
				{
					error = "Name '" + v[i].value + "' not recognized!";
					break;
				}
			}
		}

		if (error != "") return false;
		return true;
	}

	bool Interpreter::bare(const std::vector<var>& v)
	{
		if (v[0].type == "BARE")
		{
			if (v[0].value == "return") { returned = true; return false; }
			else if (v[0].value == "pause")
			{
				std::string buff;
				std::getline(std::cin, buff);
			}
			else if (v[0].value == "cout")
			{
				for (size_t i = 1; i < v.size(); i++)
				{
					if (v[i].value == "endl") std::cout << '\n';
					else if (v[i].type == "CHAR") std::cout << char(v[i].getInt());
					else std::cout << v[i].getString();
				}
			}			
			else if (v[0].value == "cin")
			{
				for (size_t i = 1; i < v.size(); i++)
				{
					std::string buff;
					std::getline(std::cin, buff);
					if (var(buff).type == "UNDEFINED" || var(buff).type == "BARE" || var(buff).type == "FUNCTION") buff = "\"" + buff + "\"";
					setVar(v[i].name, buff);
				}
			}
			else if (v[0].value == "if" || v[0].value == "while" || v[0].value == "else")
			{
				scope_action.emplace_back(v[0]);
				if (v.size() > 1) scope_action.emplace_back(v[1]);
			}
			else if (v[0].value == "delete")
			{
				for (size_t i = 1; i < v.size(); i++) if (v[i].type != "SYMBOL") deleteVar(v[i].name);
			}			
			else if (v[0].value == "new")
			{
				std::vector<var> new_vars;

				for (size_t i = 1; i < v.size(); i++)
				{
					if (v[i].type != "SYMBOL")
					{
						var buff = var(v[i].value, "UNDEFINED");
						if(buff.name != "INVALID_NAME") new_vars.emplace_back(buff);
					}
					else if (v[i].value == "=") new_vars[new_vars.size() - 1] = var(new_vars[new_vars.size() - 1].name, v[i + 1]);
				}

				if (error != "") return false;

				vars.insert(std::end(vars), std::begin(new_vars), std::end(new_vars));
				std::sort(vars.begin(), vars.end(), [](var a, var b) { return a.name < b.name; });
			}
			else if (v[0].value == "cll") return newInterpreter(v);
		}
		else
		{
			// INTERPRETS OTHER SPECIAL OPERATORS
			if (v[0].value == "{" && v[0].type == "SYMBOL") scope = 1;
			else if (v[0].value == "}" && v[0].type == "SYMBOL")
			{
				if (scope > 0) scope = 0;
				else error = "Unexpected symbol '}' - nothing to close!";
			}
			else if (v.size() >= 3)
			{
				// TODO: Make it in math
				if (v[1].type == "SYMBOL")
				{
					if(v[1].value == "=") setVar(v[0].name, v[2].value);
					else if(v[1].value == "+=") setVar(v[0].name, (v[0] + v[2]).value);
					else if(v[1].value == "-=") setVar(v[0].name, (v[0] - v[2]).value);
					else if(v[1].value == "/=") setVar(v[0].name, (v[0] / v[2]).value);
					else if(v[1].value == "*=") setVar(v[0].name, (v[0] * v[2]).value);
					else if(v[1].value == "%=") setVar(v[0].name, (v[0] % v[2]).value);
					else if(v[1].value == "&=") setVar(v[0].name, (v[0] & v[2]).value);
					else if(v[1].value == "|=") setVar(v[0].name, (v[0] | v[2]).value);
					else if(v[1].value == "^=") setVar(v[0].name, (v[0] ^ v[2]).value);
				}
			}
			else
			{
				var buff = getVar(v[0].name);
				if (buff.type == "UNDEFINED") std::cout << v[0].value << " " << v[0].type;
				else std::cout << buff.value << " " << buff.type;
			}
		}

		if (error != "") return false;
		return true;
	}

	std::vector<var> Interpreter::math(const std::vector<var>& v)
	{
		std::vector<var> vec;

		for (size_t i = 0; i < v.size(); i++)
		{
			if (v[i].type == "PARENTHESIS")
			{
				std::vector<var> buff = math(interpret(v[i].value.substr(1, v[i].value.length() - 2)));
				vec.insert(std::end(vec), std::begin(buff), std::end(buff));
			}
			else if (v[i].type == "ARRAY")
			{
				std::vector<var> buff = math(interpret(v[i].value.substr(1, v[i].value.length() - 2)));
				std::string arr = "[";
				for (size_t i = 0; i < buff.size(); i++) arr += buff[i].value;
				arr += "]";
				vec.push_back(arr);
			}
			else if (v[i].type == "FUNCTION")
			{
				std::vector<var> args;
				std::string fun;

				for (size_t ii = 0; ii < functions.size(); ii++)
				{
					if (v[i].value.substr(0, functions[ii].length()) == functions[ii])
					{
						fun = v[i].value.substr(0, functions[ii].length());
						args = math(interpret(v[i].value.substr(functions[ii].length() + 1, v[i].value.length() - functions[ii].length() - 2)));
						break;
					}
				}

				vec.emplace_back(function(fun, args));
			}
			else if (getVar(v[i].value).type != "UNDEFINED") vec.emplace_back(getVar(v[i].value));
			else vec.emplace_back(v[i]);
		}

		// MATH WITH OPERATOR PRECEDENCE

		for (unsigned char step = 0; step < 12; step++)
		{
			for (size_t i = 0; i < vec.size(); i++)
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
								if (i > 1)
								{
									if (vec[i - 2].type != "SYMBOL") continue;
								}

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
						if (vec[i].type == "SYMBOL" || vec[i].type == "UNDEFINED") continue;
						if (vec[i - 2].type == "SYMBOL" && vec[i - 1].value != "-") continue;
						if (std::find(math_symbols.begin(), math_symbols.end(), vec[i - 1].value) == math_symbols.end()) continue;

						var ins("");

						//std::cout << "step " << (int)step << ": " << vec[i - 2] << vec[i - 1] << vec[i] << '\n'; // DEBUG

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
			}
		}

		return vec;
	}

	// Function that interpretes one line
	bool Interpreter::readLine(const std::string& l)
	{
		// SKIPS EMPTY LINES
		if (l == "")  return true;
		if (l == ";") return true;

		// SEPARATES LINE BY ARGUMENTS
		std::vector<var> args_line = interpret(l); // Holds raw line tokens

		// CHECKS FOR LINE BREAK (SEMICOLON) AND FOR BRACKETS
		// TODO: Repair detecting '}' at the end of line - make it new line

		std::vector<var> args; // Holds actual line tokens - everything after semicolon is treated as new line
		size_t args_size = 0; // Holds unaltered size of args
		bool skip_char = false; // Holds wheter to skip a char or not (helpfull with semicolons that should not be interpreted)

		for (size_t i = 0; i < args_line.size(); i++)
		{
			if ((args_line[i].value == ";" || args_line[i].value == "{" || args_line[i].value == "}") && args_line[i].type == "SYMBOL")
			{
				if (args_line[i].value == ";")
				{
					skip_char = true;
					break;
				}

				if ((args_line[i].value == "{" || args_line[i].value == "}") && i == 0)
				{
					args.emplace_back(args_line[i]);
					skip_char = true;
					break;
				}
				else if (args_line[i].value == "{" || args_line[i].value == "}")
				{
					skip_char = false;
					break;
				}
				
			}
			
			args.emplace_back(args_line[i]);
		}

		if (args.empty()) return true;
		args_size = args.size();

		// SCOPING - i.e. CODE IN BRACKETS
		if (scope)
		{
			for (size_t i = 0; i < args.size(); i++)
			{
				if (args[i].value == "{" && args[0].type == "SYMBOL") scope++;
				if (args[i].value == "}" && args[0].type == "SYMBOL") scope--;
			}

			if (!scope)
			{
				if (!newScope(scope_lines))
				{
					scope_lines.clear();
					return errorLog();
				}
				
				scope_lines.clear();
			}
			else scope_lines.emplace_back(l);

			return true;
		}

		// PRINTS DEBUG MODE ADDITIONAL INFORMATION (before math)
		if (debug)
		{
			std::cout << "DEBUG: ";

			for (size_t i = 0; i < args.size(); i++)
			{
				std::cout << args[i].value << " " << args[i].type;
				if (i != args.size() - 1) std::cout << " | ";
			}

			if (args.size() != 0) std::cout << '\n';
		}

		// CHECKS FOR PARSER ERRORS (before math)
		//if (!parse(args)) return errorLog();

		// APPLIES MATH TO TOKENS
		args = math(args);
		if (args.empty()) return true;

		// PRINTS DEBUG MODE ADDITIONAL INFORMATION (after math)
		if (debug)
		{
			std::cout << "DMATH: ";

			for (size_t i = 0; i < args.size(); i++)
			{
				std::cout << args[i].value << " " << args[i].type;
				if (i != args.size() - 1) std::cout << " | ";
			}

			if (args.size() != 0) std::cout << '\n';
		}

		// CHECKS FOR PARSER ERRORS (after math)
		if (!parse(args)) return errorLog();

		// INTERPRETS ARGUMENTS
		if (!bare(args)) return errorLog();

		// BREAK LINE IF SEMICOLON IS PRESENT
		if (args_line.size() != args_size && error == "")
		{
			std::string newline = ""; // Holds new line
			for (size_t i = (skip_char) ? (args_size + 1) : args_size; i < args_line.size(); i++) newline += args_line[i].value + ' ';
			return readLine(newline);
		}

		return errorLog();
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

	size_t Interpreter::searchVar(const std::string& name, const size_t& l, const size_t& r)
	{
		if (r >= l)
		{
			size_t mid = l + (r - l) / 2;

			if (mid >= vars.size()) return vars.size();
			if (vars[mid].name == name) return mid;
			if (vars[mid].name > name) return searchVar(name, l, mid - 1);
			return searchVar(name, mid + 1, r);
		}

		return vars.size();
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

				for (size_t i = 0; i < buff.length(); i++)
				{
					if (buff[i] != '[' && buff[i] != ']') raw += buff[i];
					if (buff[i] == ']') raw += ' ';
				}

				std::vector<var> elem = math(interpret(raw));
				if (elem.empty()) return var(n, "UNDEFINED");

				var ret = getVar(name);

				if (ret.type == "UNDEFINED")
				{
					var test(name);

					if (test.type != "BARE" && test.type != "UNDEFINED")
					{
						ret = math(interpret(name))[0].getElement((size_t)elem[0].getInt()); // String literals, arrays and parenthesis
						literal = true;
					}
				}
				else ret = ret.getElement((size_t)elem[0].getInt());

				if (ret.type == "CHAR" && ret.getInt() == 0) return var(n, "UNDEFINED");
				else if (ret.value == "") return var(n, "UNDEFINED");
				else if (!literal)
				{
					ret.name = name;
					for (size_t i = 0; i < elem.size(); i++) ret.name += "[" + elem[i].getString() + "]";
				}

				return ret;
			}
		}

		size_t index = searchVar(n, 0, vars.size() - 1);
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

				for (size_t i = 0; i < buff.length(); i++)
				{
					if (buff[i] != '[' && buff[i] != ']') raw += buff[i];
					if (buff[i] == ']') raw += ' ';
				}

				std::vector<var> elem = math(interpret(raw));
				if (elem.empty()) return;

				var ret = getVar(name);

				if (ret.type == "UNDEFINED") return;
				ret.setElement((size_t)elem[0].getInt(), v.value);

				setVar(ret);

				return;
			}
		}

		if (ins.name == "ILLEGAL_NAME" || ins.name == "") return;

		size_t index = searchVar(ins.name, 0, vars.size() - 1);
		if (index < vars.size()) vars[index] = ins;
		else if(ins.name != "") vars.emplace_back(v);
		std::sort(vars.begin(), vars.end(), [](var a, var b) { return a.name < b.name; });
	}

	void Interpreter::deleteVar(const std::string& n)
	{
		if (n.find_first_of("[]") != std::string::npos && n.length() > 1)
		{
			if (n[n.length() - 1] == ']') setVar(n, "");

			return;
		}

		size_t index = searchVar(n, 0, vars.size() - 1);
		if (index < vars.size()) vars.erase(vars.begin() + index);
	}

}
#include "var.h"

// Author: Bartosz Niciak

#include "lexer.h"
#include "static.h"

#include <stdexcept>
#include <cmath>

namespace cll
{
	// CONSTRUCTORS //

	// Copy constructor
	var::var(const var& v)
	{
		name = v.name;
		value = v.value;
		type = v.type;
	}

	// Copy constructor with custom name
	var::var(const std::string& n, const var& v)
	{
		setName(n);
		value = v.value;
		type = v.type;
	}

	// Value only constructor
	var::var(const std::string& v)
	{
		setName("");
		setValue(v);
	}

	// "Safe" constructor
	var::var(const std::string& n, const std::string& v)
	{
		setName(n);
		setValue(v);
	}

	// Constructor with forced value and type 
	var::var(const std::string& n, const std::string& v, const std::string& t)
	{
		setName(n); 
		value = v; 
		type = t;
	}

	// SET METHODS //
	void var::setName(const std::string& n)
	{
		name = n;

		if (n.length() > 0 && isdigit(n[0])) name = "INVALID_NAME"; // Checks for illegal first digit

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

		if (v.find_first_not_of("x0123456789abcdefABCDEF") == std::string::npos && v.length() > 2) // Checks for hexadecimal
		{
			if (v.substr(0, 2) == "0x" && v.substr(2).find_first_of("x") == std::string::npos) type = "INT";
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
			else if (v.find("(") != std::string::npos && v.find(")") != std::string::npos) // Checks for functions
			{
				for (size_t i = 0; i < functions.size(); ++i)
				{
					if (v.length() >= functions[i].length() && v.substr(0, functions[i].length()) == functions[i])
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

	void var::setValue(const std::string& v)
	{
		setType(v);

		if (type == "STRING") value = v;
		else if (type == "CHAR") value = (v.length() < 3) ? "'\\0'" : v;
		else if (type == "INT" || type == "FLOAT" || type == "DOUBLE")
		{
			try
			{
				if (type == "INT")
				{
					if (v[0] == '0' && v.length() > 1 && v.find_first_of("x89") == std::string::npos) value = std::to_string(std::stoll(v.substr(1), 0, 8));
					else if (v.substr(0, 2) == "0x" && v.length() > 2) value = std::to_string(std::stoll(v.substr(2), 0, 16));
					else value = std::to_string(std::stoll(v));
				}
				else if (type == "FLOAT") value = std::to_string(std::stof(v));
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
		if (value[0] == '[' || value[0] == '"' || value[0] == '\'') ins = std::string(1, value[0]);

		if (type == "ARRAY")
		{
			std::vector<var> buff = lexer(value.substr(1, value.length() - 2));

			for (size_t i = 0; i < buff.size(); ++i)
			{
				if (buff[i].type != "SYMBOL" && buff[i].value != ",")
				{
					if (actual_element == n)
					{
						ins += v.value;
						continue;
					}
				}
				else actual_element++;

				if (actual_element == n && v.value == "") continue;

				ins += buff[i].value;
			}
		}
		else
		{
			std::string buff = getString();

			for (size_t i = 0; i < buff.length(); ++i)
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
		setValue(ins);
	}

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

	char var::getChar(const size_t& n) const
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

	var var::getElement(const size_t& n) const
	{
		if (type == "STRING") return var("'" + std::string(1, getChar(n)) + "'");
		else if (type == "CHAR" && n < getSize()) return var("'" + std::string(1, getChar(n)) + "'");
		else if (type == "ARRAY")
		{
			std::vector<var> buff = lexer(value.substr(1, value.length() - 2));
			size_t actual_element = 0;

			for (size_t i = 0; i < buff.size(); ++i)
			{
				if (buff[i].type != "SYMBOL" && buff[i].value != ",")
				{
					if (actual_element == n) return buff[i];
				}
				else actual_element++;
			}
		}
		else if (n < getSize()) return var("'" + std::string(1, value[n]) + "'");

		return var("");
	}

	std::string var::getString() const
	{
		if (type == "STRING" || type == "CHAR") return value.substr(1, value.length() - 2);
		else return value;
	}

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
			if (value == "[]") return 0;

			std::vector<var> buff = lexer(value.substr(1, value.length() - 2));
			size_t size = 0;

			for (size_t i = 0; i < buff.size(); ++i)
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
		return ((var(value) == v).value == "1") ? var("0") : var("1");
	}

	var var::operator>(const var& v) const
	{
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

		return (state) ? var("1") : var("0");
	}

	var var::operator<(const var& v) const
	{
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

		return (state) ? var("1") : var("0");
	}

	var var::operator>=(const var& v) const
	{
		return ((var(value) > v).value == "1" || (var(value) == v).value == "1") ? var("1") : var("0");
	}

	var var::operator<=(const var& v) const
	{
		return ((var(value) < v).value == "1" || (var(value) == v).value == "1") ? var("1") : var("0");
	}

	var var::operator&&(const var& v) const
	{
		return (getBool() && v.getBool()) ? var("1") : var("0");
	}

	var var::operator||(const var& v) const
	{
		return (getBool() || v.getBool()) ? var("1") : var("0");
	}

	// MATH OPERATORS //
	var var::operator+(const var& v) const
	{
		std::string val = value;

		if (type == "ARRAY" || v.type == "ARRAY")
		{
			if (val[val.length() - 1] == ']') val.pop_back();

			if (v.getSize() != 0 && getSize() != 0) val += ",";

			if (v.type == "ARRAY") val += v.getString().substr(1);
			else val += v.getValue() + "]";

			if (val[0] != '[') val = "[" + val;
			if (val[val.length() - 1] != ']') val += "]";
		}
		else if (type == "STRING" || v.type == "STRING")
		{
			if (v.type == "CHAR") val = "\"" + getString() + v.getChar(0) + "\"";
			else val = "\"" + getString() + v.getString() + "\"";
		}
		else if (type == "INT" || type == "CHAR")
		{
			if (v.type == "DOUBLE") val = std::to_string(getFloat() + v.getDouble());
			else if (v.type == "FLOAT") val = std::to_string(getFloat() + v.getFloat());
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
			if (v.getInt() > 0) for (int i = 0; i < v.getInt(); ++i) val += getString();
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
}
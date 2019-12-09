#include "var.h"

// Author: Bartosz Niciak

#include "utils/convert.h"

#include "lexer.h"
#include "static.h"

#include <algorithm>
#include <cmath>

namespace cll
{
	// CONSTRUCTORS //

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

	// SET METHODS //
	void var::setName(const std::string& n)
	{
		name = n;

		if (name == "") return;

		if (n.length() > 0 && isdigit(n[0])) name = "INVALID_NAME"; // Checks for illegal first digit
		else if (symbols.find_first_of(n) != std::string::npos) name = "INVALID_NAME";
		else if (std::binary_search(barewords.begin(), barewords.end(), n)) name = "INVALID_NAME";
		else if (std::binary_search(rnames.begin(), rnames.end(), n)) name = "INVALID_NAME";
	}

	void var::setType(const std::string& v)
	{
		type = Type::UNDEFINED;
		if (v == "") return;

		if (v == "inf" || v == "-inf" || v == "-nan(ind)")
		{
			type = Type::DOUBLE;
			return;
		}

		bool num = true;
		bool hex = true;
		bool bin = true;

		for (size_t i = 0; i < v.length(); ++i)
		{
			if (num && !((v[i] == '-' && i == 0) || v[i] == '.' || (v[i] == 'f' && i == v.length() - 1) || (v[i] >= '0' && v[i] <= '9'))) num = false;
			if (hex && !((v[i] == 'x' && i != v.length() - 1) || (v[i] >= 'a' && v[i] <= 'f') || (v[i] >= 'A' && v[i] <= 'F') || (v[i] >= '0' && v[i] <= '9'))) hex = false;
			if (bin && !(v[i] == 'b' || v[i] == '0' || v[i] == '1')) bin = false;
		}

		if (num) // Checks for numerical
		{
			if (v == "-" || v == "-=") type = Type::SYMBOL;
			else if (v.find('.') != std::string::npos)
			{
				if (v[v.length() - 1] == 'f') type = Type::FLOAT;
				else type = Type::DOUBLE;

				if (v == ".") type = Type::SYMBOL;
			}
			else if (v.find("f") == std::string::npos) type = Type::INT;

			return;
		}

		if (hex) // Checks for hexadecimal
		{
			if (v.substr(0, 2) == "0x" && v.substr(2).find_first_of("x") == std::string::npos)
			{
				type = Type::INT;
				return;
			}
		}

		if (bin) // Checks for binary
		{
			if (v.substr(0, 2) == "0b" && v.substr(2).find_first_of("b") == std::string::npos)
			{
				type = Type::INT;
				return;
			}
		}

		if (v.length() > 0) // Checks for string, char or array
		{
			if (v[0] == '"' && v[v.length() - 1] == '"' && v.length() > 1) type = Type::STRING;
			else if (v[0] == '\'' && v[v.length() - 1] == '\'' && v.length() > 1) type = Type::CHAR;
			else if (v[0] == '[' && v[v.length() - 1] == ']')
			{
				type = Type::ARRAY;

				size_t nests = 0, ii = v.length() - 1;
				bool string = false, chars = false;
				for (ii; ii != 0; --ii)
				{
					if (!chars && string && v[ii] == '"')
					{
						if (ii != 0 && v[ii - 1] != '\\') string = false;
						else if (ii - 1 != 0 && v[ii - 2] == '\\') string = false;
					}
					else if (!chars && !string && v[ii] == '"') string = true;

					if (!string && chars && v[ii] == '\'')
					{
						if (ii != 0 && v[ii - 1] != '\\') chars = false;
						else if (ii - 1 != 0 && v[ii - 2] == '\\') chars = false;
					}
					else if (!string && !chars && v[ii] == '\'') chars = true;

					if (string || chars) continue;

					if (v[ii] == ']') nests++;
					if (v[ii] == '[') nests--;
					if (nests == 0) break;
				}

				if (ii != 0 || nests != 1) type = Type::UNDEFINED;
			}
			else if (v[0] == '(' && v[v.length() - 1] == ')')
			{
				if (v.find(")(") == std::string::npos) type = Type::PARENTHESIS;
			}

			if (type != Type::UNDEFINED) return;
		}

		if (std::all_of(v.begin(), v.end(), ::ispunct)) type = Type::SYMBOL;
		else if (std::binary_search(barewords.begin(), barewords.end(), v)) type = Type::BARE;
	}

	void var::setValue(const std::string& v)
	{
		setType(v);

		if (type == Type::STRING) value = v;
		else if (type == Type::CHAR)
		{
			if (v.length() == 3)	{ value = v; buffor.c = int(value[1]); }
			else if (v == "'\\0'")	{ value = v; buffor.c = int('\0'); }
			else if (v == "'\\n'")	{ value = v; buffor.c = int('\n'); }
			else if (v == "'\\t'")	{ value = v; buffor.c = int('\t'); }
			else if (v == "'\\v'")	{ value = v; buffor.c = int('\v'); }
			else if (v == "'\\b'")	{ value = v; buffor.c = int('\b'); }
			else if (v == "'\\r'")	{ value = v; buffor.c = int('\r'); }
			else if (v == "'\\f'")	{ value = v; buffor.c = int('\f'); }
			else if (v == "'\\a'")	{ value = v; buffor.c = int('\a'); }
			else if (v == "'\\\\'")	{ value = v; buffor.c = int('\\'); }
			else if (v == "'\\?'")	{ value = v; buffor.c = int('\?'); }
			else if (v == "'\\''")	{ value = v; buffor.c = int('\''); }
			else if (v == "'\\\"'")	{ value = v; buffor.c = int('\"'); }
			else { value = "'\\0'"; buffor.c = int('\0'); }
		}
		else if (type == Type::INT)
		{
			if (v[0] == '0' && v.length() > 1 && v.find_first_of("bx89") == std::string::npos) buffor.i = cll::fatoi(v.substr(1).c_str(), 8);
			else if (v.substr(0, 2) == "0x" && v.length() > 2) buffor.i = cll::fatoi(v.substr(2).c_str(), 16);
			else if (v.substr(0, 2) == "0b" && v.length() > 2) buffor.i = cll::fatoi(v.substr(2).c_str(), 2);
			else if (v[0] == '0' && v.length() > 1 && v.find_first_of("89") != std::string::npos) buffor.i = cll::fatoi(v.c_str());
			else buffor.i = cll::fatoi(v.c_str());

			value = std::to_string(buffor.i);
		}
		else if (type == Type::FLOAT)
		{
			buffor.f = float(cll::fatof(v.c_str()));
			value = std::to_string(buffor.f);
		}
		else if (type == Type::DOUBLE && (v == "-nan(ind)" || v == "-inf" || v == "inf"))
		{
			buffor.d = ((v == "inf") ? 1 : -1) * std::numeric_limits<double>::infinity();
			value = (v == "inf") ? "inf" : "-inf";
		}
		else if (type == Type::DOUBLE)
		{
			buffor.d = cll::fatof(v.c_str());
			value = std::to_string(buffor.d);
		}
		else value = v;
	}

	void var::setElement(const size_t& n, const var& v)
	{
		if (value.length() < 1) return;

		size_t actual_element = 0;
		std::string ins("");
		if (value[0] == '[' || value[0] == '"' || value[0] == '\'') ins = std::string(1, value[0]);

		if (type == Type::ARRAY)
		{
			std::vector<var> buff = lexer(value.substr(1, value.length() - 2));

			for (size_t i = 0; i < buff.size(); ++i)
			{
				if (buff[i].type == Type::SYMBOL && buff[i].value == ",") actual_element++;
				else if (actual_element == n)
				{
					if (buff[i].type == Type::SYMBOL && buff[i].value == "-") continue;
					ins += v.value;
					continue;
				}

				if (actual_element == n && v.value == "") continue;

				ins += buff[i].value;
			}
		}
		else
		{
			std::string buff = getRawString();

			for (auto it = buff.begin(), end = buff.end(); it != end; ++it) 
			{
				actual_element++;

				if ((actual_element - 1) == n)
				{
					if (v.value == "") continue;

					if (v.type == Type::STRING) ins += v.getRawString();
					else if (type == Type::CHAR) ins += v.getChar(0);
					else if (type == Type::INT) ins += std::to_string(v.getInt());
					else ins += std::string(1, char(v.getInt()));
					continue;
				}

				ins += *it;
			}
		}

		if (value[value.length() - 1] == ']' || value[value.length() - 1] == '"' || value[value.length() - 1] == '\'') ins += value[value.length() - 1];
		setValue(ins);
	}

	// GET METHODS //
	long long int var::getInt() const
	{
		if (type == Type::STRING || type == Type::ARRAY) return getSize();
		else if (type == Type::CHAR) return int(buffor.c);
		else if (type == Type::INT) return buffor.i;
		else if (type == Type::DOUBLE) return int(buffor.d);
		else if (type == Type::FLOAT) return int(buffor.f);
		else return 0;
	}

	bool var::getBool() const
	{
		return (value == "0" || value == "" || getInt() == 0) ? false : true;
	}

	char var::getChar(const size_t& n) const
	{
		if (type == Type::CHAR || type == Type::INT) return char((getInt() != 27) ? getInt() : ' ');
		else if (type == Type::STRING) return (n < getSize()) ? getString()[n] : '\0';
		else return '\0';
	}

	float var::getFloat() const
	{
		if (type == Type::STRING || type == Type::ARRAY) return float(getSize());
		else if (type == Type::CHAR) return float(buffor.c);
		else if (type == Type::INT) return float(buffor.i);
		else if (type == Type::DOUBLE) return float(buffor.d);
		else if (type == Type::FLOAT) return buffor.f;
		else return 0.0f;
	}

	double var::getDouble() const
	{
		if (type == Type::STRING || type == Type::ARRAY) return double(getSize());
		else if (type == Type::CHAR) return double(buffor.c);
		else if (type == Type::INT) return double(buffor.i);
		else if (type == Type::DOUBLE) return buffor.d;
		else if (type == Type::FLOAT) return double(buffor.f);
		else return 0.0;
	}

	var var::getElement(const size_t& n) const
	{
		if (type == Type::STRING) return var("'" + std::string(1, getChar(n)) + "'");
		else if (type == Type::CHAR && n < getSize()) return var("'" + std::string(1, getChar(n)) + "'");
		else if (type == Type::ARRAY)
		{
			std::vector<var> buff = lexer(value.substr(1, value.length() - 2));
			size_t actual_element = 0;
			std::string ret("");

			for (size_t i = 0; i < buff.size(); ++i)
			{
				if (buff[i].type == Type::SYMBOL && buff[i].value == ",") actual_element++;
				else if (actual_element == n) ret += buff[i].value;
			}

			return ret;
		}
		else if (n < getSize()) return var("'" + std::string(1, value[n]) + "'");

		return var("");
	}

	std::string var::getRawString() const
	{
		if (type == Type::STRING || type == Type::CHAR) return value.substr(1, value.length() - 2);
		else return value;
	}

	std::string var::getString() const
	{
		if (type == Type::STRING)
		{
			std::string ret = "";
			bool force = false;

			for (size_t i = 1; i < value.length() - 1; ++i)
			{
				if (value[i] == '\\' && value[i - 1] != '\\') continue;
				if (i > 2 && value[i - 1] == '\\' && value[i - 2] == '\\') force = true;

				if (value[i - 1] != '\\' || force) ret += value[i];
				else ret += var("'\\" + std::string(1, value[i]) + "'").getInt();

				force = false;
			}

			return ret;
		}
		else if (type == Type::CHAR) return value.substr(1, value.length() - 2);
		else return value;
	}

	std::string var::getError() const
	{
		if (name == "INVALID_NAME") return name;
		else if (value == "INVALID_VALUE") return value;
		else return "";
	}

	size_t var::getSubscript() const
	{
		size_t nests = 0, ii = value.length() - 1;
		bool string = false, chars = false;
		for (ii; ii != 0; --ii)
		{
			if (!chars && string && value[ii] == '"')
			{
				if (ii != 0 && value[ii - 1] != '\\') string = false;
				else if (ii - 1 != 0 && value[ii - 2] == '\\') string = false;
			}
			else if (!chars && !string && value[ii] == '"') string = true;

			if (!string && chars && value[ii] == '\'')
			{
				if (ii != 0 && value[ii - 1] != '\\') chars = false;
				else if (ii - 1 != 0 && value[ii - 2] == '\\') chars = false;
			}
			else if (!string && !chars && value[ii] == '\'') chars = true;

			if (string || chars) continue;

			if (value[ii] == ']') nests++;
			if (value[ii] == '[') nests--;
			if (nests == 0) break;
		}

		return ii;
	}

	size_t var::getSize() const
	{
		if (type == Type::ARRAY)
		{
			if (value == "[]") return 0;

			std::vector<var> buff = lexer(value.substr(1, value.length() - 2));
			size_t size = 0;

			for (size_t i = 0; i < buff.size(); ++i)
			{
				if (buff[i].type == Type::SYMBOL && buff[i].value == ",") size++;
			}

			return (size + 1);
		}
		if (type == Type::STRING || type == Type::CHAR) return (getString().length());
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
		bool state = true;

		if (type == Type::STRING)
		{
			if (v.type == Type::STRING) state = (getRawString() == v.getRawString());
			else if (v.type == Type::DOUBLE) state = (getFloat() == v.getDouble());
			else if (v.type == Type::FLOAT) state = (getFloat() == v.getFloat());
			else state = (getInt() == v.getInt());
		}
		else if (type == Type::INT || type == Type::CHAR)
		{
			if (v.type == Type::DOUBLE) state = (getFloat() == v.getDouble());
			else if (v.type == Type::FLOAT) state = (getFloat() == v.getFloat());
			else state = (getInt() == v.getInt());
		}
		else if (type == Type::FLOAT)
		{
			if (v.type == Type::DOUBLE) state = (getFloat() == v.getDouble());
			else if (v.type == Type::FLOAT) state = (getFloat() == v.getFloat());
			else state = (getFloat() == v.getInt());
		}
		else if (type == Type::DOUBLE)
		{
			if (v.type == Type::DOUBLE) state = (getDouble() == v.getDouble());
			else if (v.type == Type::FLOAT) state = (getDouble() == v.getFloat());
			else state = (getDouble() == v.getInt());
		}
		else if (type == Type::ARRAY)
		{
			if (v.type == Type::ARRAY) state = (getSize() == v.getSize());
			else if (v.type == Type::DOUBLE) state = (getFloat() == v.getDouble());
			else if (v.type == Type::FLOAT) state = (getFloat() == v.getFloat());
			else state = (getInt() == v.getInt());
		}

		return (state) ? var("1") : var("0");
	}

	var var::operator!=(const var& v) const
	{
		return ((var(value) == v).value == "1") ? var("0") : var("1");
	}

	var var::operator>(const var& v) const
	{
		bool state = true;

		if (type == Type::STRING)
		{
			if (v.type == Type::STRING) state = (getRawString() > v.getRawString());
			else if (v.type == Type::DOUBLE) state = (getFloat() > v.getDouble());
			else if (v.type == Type::FLOAT) state = (getFloat() > v.getFloat());
			else state = (getInt() > v.getInt());
		}
		else if (type == Type::INT || type == Type::CHAR)
		{
			if (v.type == Type::DOUBLE) state = (getFloat() > v.getDouble());
			else if (v.type == Type::FLOAT) state = (getFloat() > v.getFloat());
			else state = (getInt() > v.getInt());
		}
		else if (type == Type::FLOAT)
		{
			if (v.type == Type::DOUBLE) state = (getFloat() > v.getDouble());
			else if (v.type == Type::FLOAT) state = (getFloat() > v.getFloat());
			else state = (getFloat() > v.getInt());
		}
		else if (type == Type::DOUBLE)
		{
			if (v.type == Type::DOUBLE) state = (getDouble() > v.getDouble());
			else if (v.type == Type::FLOAT) state = (getDouble() > v.getFloat());
			else state = (getDouble() > v.getInt());
		}
		else if (type == Type::ARRAY)
		{
			if (v.type == Type::ARRAY) state = (getSize() > v.getSize());
			else if (v.type == Type::DOUBLE) state = (getFloat() > v.getDouble());
			else if (v.type == Type::FLOAT) state = (getFloat() > v.getFloat());
			else state = (getInt() > v.getInt());
		}

		return (state) ? var("1") : var("0");
	}

	var var::operator<(const var& v) const
	{
		bool state = true;

		if (type == Type::STRING)
		{
			if (v.type == Type::STRING) state = (getRawString() < v.getRawString());
			else if (v.type == Type::DOUBLE) state = (getFloat() < v.getDouble());
			else if (v.type == Type::FLOAT) state = (getFloat() < v.getFloat());
			else state = (getInt() < v.getInt());
		}
		else if (type == Type::INT || type == Type::CHAR)
		{
			if (v.type == Type::DOUBLE) state = (getFloat() < v.getDouble());
			else if (v.type == Type::FLOAT) state = (getFloat() < v.getFloat());
			else state = (getInt() < v.getInt());
		}
		else if (type == Type::FLOAT)
		{
			if (v.type == Type::DOUBLE) state = (getFloat() < v.getDouble());
			else if (v.type == Type::FLOAT) state = (getFloat() < v.getFloat());
			else state = (getFloat() < v.getInt());
		}
		else if (type == Type::DOUBLE)
		{
			if (v.type == Type::DOUBLE) state = (getDouble() < v.getDouble());
			else if (v.type == Type::FLOAT) state = (getDouble() < v.getFloat());
			else state = (getDouble() < v.getInt());
		}
		else if (type == Type::ARRAY)
		{
			if (v.type == Type::ARRAY) state = (getSize() < v.getSize());
			else if (v.type == Type::DOUBLE) state = (getFloat() < v.getDouble());
			else if (v.type == Type::FLOAT) state = (getFloat() < v.getFloat());
			else state = (getInt() < v.getInt());
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

		if (type == Type::ARRAY || v.type == Type::ARRAY)
		{
			val.reserve(val.length() + v.value.length() + 2);

			if (val[val.length() - 1] == ']') val.pop_back();

			if (v.getSize() != 0 && getSize() != 0) val += ",";
			else if (v.type == Type::STRING && getSize() != 0) val += ",";

			if (v.type == Type::ARRAY) val += v.getRawString().substr(1);
			else val += v.getValue() + "]";

			if (val[0] != '[') val = "[" + val;
			if (val[val.length() - 1] != ']') val += "]";
		}
		else if (type == Type::STRING || v.type == Type::STRING) val = "\"" + getRawString() + v.getRawString() + "\"";
		else if (type == Type::INT || type == Type::CHAR)
		{
			if (v.type == Type::DOUBLE) val = std::to_string(getInt() + v.getDouble());
			else if (v.type == Type::FLOAT)
			{
				val = std::to_string(getFloat() + v.getFloat());
				if (val.find('.') == std::string::npos) val += ".0f";
				else val += "f";
			}
			else val = std::to_string(getInt() + v.getInt());

			if (type == Type::CHAR && v.type == Type::INT) val = "'" + std::string(1, var(val).getChar(0)) + "'";
		}
		else if (type == Type::FLOAT)
		{
			if (v.type == Type::DOUBLE) val = std::to_string(getFloat() + v.getDouble());
			else if (v.type == Type::FLOAT) val = std::to_string(getFloat() + v.getFloat());
			else val = std::to_string(getFloat() + v.getInt());

			if (v.type != Type::DOUBLE)
			{
				if (val.find('.') == std::string::npos) val += ".0f";
				else val += "f";
			}
		}
		else if (type == Type::DOUBLE)
		{
			if (v.type == Type::DOUBLE) val = std::to_string(getDouble() + v.getDouble());
			else if (v.type == Type::FLOAT) val = std::to_string(getDouble() + v.getFloat());
			else val = std::to_string(getDouble() + v.getInt());
		}

		return var(val);
	}

	var var::operator-(const var& v) const
	{
		std::string val = value;

		if (type == Type::STRING) val = getValue();
		else if (type == Type::INT || type == Type::CHAR)
		{
			if (v.type == Type::DOUBLE) val = std::to_string(getInt() - v.getDouble());
			else if (v.type == Type::FLOAT)
			{
				val = std::to_string(getFloat() - v.getFloat());
				if (val.find('.') == std::string::npos) val += ".0f";
				else val += "f";
			}
			else val = std::to_string(getInt() - v.getInt());

			if (type == Type::CHAR && v.type == Type::INT) val = "'" + std::string(1, var(val).getChar(0)) + "'";
		}
		else if (type == Type::FLOAT)
		{
			if (v.type == Type::DOUBLE) val = std::to_string(getFloat() - v.getDouble());
			else if (v.type == Type::FLOAT) val = std::to_string(getFloat() - v.getFloat());
			else val = std::to_string(getFloat() - v.getInt());

			if (v.type != Type::DOUBLE)
			{
				if (val.find('.') == std::string::npos) val += ".0f";
				else val += "f";
			}
		}
		else if (type == Type::DOUBLE)
		{
			if (v.type == Type::DOUBLE) val = std::to_string(getDouble() - v.getDouble());
			else if (v.type == Type::FLOAT) val = std::to_string(getDouble() - v.getFloat());
			else val = std::to_string(getDouble() - v.getInt());
		}

		return var(val);
	}

	var var::operator*(const var& v) const
	{
		std::string val = value;

		if (type == Type::ARRAY || v.type == Type::ARRAY)
		{
			var buff; 

			if (type == Type::ARRAY)
			{
				buff = val;
				for (size_t i = 1; i < size_t(v.getInt()); ++i) buff += getValue();
				if (v.getInt() == 0) buff.value = "[]";
			}
			else
			{
				buff = v.value;
				for (size_t i = 1; i < size_t(getInt()); ++i) buff += v.getValue();
				if (getInt() == 0) buff.value = "[]";
			}

			val = buff.getValue();
		}
		else if (type == Type::STRING || v.type == Type::STRING)
		{
			std::string buff = (type == Type::STRING) ? getRawString() : v.getRawString();
			val = '"' + buff;
			if (v.getInt() > 0) for (int i = 1; i < v.getInt(); ++i) val += buff;
			val += '"';
		}
		else if (type == Type::INT || type == Type::CHAR)
		{
			if (v.type == Type::DOUBLE) val = std::to_string(getInt() * v.getDouble());
			else if (v.type == Type::FLOAT)
			{
				val = std::to_string(getInt() * v.getFloat());
				if (val.find('.') == std::string::npos) val += ".0f";
				else val += "f";
			}
			else val = std::to_string(getInt() * v.getInt());

			if (type == Type::INT && v.type == Type::INT) val = std::to_string(var(val).getInt());
		}
		else if (type == Type::FLOAT)
		{
			if (v.type == Type::DOUBLE) val = std::to_string(getFloat() * v.getDouble());
			else if (v.type == Type::FLOAT) val = std::to_string(getFloat() * v.getFloat());
			else val = std::to_string(getFloat() * v.getInt());

			if (v.type != Type::DOUBLE)
			{
				if (val.find('.') == std::string::npos) val += ".0f";
				else val += "f";
			}
		}
		else if (type == Type::DOUBLE)
		{
			if (v.type == Type::DOUBLE) val = std::to_string(getDouble() * v.getDouble());
			else if (v.type == Type::FLOAT) val = std::to_string(getDouble() * v.getFloat());
			else val = std::to_string(getDouble() * v.getInt());
		}

		return var(val);
	}

	var var::operator/(const var& v) const
	{
		std::string val = value;

		if (type == Type::STRING) val = getRawString();
		else if (type == Type::INT || type == Type::CHAR)
		{
			if (v.type == Type::DOUBLE && v.getDouble() != 0) val = std::to_string(getInt() / v.getDouble());
			else if (v.type == Type::FLOAT && v.getFloat() != 0)
			{
				val = std::to_string(getInt() / v.getFloat());
				if (val.find('.') == std::string::npos) val += ".0f";
				else val += "f";
			}
			else if (v.getInt() != 0)
			{
				int b1 = getInt();
				float b2 = v.getFloat();

				if (b1 / b2 == b1 / v.getInt()) val = std::to_string(b1 / int(b2));
				else val = std::to_string(b1 / b2);
			}
			else val = "inf";
		}
		else if(type == Type::FLOAT)
		{
			if (v.type == Type::DOUBLE && v.getDouble() != 0) val = std::to_string(getFloat() / v.getDouble());
			else if (v.type == Type::FLOAT && v.getFloat() != 0) val = std::to_string(getFloat() / v.getFloat());
			else if (v.getInt() != 0) val = std::to_string(getFloat() / v.getInt());

			if (v.type != Type::DOUBLE)
			{
				if (val.find('.') == std::string::npos) val += ".0f";
				else val += "f";
			}

			if (v.getInt() == 0) val = "inf";
		}
		else if (type == Type::DOUBLE)
		{
			if (v.type == Type::DOUBLE && v.getDouble() != 0) val = std::to_string(getDouble() / v.getDouble());
			else if (v.type == Type::FLOAT && v.getFloat() != 0) val = std::to_string(getDouble() / v.getFloat());
			else if (v.getInt() != 0) val = std::to_string(getDouble() / v.getInt());
			else val = "inf";
		}

		return var(val);
	}

	var var::operator%(const var& v) const
	{
		return (v.getInt() != 0) ? std::to_string(getInt() % v.getInt()) : "inf";
	}

	var var::pow(const var& v) const
	{
		std::string val = value;

		if (type == Type::STRING) val = getRawString();
		else if (type == Type::INT || type == Type::CHAR)
		{
			if (v.type == Type::DOUBLE) val = std::to_string(std::pow(getFloat(), v.getDouble()));
			else if (v.type == Type::FLOAT)
			{
				val = std::to_string(std::powf(getFloat(), v.getFloat()));
				if (val.find('.') == std::string::npos) val += ".0f";
				else val += "f";
			}
			else val = std::to_string(int(std::pow(getInt(), v.getInt())));
		}
		else if (type == Type::FLOAT)
		{
			if (v.type == Type::DOUBLE) val = std::to_string(std::pow(getFloat(), v.getDouble()));
			else val = std::to_string(std::powf(getFloat(), v.getFloat()));

			if (v.type != Type::DOUBLE)
			{
				if (val.find('.') == std::string::npos) val += ".0f";
				else val += "f";
			}
		}
		else if (type == Type::DOUBLE)
		{
			if (v.type == Type::DOUBLE) val = std::to_string(std::pow(getDouble(), v.getDouble()));
			else if (v.type == Type::FLOAT) val = std::to_string(std::pow(getDouble(), v.getFloat()));
			else val = std::to_string(std::pow(getDouble(), v.getInt()));
		}

		return var(val);
	}

	// BITWISE OPERATORS //

	var var::operator^(const var& v) const
	{
		return std::to_string(getInt() ^ v.getInt());
	}

	var var::operator&(const var& v) const
	{
		return std::to_string(getInt() & v.getInt());
	}

	var var::operator|(const var& v) const
	{
		return std::to_string(getInt() | v.getInt());
	}

	var var::operator<<(const var& v) const
	{
		return std::to_string(getInt() << v.getInt());
	}

	var var::operator>>(const var& v) const
	{
		return std::to_string(getInt() >> v.getInt());
	}
}
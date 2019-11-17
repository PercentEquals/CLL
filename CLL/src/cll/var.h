#pragma once

// Author: Bartosz Niciak

#include <iostream>
#include <string>

namespace cll
{
	enum class Type
	{
		UNDEFINED, INT, FLOAT, DOUBLE, CHAR, STRING, ARRAY, PARENTHESIS, SYMBOL, BARE
	};

	union Buffor
	{
		long long int i;
		double d;
		float f;
		char c;

		Buffor() : i(0) {};
	};

	struct var
	{
		std::string name; // Holds declared name of variable
		std::string value; // Holds non-raw value of variable - meaning its value representation depends on detected type
		Type type;
		Buffor buffor;

		// CONSTRUCTORS //
		var() : name(""), value(""), type(Type::UNDEFINED) { value.reserve(100); };
		var(const var& v) : name(v.name), value(v.value), type(v.type), buffor(v.buffor) {}
		var(const std::string& n, const var& v) : value(v.value), type(v.type), buffor(v.buffor) { setName(n); };
		var(const std::string& v);
		var(const std::string& n, const std::string& v);

		// SET METHODS //
		void setName(const std::string& n); // Sets variable name and checks for special symbols and bare words
		void setType(const std::string& v); // Sets types of variable based on its value
		void setValue(const std::string& v); // Sets value of variable with a little tweaks
		void setElement(const size_t& n, const var& v);

		// DELETE METHODS
		__forceinline void deleteElement(const size_t& n) { setElement(n, var("")); };

		// GET METHODS //
		__forceinline std::string getName() const { return name; };
		__forceinline std::string getValue() const { return value; };
		std::string getType() const
		{
			switch (type)
			{
				case Type::INT: return "INT";
				case Type::BARE: return "BARE";
				case Type::CHAR: return "CHAR";
				case Type::FLOAT: return "FLOAT";
				case Type::ARRAY: return "ARRAY";
				case Type::DOUBLE: return "DOUBLE";
				case Type::STRING: return "STRING";
				case Type::SYMBOL: return "SYMBOL";
				case Type::UNDEFINED: return "UNDEFINED";
				case Type::PARENTHESIS: return "PARENTHESIS";
				default: return "UNDEFINED";
			}
		};

		long long int getInt() const;
		bool getBool() const;
		char getChar(const size_t& n) const;
		float getFloat() const;
		double getDouble() const;
		var getElement(const size_t& n) const;
		std::string getString() const;
		std::string getEscapedString() const;
		std::string getError() const;
		size_t getSubscript() const;
		size_t getSize() const;

		// OPERATORS //
		friend std::ostream& operator<<(std::ostream& out, const var& v);
		friend std::istream& operator>>(std::istream& in, var& v);

		// BOOLEAN OPERATORS //
		var operator~() const;
		var operator!() const;
		var operator==(const var& v) const;
		var operator!=(const var& v) const;
		var operator>(const var& v) const;
		var operator<(const var& v) const;
		var operator>=(const var& v) const;
		var operator<=(const var& v) const;
		var operator&&(const var& v) const;
		var operator||(const var& v) const;

		// MATH OPERATORS //
		var operator+(const var& v) const;
		var operator-(const var& v) const;
		var operator*(const var& v) const;
		var operator/(const var& v) const;
		var operator%(const var& v) const;

		var pow(const var& v) const;

		// BITWISE OPERATORS //
		var operator<<(const var& v) const;
		var operator>>(const var& v) const;
		var operator^(const var& v) const;
		var operator&(const var& v) const;
		var operator|(const var& v) const;

		// INLINE ASSIGNMENT OPERATORS // 
		__forceinline var& operator+=(const var& v) { *this = *this + v; return *this; };
		__forceinline var& operator-=(const var& v) { *this = *this - v; return *this; };
		__forceinline var& operator*=(const var& v) { *this = *this * v; return *this; };
		__forceinline var& operator/=(const var& v) { *this = *this / v; return *this; };
		__forceinline var& operator%=(const var& v) { *this = *this % v; return *this; };
		__forceinline var& operator^=(const var& v) { *this = *this ^ v; return *this; };
		__forceinline var& operator&=(const var& v) { *this = *this & v; return *this; };
		__forceinline var& operator|=(const var& v) { *this = *this | v; return *this; };
		__forceinline var& operator<<=(const var& v) { *this = *this << v; return *this; };
		__forceinline var& operator>>=(const var& v) { *this = *this >> v; return *this; };
	};
}
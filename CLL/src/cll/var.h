#pragma once

// Author: Bartosz Niciak

#include <iostream>
#include <string>

// Contains var struct that acts as a dynamic variable and also as a token (all tokens are variables in CLL).
// Every object of it contains information such as : name, value, type (as enum).
//
// Every variable value is being held in a string, but primitive types like:
// int, float, double and char have also their own union that holds converted value of a variable.
// This aproach allows for dynamic variables and for multiple type arrays.
// Main disadvantage of this aproach is its speed.
//
// Variable type is being held in a simple enum.
// This offers a good code readability with minimal overhead.

namespace cll
{
	// Enum that holds active type of a variable
	enum class Type
	{
		UNDEFINED, INT, FLOAT, DOUBLE, CHAR, STRING, ARRAY, PARENTHESIS, SYMBOL, BARE
	};

	// Union that holds converted values so that they can be accessed faster without additional conversions
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
		std::string name;
		std::string value;
		Type type;
		Buffor buffor;

		// CONSTRUCTORS //
		var() : name(""), value(""), type(Type::UNDEFINED) { value.reserve(100); };
		var(const var& v) : name(v.name), value(v.value), type(v.type), buffor(v.buffor) {}
		var(const std::string& n, const var& v) : value(v.value), type(v.type), buffor(v.buffor) { setName(n); };
		var(const std::string& v);
		var(const std::string& n, const std::string& v);

		// IS METHODS //
		bool isFunction() const;

		// SET METHODS //
		void setName(const std::string& n); // Sets variable name and checks for special symbols and bare words
		void setType(const std::string& v); // Sets types of variable based on its value
		void setValue(const std::string& v); // Sets value, buffor and type of variable based on its value
		bool setElement(const size_t& n, const var& v); // Sets value for specific element in array/vector

		// DELETE METHODS
		inline void deleteElement(const size_t& n) { setElement(n, var("")); };

		// GET METHODS //
		inline std::string getName() const { return name; };
		inline std::string getValue() const { return value; };
		std::string getType() const;

		long long int getInt() const;
		bool getBool() const;
		char getChar(const size_t& n) const;
		float getFloat() const;
		double getDouble() const;
		var getElement(const size_t& n) const;
		std::string getRawString() const; // Returns value as string with escape represented
		std::string getString() const; // Returns value as string with escape characters acting as they should
		std::string getError() const; // Returns non-empty string when some error occured
		size_t getSubscript() const; // Returns position of right most square bracket that opens a subscript
		size_t getSize() const;

		// OPERATORS //
		friend std::ostream& operator<<(std::ostream& out, const var& v);
		friend std::istream& operator>>(std::istream& in, var& v);

		// BOOLEAN OPERATORS //
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
		var operator~() const;

		// INLINE ASSIGNMENT OPERATORS // 
		inline var& operator+=(const var& v) { *this = *this + v; return *this; };
		inline var& operator-=(const var& v) { *this = *this - v; return *this; };
		inline var& operator*=(const var& v) { *this = *this * v; return *this; };
		inline var& operator/=(const var& v) { *this = *this / v; return *this; };
		inline var& operator%=(const var& v) { *this = *this % v; return *this; };
		inline var& operator^=(const var& v) { *this = *this ^ v; return *this; };
		inline var& operator&=(const var& v) { *this = *this & v; return *this; };
		inline var& operator|=(const var& v) { *this = *this | v; return *this; };
		inline var& operator<<=(const var& v) { *this = *this << v; return *this; };
		inline var& operator>>=(const var& v) { *this = *this >> v; return *this; };
	};
}
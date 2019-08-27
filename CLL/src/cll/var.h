#pragma once

// Author: Bartosz Niciak

#include <iostream>
#include <string>

namespace cll
{
	struct var
	{
		std::string name; // Holds declared name of variable
		std::string value; // Holds non-raw value of variable - meaning its value representation depends on detected type
		std::string type; // Holds variable type - (LONG LONG) INT, FLOAT, DOUBLE, CHAR, STRING, UNDEFINED, ARRAY, PARENTHESIS

		// CONSTRUCTORS //
		var() : name(""), value(""), type("") {};
		var(const var& v) : name(v.name), value(v.value), type(v.type) {};
		var(const std::string& n, const var& v) : value(v.value), type(v.type) { setName(n); };
		var(const std::string& v);
		var(const std::string& n, const std::string& v);
		var(const std::string& n, const std::string& v, const std::string& t) : value(v), type(t) { setName(n); };

		// SET METHODS //
		void setName(const std::string& n); // Sets variable name and checks for special symbols and bare words
		void setType(const std::string& v); // Sets types of variable based on its value
		void setValue(const std::string& v); // Sets value of variable with a little tweaks
		void setElement(const size_t& n, const var& v);

		// DELETE METHODS
		inline void deleteElement(const size_t& n) { setElement(n, var("")); };

		// GET METHODS //
		inline std::string getName() const { return name; };
		inline std::string getType() const { return type; };
		inline std::string getValue() const { return value; };

		long long int getInt() const;
		bool getBool() const;
		char getChar(const size_t& n) const;
		float getFloat() const;
		double getDouble() const;
		var getElement(const size_t& n) const;
		std::string getString() const;
		std::string getError() const;
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
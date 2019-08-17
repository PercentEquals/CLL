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
		std::string type; // Holds variable type - (LONG LONG) INT, FLOAT, DOUBLE, CHAR, STRING, BOOL, UNDEFINED, ARRAY, PARENTHESIS

		// CONSTRUCTORS //
		var() : name(""), value(""), type("") {}; // Empty constructor
		var(const var& v) { name = v.name; value = v.value; type = v.type; }; // Copy constructor
		var(const std::string& n, const var& v) { setName(n); value = v.value; type = v.type; }; // Copy constructor with custom name
		var(const std::string& v) { setName(""); setValue(v); }; // Value only constructor
		var(const std::string& n, const std::string& v) { setName(n); setValue(v); }; // "Safe" constructor
		var(const std::string& n, const std::string& v, const std::string& t) { setName(n); value = v; type = t; }; // Forced value and type constructor

		// SET METHODS //
		void setName(const std::string& n); // Sets variable name and checks for special symbols and bare words
		void setType(const std::string& v); // Sets types of variable based on its value
		void setValue(const std::string& v); // Sets value of variable with a little tweaks
		void setElement(const size_t& n, const var& v);

		// DELETE METHODS
		void deleteElement(const size_t& n);

		// GET METHODS //
		std::string getName() const { return name; };
		std::string getType() const { return type; };

		long long int getInt() const;
		bool getBool() const;
		char getChar(const size_t& n) const;
		float getFloat() const;
		double getDouble() const;
		var getElement(const size_t& n) const;
		std::string getString() const;
		std::string getValue() const;
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
	};
}
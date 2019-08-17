#pragma once

// Author: Bartosz Niciak

#include <string>
#include <vector>

namespace cll
{
	// Vector of bare words
	const std::vector<std::string> barewords =
	{
		"cll",
		"delete",
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

	// Vector of multiple char operators (must be sorted by char count - from highest to lowest)
	const std::vector<std::string> multi_symbols =
	{
		">>=", "<<=", "!==", "===",
		">=", "<=", "==", "!=",
		"**", "&&", "||", "<<", ">>",
		"+=", "-=", "/=", "*=", "%=",
		"|=", "&=", "^="
	};

	// Vector of math operators
	const std::vector<std::string> math_symbols =
	{
		"+=", "-=", "/=", "*=", "%=", "|=", "&=", "^=",
		">", "<", "+", "-", "*", "/", "%", "^", "&", "|",
		"==", "!=", ">=", "<=", "**", "&&", "||", "<<", ">>",
		"===", "!=="
	};

	// Symbols that are illegal in variable name
	const std::string symbols = "~{}/,.<>\\|&*!@#$%^&*()+-=;':\"?";

	// Symbols that create new token
	const std::string lexer_symbols = " ~{}/,<>\\|&*!@#$%^&*+-=;:\?";
}
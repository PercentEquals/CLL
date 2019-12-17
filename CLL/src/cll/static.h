#pragma once

// Author: Bartosz Niciak

#include <string>
#include <vector>

// Contains literals that are used by Interpreter class methods and lexer such as:
// list of barewords, restricted variable names, multiple character operators,
// symbols that are illegal in variable name and symbols.

namespace cll
{
	// Vector of bare words
	const std::vector<std::string> barewords =
	{
		"break",
		"cin",		
		"cll",
		"continue",
		"cout",
		"delete",
		"do",
		"else",
		"for",
		"function",
		"if",
		"include",
		"return",
		"while"
	};

	// Vector of restricted names
	const std::vector<std::string> rnames =
	{
		"-inf", "inf", "INVALID_NAME", "INVALID_VALUE", "UNDEFINED"
	};

	// Vector of multiple char operators (must be sorted by char count - from highest to lowest)
	const std::vector<std::string> multi_symbols =
	{
		">>=", "<<=", "!==", "===",
		">=", "<=", "==", "!=",
		"**", "&&", "||", "<<", ">>",
		"+=", "-=", "/=", "*=", "%=",
		"|=", "&=", "^=", "//"
	};

	// Symbols that are illegal in variable name
	const std::string symbols = "`~{}/,.<>\\|&*!@#$%^&()+-=;':\"?";

	// Symbols that create new token
	const std::string lexer_symbols = " `~{}/,<>\\|&*!@#$%^&+-=;:\?";
}
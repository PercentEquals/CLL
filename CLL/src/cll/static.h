#pragma once

// Author: Bartosz Niciak

#include <string>
#include <vector>

namespace cll
{
	// Vector of bare words
	const std::vector<std::string> barewords =
	{
		"cin",		
		"cll",
		"cout",
		"delete",
		"do",
		"else",
		"if",
		"pause",
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

	// Vector of math operators
	const std::vector<std::string> math_symbols =
	{
		"+=", "==", "===", ">", "!=", "!==", "%", 
		"%=", "&", "&&", "&=", "*", "**", "*=",
		"+", "-", "-=", "/", "/=", "<", "<<", "<=", 
		">=", ">>", "^", "^=", "|", "|=", "||"
	};

	// Symbols that are illegal in variable name
	const std::string symbols = "`~{}/,.<>\\|&*!@#$%^&()+-=;':\"?";

	// Symbols that create new token
	const std::string lexer_symbols = " `~{}/,<>\\|&*!@#$%^&+-=;:\?";
}
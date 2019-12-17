#pragma once

// Author: Bartosz Niciak

#include "var.h"

#include <vector>

// Contains function that interpretes string as a tokens.
// All tokens in CLL are variables. Type detection is done by variable struct.
// 
// This lexer is capable of detecting strings, chars, arrays, parentheses, symbols and for others.
// It can also "slice" tokens based on character detection like so: x=10 -> x = 10
//
// Precedence order in:
// 1. strings
// 2. chars
// 3. parentheses
// 4. symbols
// 5. comments
// 6. other
//
// Example of tokenization:
// - before (as string): 
//
// x="20()"+'\t'+([20][0] / 2)+[20+30]
//
// - after (as vector of variables with types):
//
// x			 UNDEFINED
// =			 SYMBOL
// "20()"		 STRING
// +			 SYMBOL
// '\t'			 CHAR
// +			 SYMBOL
// ([20][0] / 2) PARENTHESIS
// +			 SYMBOL
// [20+30]		 ARRAY
//
// To tokenize inside of parentheses or arrays lexer must be applied one more time
// to target variables without the first and last characters. 
// Here is example of this fot above parenthesis:
//
// - before (as string): [20][0] / 2
//
// - after (as vector of variables with types):
//
// [20][0]		 ARRAY
// /			 SYMBOL
// 2			 INT

namespace cll
{
	std::vector<var> lexer(const std::string& l);
}
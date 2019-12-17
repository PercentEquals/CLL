#pragma once

#include "string.h"
#include "math.h"
#include "rand.h"
#include "time.h"
#include "type.h"
#include "file.h"

// Header file for all CLL builtin functions
//
// Example of function:
// 
//	var name(const std::vector<var>& args)
//	{
//		return var("0");
//	}
//
// Example of function that returns an array:
//
//	var name(const std::vector<var>& args)
//	{
//		var ret("[]");
//		ret += var("0");
//		ret += var("1");
//		return ret;
//	}
//
// Example of function returns an array depending on 1 parameter:
//
//	var name(const std::vector<var>& args)
//	{
// 		if (args.empty()) return var("0");
//
//		var ret("[]");
//
//		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(0);
//
//		return (ret.getSize() > 1) ? ret : ret.getElement(0);
//	}
//
// Explanation: Args parameter also include a comma operator, so it is important to skip it
// Returns one variable when only 1 parameter are given, or an array if 3, 5, 7, 9 or more parameters are given (including commas)
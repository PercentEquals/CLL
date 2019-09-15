#pragma once

#include "../var.h"

#include <vector>

namespace cll
{
	var typeof(const std::vector<var>& args)
	{
		if (args.empty()) return var("\"UNDEFINED\"");
		return "\"" + args[0].getType() + "\"";
	}

	var toint(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");
		return (args[0].type == INT) ? args[0] : std::to_string(args[0].getInt());
	}

	var tofloat(const std::vector<var>& args)
	{
		if (args.empty()) return var("0.0f");
		return (args[0].type == FLOAT) ? args[0] : std::to_string(args[0].getFloat());
	}

	var todouble(const std::vector<var>& args)
	{
		if (args.empty()) return var("0.0");
		return (args[0].type == DOUBLE) ? args[0] : std::to_string(args[0].getDouble());
	}

	var tochar(const std::vector<var>& args)
	{
		if (args.empty()) return var("'\0'");
		return var("'" + std::string(1, args[0].getChar(0)) + "'");
	}

	var tostring(const std::vector<var>& args)
	{
		if (args.empty()) return var("\"\"");
		return "\"" + args[0].getString() + "\"";
	}
}
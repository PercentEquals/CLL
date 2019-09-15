#pragma once

#include "../var.h"

#include <vector>

namespace cll
{
	var length(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");
		return std::to_string(args[0].getSize());
	}

	var sqrt(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");
		return std::to_string(std::sqrt(args[0].getDouble()));
	}

	var abs(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");
		return std::to_string(std::fabs(args[0].getDouble()));
	}

	var floor(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");
		return std::to_string(std::floor(args[0].getDouble()));
	}

	var ceil(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");
		return std::to_string(std::ceil(args[0].getDouble()));
	}

	var round(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");
		return std::to_string(std::round(args[0].getDouble()));
	}
}
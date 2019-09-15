#pragma once

#include "../var.h"

#include <fstream>
#include <vector>

namespace cll
{
	var fopen(const std::vector<var>& args)
	{
		if (args.empty()) return var("[]");
		std::fstream f(args[0].getString(), std::ios::in, std::ios::binary);
		std::string l;
		var ret("[]");

		if (f.good())
		{
			while (std::getline(f, l)) ret += var("\"" + l + "\"");
		}

		return ret;
	}

	var fwrite(const std::vector<var>& args)
	{
		if (args.size() < 2) return var("0");

		std::fstream f(args[0].getString(), std::ios::out);

		if (f.good())
		{
			for (size_t i = 0; i < args[2].value.length(); ++i) f << args[2].getElement(i).getString();
		}
		else return var("0");

		return var("1");
	}
}
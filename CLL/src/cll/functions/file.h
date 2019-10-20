#pragma once

#include "../var.h"

#include <fstream>
#include <vector>

namespace cll
{
	var fopen(const std::vector<var>& args)
	{
		if (args.empty()) return var("[]");
		std::fstream f(args[0].getString(), std::ios::in);
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
			for (size_t i = 2; i < args.size(); i += 2)
			{
				for (size_t ii = 0; ii < args[i].getSize(); ++ii)
				{
					f << args[i].getElement(ii).getString();
					if (args[i].type == Type::ARRAY) f << '\n';
				}
				if (args[i].type != Type::ARRAY) f << '\n';
			}
		}
		else return var("0");

		return var("1");
	}

	var fappend(const std::vector<var>& args)
	{
		if (args.size() < 2) return var("0");

		std::fstream f(args[0].getString(), std::ios::out | std::ios::app);

		if (f.good())
		{
			for (size_t i = 2; i < args.size(); i += 2)
			{
				for (size_t ii = 0; ii < args[i].getSize(); ++ii)
				{
					f << args[i].getElement(ii).getString();
					if (args[i].type == Type::ARRAY) f << '\n';
				}
				if (args[i].type != Type::ARRAY) f << '\n';
			}
		}
		else return var("0");

		return var("1");
	}
}
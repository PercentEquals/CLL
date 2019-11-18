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
			while (std::getline(f, l))
			{
				std::string b = "";
				for (size_t i = 0; i < l.length(); ++i)
				{
					if (l[i] == '\\') b += "\\\\";
					else if (l[i] == '"') b += "\\\"";
					else if (l[i] == '\'') b += "\\\'";
					else b += l[i];
				}
				ret += var("\"" + b + "\"");
			}
		}

		f.close();
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
					f << args[i].getElement(ii).getEscapedString();
					if (args[i].type == Type::ARRAY) f << '\n';
				}
				if (args[i].type != Type::ARRAY) f << '\n';
			}
		}
		else return var("0");

		f.close();
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

		f.close();
		return var("1");
	}

	var fexist(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2)
		{
			std::fstream f(args[i].getString(), std::ios::in);
			ret += std::to_string(f.good());
			f.close();
		}

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}
}
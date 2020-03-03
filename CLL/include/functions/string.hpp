#pragma once

#include "../var.hpp"

#include <cstring>
#include <string>
#include <vector>

namespace cll
{
	var find(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");
		if (args.size() < 3) return var("0");

		var ret("[]");

		for (size_t i = 0; i + 2 < args.size(); i += 4)
		{
			size_t buff = args[i].getRawString().find(args[i + 2].getRawString());
		
			if (args[i].type == Type::ARRAY)
			{
				buff = args[i].getSize();

				for (size_t ii = 0; ii < args[i].getSize(); ++ii)
				{
					if ((args[i].getElement(ii) == args[i + 2]).getBool())
					{
						buff = ii; break;
					}
				}
			}
			
			if (buff > args[i].getSize()) buff = args[i].getSize();

			ret += std::to_string(buff);
		}	

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	var rfind(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");
		if (args.size() < 3) return var("0");

		var ret("[]");

		for (size_t i = 0; i + 2 < args.size(); i += 4)
		{
			size_t buff = args[i].getRawString().rfind(args[i + 2].getRawString());

			if (args[i].type == Type::ARRAY)
			{
				buff = args[i].getSize();

				for (size_t ii = 0; ii < args[i].getSize(); ++ii)
				{
					if ((args[i].getElement(args[i].getSize() - 1 - ii) == args[i + 2]).getBool())
					{
						buff = ii; break;
					}
				}
			}

			if (buff > args[i].getSize()) buff = args[i].getSize();

			ret += std::to_string(buff);
		}

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	var substr(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 6)
		{
			if (i + 2 >= args.size()) ret += args[i];
			else
			{
				size_t len1 = (args[i + 2].getInt() > args[i].getRawString().length()) ? args[i].getRawString().length() : args[i + 2].getInt();
				size_t len2 = args[i].getRawString().length();

				if (i + 4 < args.size() && args[i + 4].getInt() < args[i].getRawString().length()) len2 = args[i + 4].getInt();

				ret += "\"" + args[i].getRawString().substr(len1, len2) + "\"";
			}
		}

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	var strspn(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");
		if (args.size() < 3) return var("0");

		var ret("[]");

		for (size_t i = 0; i + 2 < args.size(); i += 4)
		{
			ret += std::to_string(std::strspn(args[i].getRawString().c_str(), args[i + 2].getRawString().c_str()));
		}

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}
}
#pragma once

#include "../var.hpp"

#include <vector>
#include <random>

namespace cll
{
	var rand(const std::vector<var>& args)
	{
		double low = 0.0, high = 1.0;

		static std::random_device rd;
		static std::mt19937 random_engine;
		random_engine.seed(rd());

		if (args.empty())
		{
			std::uniform_real_distribution<double> dist(low, high);
			return std::to_string(dist(random_engine));
		}

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 4)
		{
			if (i + 2 >= args.size())
			{
				low = 0.0;
				high = args[i].getDouble();
			}
			else
			{
				low = args[i].getDouble();
				high = args[i + 2].getDouble();
			}

			std::uniform_real_distribution<double> dist(low, high);
			ret += std::to_string(dist(random_engine));
		}

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}
}
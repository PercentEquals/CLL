#pragma once

#include "../var.h"

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

		if (args.size() == 1) high = args[0].getDouble();
		else if (args.size() >= 3)
		{
			low = args[0].getDouble();
			high = args[2].getDouble();
		}

		std::uniform_real_distribution<double> dist(low, high);
		return std::to_string(dist(random_engine));
	}
}
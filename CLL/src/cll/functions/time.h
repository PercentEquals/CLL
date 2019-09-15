#pragma once

#include "../var.h"

#include <vector>
#include <chrono>
#include <thread>

namespace cll
{
	var time(const std::vector<var>& args)
	{
		auto clock = std::chrono::system_clock::now();

		if (!args.empty() && (args[0].getString() == "s" || args[0].getString() == "seconds"))
		{
			auto point = std::chrono::time_point_cast<std::chrono::seconds>(clock);
			auto epoch = point.time_since_epoch();
			return std::to_string(epoch.count());
		}
		else if (!args.empty() && (args[0].getString() == "us" || args[0].getString() == "microseconds"))
		{
			auto point = std::chrono::time_point_cast<std::chrono::microseconds>(clock);
			auto epoch = point.time_since_epoch();
			return std::to_string(epoch.count());
		}
		else if (!args.empty() && (args[0].getString() == "ns" || args[0].getString() == "nanoseconds"))
		{
			auto point = std::chrono::time_point_cast<std::chrono::nanoseconds>(clock);
			auto epoch = point.time_since_epoch();
			return std::to_string(epoch.count());
		}
		else
		{
			auto point = std::chrono::time_point_cast<std::chrono::milliseconds>(clock);
			auto epoch = point.time_since_epoch();
			return std::to_string(epoch.count());
		}
	}

	var sleep(const std::vector<var>& args)
	{
		if (!args.empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(args[0].getInt()));
			return args[0];
		}

		return var("0");
	}
}
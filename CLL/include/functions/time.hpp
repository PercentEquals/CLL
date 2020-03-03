#pragma once

#include "../var.hpp"

#include <vector>
#include <chrono>
#include <thread>

namespace cll
{
	var time(const std::vector<var>& args)
	{
		auto clock = std::chrono::system_clock::now();

		var ret("[]");

		if (args.empty())
		{
			auto point = std::chrono::time_point_cast<std::chrono::milliseconds>(clock);
			auto epoch = point.time_since_epoch();
			ret += std::to_string(epoch.count());
		}

		for (size_t i = 0; i < args.size(); i += 2)
		{
			if (args[i].getString() == "s" || args[i].getString() == "seconds")
			{
				auto point = std::chrono::time_point_cast<std::chrono::seconds>(clock);
				auto epoch = point.time_since_epoch();
				ret += std::to_string(epoch.count());
			}
			else if (args[i].getString() == "us" || args[i].getString() == "microseconds")
			{
				auto point = std::chrono::time_point_cast<std::chrono::microseconds>(clock);
				auto epoch = point.time_since_epoch();
				ret += std::to_string(epoch.count());
			}
			else if (args[i].getString() == "ns" || args[i].getString() == "nanoseconds")
			{
				auto point = std::chrono::time_point_cast<std::chrono::nanoseconds>(clock);
				auto epoch = point.time_since_epoch();
				ret += std::to_string(epoch.count());
			}
			else
			{
				auto point = std::chrono::time_point_cast<std::chrono::milliseconds>(clock);
				auto epoch = point.time_since_epoch();
				ret += std::to_string(epoch.count());
			}
		}

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
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
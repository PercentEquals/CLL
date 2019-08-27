#include "functions.h"

// Author: Bartosz Niciak

#include "utils/search.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <random>
#include <thread>
#include <chrono>
#include <ctime>

namespace cll
{
	Functions::Functions()
	{
		funs.emplace_back(function("abs", abs));
		funs.emplace_back(function("ceil", ceil));
		funs.emplace_back(function("char", tochar));
		funs.emplace_back(function("double", todouble));
		funs.emplace_back(function("float", tofloat));
		funs.emplace_back(function("floor", floor));
		funs.emplace_back(function("fopen", fopen));
		funs.emplace_back(function("fwrite", fwrite));
		funs.emplace_back(function("int", toint));
		funs.emplace_back(function("length", length));
		funs.emplace_back(function("rand", rand));
		funs.emplace_back(function("round", round));
		funs.emplace_back(function("sleep", sleep));
		funs.emplace_back(function("sqrt", sqrt));
		funs.emplace_back(function("string", tostring));
		funs.emplace_back(function("time", time));
		funs.emplace_back(function("typeof", typeof));
	}

	function Functions::get(const std::string& n)
	{
		size_t index = search(funs, n, 0, funs.size() - 1);
		if (index < funs.size()) return funs[index];
		else return function("", nullptr);
	}

	void Functions::add(const function& f)
	{
		size_t index = search(funs, f.name, 0, funs.size() - 1);
		if (index < funs.size()) funs.insert(std::upper_bound(funs.begin(), funs.end(), f, [](function a, function b) { return a.name < b.name; }), f);
	}

	void Functions::del(const std::string& n)
	{
		size_t index = search(funs, n, 0, funs.size() - 1);
		if (index < funs.size()) funs.erase(funs.begin() + index);
	}

	// BASIC FUNCTIONS

	var time(const std::vector<var>& args)
	{
		auto clock = std::chrono::system_clock::now();

		if (!args.empty() && (args[0].getString() == "s" || args[0].getString() == "seconds"))
		{
			auto point = std::chrono::time_point_cast<std::chrono::seconds>(clock);
			auto epoch = point.time_since_epoch();
			return var(std::to_string(epoch.count()));
		}
		else if (!args.empty() && (args[0].getString() == "us" || args[0].getString() == "microseconds"))
		{
			auto point = std::chrono::time_point_cast<std::chrono::microseconds>(clock);
			auto epoch = point.time_since_epoch();
			return var(std::to_string(epoch.count()));
		}
		else if (!args.empty() && (args[0].getString() == "ns" || args[0].getString() == "nanoseconds"))
		{
			auto point = std::chrono::time_point_cast<std::chrono::nanoseconds>(clock);
			auto epoch = point.time_since_epoch();
			return var(std::to_string(epoch.count()));
		}
		else
		{
			auto point = std::chrono::time_point_cast<std::chrono::milliseconds>(clock);
			auto epoch = point.time_since_epoch();
			return var(std::to_string(epoch.count()));
		}
	}

	var sleep(const std::vector<var>& args)
	{
		if (!args.empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(var(args[0]).getInt()));
			return args[0];
		}

		return var("0");
	}

	var length(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");
		return var(std::to_string(args[0].getSize()));
	}	
	
	var sqrt(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");
		return var(std::to_string(std::sqrt(args[0].getDouble())));
	}	
	
	var abs(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");
		return var(std::to_string(std::fabs(args[0].getDouble())));
	}	
	
	var floor(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");
		return var(std::to_string(std::floor(args[0].getDouble())));
	}	
	
	var ceil(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");
		return var(std::to_string(std::ceil(args[0].getDouble())));
	}	
	
	var round(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");
		return var(std::to_string(std::round(args[0].getDouble())));
	}	
	
	var fopen(const std::vector<var>& args)
	{
		if (args.empty()) return var("[]");
		std::fstream f(args[0].getString(), std::ios::in);
		std::string l;
		var ret("[]");

		if (f.good())
		{
			while (std::getline(f, l)) ret = ret + var("\"" + l + "\"");
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
	
	var typeof(const std::vector<var>& args)
	{
		if (args.empty()) return var("\"UNDEFINED\"");
		return var("\"" + var(args[0]).type + "\"");
	}	
	
	var toint(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");
		return var(std::to_string(var(args[0]).getInt()));
	}

	var tofloat(const std::vector<var>& args)
	{
		if (args.empty()) return var("0.0f");
		return var(std::to_string(var(args[0]).getFloat()));
	}	
	
	var todouble(const std::vector<var>& args)
	{
		if (args.empty()) return var("0.0");
		return var(std::to_string(var(args[0]).getDouble()));
	}	
	
	var tochar(const std::vector<var>& args)
	{
		if (args.empty()) return var("'\0'");
		return var("'" + std::string(1, var(args[0]).getChar(0)) + "'");
	}	
	
	var tostring(const std::vector<var>& args)
	{
		if (args.empty()) return var("\"\"");
		return var("\"" + var(args[0]).getString() + "\"");
	}	
	
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
		return var(std::to_string(dist(random_engine)));
	}
}
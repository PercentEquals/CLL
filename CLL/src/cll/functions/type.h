#pragma once

#include "../var.h"

#include <vector>

namespace cll
{
	var typeof(const std::vector<var>& args)
	{
		if (args.empty()) return var("\"UNDEFINED\"");
		return "\"" + args[0].getType() + "\"";
	}

	// Type casting functions //

	var toint(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");
		return (args[0].type == Type::INT) ? args[0] : std::to_string(args[0].getInt());
	}

	var tofloat(const std::vector<var>& args)
	{
		if (args.empty()) return var("0.0f");
		return (args[0].type == Type::FLOAT) ? args[0] : std::to_string(args[0].getFloat());
	}

	var todouble(const std::vector<var>& args)
	{
		if (args.empty()) return var("0.0");
		return (args[0].type == Type::DOUBLE) ? args[0] : std::to_string(args[0].getDouble());
	}

	var tochar(const std::vector<var>& args)
	{
		if (args.empty()) return var("'\0'");
		return var("'" + std::string(1, args[0].getChar(0)) + "'");
	}

	var tostring(const std::vector<var>& args)
	{
		if (args.empty()) return var("\"\"");
		return "\"" + args[0].getString() + "\"";
	}

	// String conversion functions //

	var stoi(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1)
		{
			try { return std::to_string(std::stoi(args[0].getString())); }
			catch (const std::invalid_argument&) { return var("0"); }
			catch (const std::out_of_range&) { return var("0"); }

		}
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2)
			{
				try { ret += std::to_string(std::stoi(args[i].getString())); }
				catch (const std::invalid_argument&) { ret += std::string("0"); }
				catch (const std::out_of_range&) { ret += std::string("0"); }
			}
			return ret;
		}
	}


	var stof(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1)
		{
			try { return std::to_string(std::stof(args[0].getString())); }
			catch (const std::invalid_argument&) { return var("0.0f"); }
			catch (const std::out_of_range&) { return var("0.0f"); }
			
		}
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2)
			{
				try { ret += std::to_string(std::stof(args[i].getString())); }
				catch (const std::invalid_argument&) { ret += std::string("0.0f"); }
				catch (const std::out_of_range&) { ret += std::string("0.0f"); }
			}
			return ret;
		}
	}

	var stod(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1)
		{
			try { return std::to_string(std::stod(args[0].getString())); }
			catch (const std::invalid_argument&) { return var("0.0"); }
			catch (const std::out_of_range&) { return var("0.0"); }

		}
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2)
			{
				try { ret += std::to_string(std::stod(args[i].getString())); }
				catch (const std::invalid_argument&) { ret += std::string("0.0"); }
				catch (const std::out_of_range&) { ret += std::string("0.0"); }
			}
			return ret;
		}
	}


	var to_string(const std::vector<var>& args)
	{
		if (args.empty()) return var("\"\"");

		if (args.size() == 1) return "\"" + args[0].getString() + "\"";
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += "\"" + args[i].getString() + "\"";
			return ret;
		}
	}
}
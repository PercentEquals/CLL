#pragma once

#include "../var.h"

#include <vector>

namespace cll
{
	var length(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(args[i].getSize());

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	var sqrt(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::sqrt(args[i].getDouble()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	var cbrt(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::cbrt(args[i].getDouble()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	var hypot(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");
		if (args.size() < 3) return var("0");

		var ret("[]");

		for (size_t i = 0; i + 2 < args.size(); i += 4) ret += std::to_string(std::hypot(args[i].getDouble(), args[i + 2].getDouble()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	var abs(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::fabs(args[i].getDouble()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	// Rounding functions //

	var floor(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::floor(args[i].getDouble()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	var ceil(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::ceil(args[i].getDouble()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	var round(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::round(args[i].getDouble()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	var trunc(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::trunc(args[i].getDouble()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	// Trigonometric functions //

	var cos(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::cos(args[i].getDouble()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	var sin(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::sin(args[i].getDouble()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	var tan(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::tan(args[i].getDouble()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	var acos(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::acos(args[i].getDouble()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	var asin(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::asin(args[i].getDouble()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	var atan(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::atan(args[i].getDouble()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	// Hyperbolic functions //

	var cosh(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::cosh(args[i].getDouble()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	var sinh(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::sinh(args[i].getDouble()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	var tanh(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::tanh(args[i].getDouble()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	var acosh(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::acosh(args[i].getDouble()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	var asinh(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::asinh(args[i].getDouble()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	var atanh(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::atanh(args[i].getDouble()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	// Exponential and logarithmic functions //

	var exp(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::exp(args[i].getDouble()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	var ldexp(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");
		if (args.size() < 3) return var("0");

		var ret("[]");

		for (size_t i = 0; i + 2 < args.size(); i += 4) ret += std::to_string(std::ldexp(args[i].getDouble(), args[i + 2].getInt()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	var log(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::log(args[i].getDouble()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	var log10(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::log10(args[i].getDouble()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

	var exp2(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		var ret("[]");

		for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::exp2(args[i].getDouble()));

		return (ret.getSize() > 1) ? ret : ret.getElement(0);
	}

}
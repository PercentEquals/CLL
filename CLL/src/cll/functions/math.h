#pragma once

#include "../var.h"

#include <vector>

namespace cll
{
	var length(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1) return std::to_string(args[0].getSize());
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(args[i].getSize());
			return ret;
		}
	}

	var sqrt(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1) return std::to_string(std::sqrt(args[0].getDouble()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::sqrt(args[i].getDouble()));
			return ret;
		}
	}

	var cbrt(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1) return std::to_string(std::cbrt(args[0].getDouble()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::cbrt(args[i].getDouble()));
			return ret;
		}
	}

	var hypot(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");
		if (args.size() < 3) return var("0");

		if (args.size() == 3) return std::to_string(std::hypot(args[0].getDouble(), args[2].getDouble()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 4)
			{
				if (i + 2 >= args.size()) break;
				ret += std::to_string(std::hypot(args[i].getDouble(), args[i + 2].getDouble()));
			}
			return ret;
		}
	}

	var abs(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1) return std::to_string(std::fabs(args[0].getDouble()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::fabs(args[i].getDouble()));
			return ret;
		}
	}

	// Rounding functions //

	var floor(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1) return std::to_string(std::floor(args[0].getDouble()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::floor(args[i].getDouble()));
			return ret;
		}
	}

	var ceil(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1) return std::to_string(std::ceil(args[0].getDouble()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::ceil(args[i].getDouble()));
			return ret;
		}
	}

	var round(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1) return std::to_string(std::round(args[0].getDouble()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::round(args[i].getDouble()));
			return ret;
		}
	}

	var trunc(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1) return std::to_string(std::trunc(args[0].getDouble()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::trunc(args[i].getDouble()));
			return ret;
		}
	}

	// Trigonometric functions //

	var cos(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1) return std::to_string(std::cos(args[0].getDouble()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::cos(args[i].getDouble()));
			return ret;
		}
	}

	var sin(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1) return std::to_string(std::sin(args[0].getDouble()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::sin(args[i].getDouble()));
			return ret;
		}
	}

	var tan(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1) return std::to_string(std::tan(args[0].getDouble()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::tan(args[i].getDouble()));
			return ret;
		}
	}

	var acos(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1) return std::to_string(std::acos(args[0].getDouble()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::acos(args[i].getDouble()));
			return ret;
		}
	}

	var asin(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1) return std::to_string(std::asin(args[0].getDouble()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::asin(args[i].getDouble()));
			return ret;
		}
	}

	var atan(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1) return std::to_string(std::atan(args[0].getDouble()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::atan(args[i].getDouble()));
			return ret;
		}
	}

	// Hyperbolic functions //

	var cosh(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1) return std::to_string(std::cosh(args[0].getDouble()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::cosh(args[i].getDouble()));
			return ret;
		}
	}

	var sinh(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1) return std::to_string(std::sinh(args[0].getDouble()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::sinh(args[i].getDouble()));
			return ret;
		}
	}

	var tanh(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1) return std::to_string(std::tanh(args[0].getDouble()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::tanh(args[i].getDouble()));
			return ret;
		}
	}

	var acosh(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1) return std::to_string(std::acosh(args[0].getDouble()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::acosh(args[i].getDouble()));
			return ret;
		}
	}

	var asinh(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1) return std::to_string(std::asinh(args[0].getDouble()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::asinh(args[i].getDouble()));
			return ret;
		}
	}

	var atanh(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1) return std::to_string(std::atanh(args[0].getDouble()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::atanh(args[i].getDouble()));
			return ret;
		}
	}

	// Exponential and logarithmic functions //

	var exp(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1) return std::to_string(std::exp(args[0].getDouble()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::exp(args[i].getDouble()));
			return ret;
		}
	}

	var ldexp(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");
		if (args.size() < 3) return var("0");

		if (args.size() == 3) return std::to_string(std::ldexp(args[0].getDouble(), (int)args[2].getInt()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 4)
			{
				if (i + 2 >= args.size()) break;
				ret += std::to_string(std::ldexp(args[i].getDouble(), (int)args[i + 2].getInt()));
			}
			return ret;
		}
	}

	var log(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1) return std::to_string(std::log(args[0].getDouble()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::log(args[i].getDouble()));
			return ret;
		}
	}

	var log10(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1) return std::to_string(std::log10(args[0].getDouble()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::log10(args[i].getDouble()));
			return ret;
		}
	}

	var exp2(const std::vector<var>& args)
	{
		if (args.empty()) return var("0");

		if (args.size() == 1) return std::to_string(std::exp2(args[0].getDouble()));
		else
		{
			var ret("[]");
			for (size_t i = 0; i < args.size(); i += 2) ret += std::to_string(std::exp2(args[i].getDouble()));
			return ret;
		}
	}

}
#include "functions.hpp"

// Author: Bartosz Niciak

#include "functions/header.hpp"
#include "utils/search.hpp"

#include <algorithm>

namespace cll
{
	Functions::Functions()
	{
		funs =
		{
			function("abs", cll::abs),
			function("acos", cll::acos),
			function("acosh", cll::acosh),
			function("asin", cll::asin),
			function("asinh", cll::asinh),
			function("atan", cll::atan),
			function("atanh", cll::atanh),
			function("bool", cll::tobool),
			function("cbrt", cll::cbrt),
			function("ceil", cll::ceil),
			function("char", cll::tochar),
			function("cos", cll::cos),
			function("cosh", cll::cosh),
			function("double", cll::todouble),
			function("exp", cll::exp),
			function("exp2", cll::exp2),
			function("fappend", cll::fappend),
			function("fexist", cll::fexist),
			function("find", cll::find),
			function("float", cll::tofloat),
			function("floor", cll::floor),
			function("fopen", cll::fopen),
			function("fwrite", cll::fwrite),
			function("hypot", cll::hypot),
			function("int", cll::toint),
			function("ldexp", cll::ldexp),
			function("length", cll::length),
			function("log", cll::log),
			function("log10", cll::log10),
			function("rand", cll::rand),
			function("rfind", cll::rfind),
			function("round", cll::round),
			function("sin", cll::sin),
			function("sinh", cll::sinh),
			function("sleep", cll::sleep),
			function("sqrt", cll::sqrt),
			function("stod", cll::stod),
			function("stof", cll::stof),
			function("stoi", cll::stoi),
			function("string", cll::tostring),
			function("strspn", cll::strspn),
			function("substr", cll::substr),
			function("tan", cll::tan),
			function("tanh", cll::tanh),
			function("time", cll::time),
			function("to_string", cll::to_string),
			function("trunc", cll::trunc),
			function("typeof", cll::type)
		};
	}

	function Functions::get(const std::string& n)
	{
		size_t index = search(funs, n, 0, funs.size() - 1);
		if (index < funs.size()) return funs[index];
		return function("", nullptr);
	}

	void Functions::add(const function& f)
	{
		size_t index = search(funs, f.name, 0, funs.size() - 1);
		if (index >= funs.size()) funs.insert(std::upper_bound(funs.begin(), funs.end(), f, [](function a, function b) { return a.name < b.name; }), f);
		else funs[index] = f;
	}

	void Functions::del(const std::string& n)
	{
		size_t index = search(funs, n, 0, funs.size() - 1);
		if (index < funs.size()) funs.erase(funs.begin() + index);
	}	
}
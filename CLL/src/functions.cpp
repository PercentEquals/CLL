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
			function("abs", abs),
			function("acos", acos),
			function("acosh", acosh),
			function("asin", asin),
			function("asinh", asinh),
			function("atan", atan),
			function("atanh", atanh),
			function("bool", tobool),
			function("cbrt", cbrt),
			function("ceil", ceil),
			function("char", tochar),
			function("cos", cos),
			function("cosh", cosh),
			function("double", todouble),
			function("exp", exp),
			function("exp2", exp2),
			function("fappend", fappend),
			function("fexist", fexist),
			function("find", find),
			function("float", tofloat),
			function("floor", floor),
			function("fopen", fopen),
			function("fwrite", fwrite),
			function("hypot", hypot),
			function("int", toint),
			function("ldexp", ldexp),
			function("length", length),
			function("log", log),
			function("log10", log10),
			function("rand", rand),
			function("rfind", rfind),
			function("round", round),
			function("sin", sin),
			function("sinh", sinh),
			function("sleep", sleep),
			function("sqrt", sqrt),
			function("stod", stod),
			function("stof", stof),
			function("stoi", stoi),
			function("string", tostring),
			function("strspn", strspn),
			function("substr", substr),
			function("tan", tan),
			function("tanh", tanh),
			function("time", time),
			function("to_string", to_string),
			function("trunc", trunc),
			function("typeof", typeof)
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
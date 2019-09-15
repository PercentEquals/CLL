#include "functions.h"

// Author: Bartosz Niciak

#include "functions/header.h"
#include "utils/search.h"

#include <algorithm>

namespace cll
{
	Functions::Functions()
	{
		funs =
		{
			function("abs", abs),
			function("ceil", ceil),
			function("char", tochar),
			function("double", todouble),
			function("float", tofloat),
			function("floor", floor),
			function("fopen", fopen),
			function("fwrite", fwrite),
			function("int", toint),
			function("length", length),
			function("rand", rand),
			function("round", round),
			function("sleep", sleep),
			function("sqrt", sqrt),
			function("string", tostring),
			function("time", time),
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
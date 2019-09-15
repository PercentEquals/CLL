#include "defined.h"

// Author: Bartosz Niciak

#include "utils/search.h"

#include <algorithm>

namespace cll
{
	defined Defined::get(const std::string& n)
	{
		size_t index = search(funs, n, 0, funs.size() - 1);
		if (index < funs.size()) return funs[index];
		return defined("", {});
	}

	void Defined::add(const defined& f)
	{
		size_t index = search(funs, f.name, 0, funs.size() - 1);
		if (index >= funs.size()) funs.insert(std::upper_bound(funs.begin(), funs.end(), f, [](defined a, defined b) { return a.name < b.name; }), f);
		else funs[index] = f;
	}

	void Defined::del(const std::string& n)
	{
		size_t index = search(funs, n, 0, funs.size() - 1);
		if (index < funs.size()) funs.erase(funs.begin() + index);
	}	
}
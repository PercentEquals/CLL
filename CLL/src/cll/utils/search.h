#pragma once

#include <string>
#include <vector>

namespace cll
{
	template<typename T>
	size_t search(const std::vector<T>& vec, const std::string& name, const size_t& l, const size_t& r)
	{
		if (r >= l)
		{
			size_t mid = l + (r - l) / 2;

			if (mid >= vec.size()) return vec.size();
			if (vec[mid].name == name) return mid;
			if (vec[mid].name > name) return search(vec, name, l, mid - 1);
			return search(vec, name, mid + 1, r);
		}

		return vec.size();
	}
}
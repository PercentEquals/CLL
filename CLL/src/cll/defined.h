#pragma once

// Author: Bartosz Niciak

#include "var.h"

#include <vector>

namespace cll
{
	struct defined
	{
		std::string name;
		std::vector<std::string> lines;

		defined(const std::string& n, const std::vector<std::string>& l) : name(n), lines(l) {};
	};
	
	class Defined
	{
		std::vector<defined> funs;

	public:

		Defined() {};

		defined get(const std::string& n);
		void add(const defined& f);
		void del(const std::string& n);
	};
}
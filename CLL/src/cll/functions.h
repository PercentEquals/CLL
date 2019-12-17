#pragma once

// Author: Bartosz Niciak

#include "var.h"

#include <vector>

// Contains function struct that holds function name (used in CLL) and pointer to that function.
// It also contains vector wrapper for that struct that allows for function searching, addition and deletion.
//
// Builtin function can be found in 'functions' directory.

namespace cll
{
	struct function
	{
		std::string name;
		var(*fun)(const std::vector<var>&);

		function(const std::string& n, var(*f)(const std::vector<var>&)) : name(n), fun(f) {};

		inline var exec(const std::vector<var>& args) { return fun(args); };
	};
	
	class Functions
	{
		std::vector<function> funs;

	public:

		Functions();

		function get(const std::string& n);
		void add(const function& f);
		void del(const std::string& n);
	};
}
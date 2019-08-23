#pragma once

// Author: Bartosz Niciak

#include "var.h"

#include <vector>

namespace cll
{
	struct function
	{
		std::string name;
		var(*fun)(const std::vector<var>&);

		function(const std::string& n, var(*f)(const std::vector<var>&)) : name(n), fun(f) {};
	};

	class Functions
	{
		std::vector<function> funs;
		size_t searchFun(const std::string& name, const size_t& l, const size_t& r);

	public:

		Functions();

		function get(const std::string& n);
		void add(const function& f);
		void del(const std::string& n);
	};

	// BASIC FUNCTIONS
	var time(const std::vector<var>& args);
	var sleep(const std::vector<var>& args);
	var length(const std::vector<var>& args);
	var sqrt(const std::vector<var>& args);
	var abs(const std::vector<var>& args);
	var floor(const std::vector<var>& args);
	var ceil(const std::vector<var>& args);
	var round(const std::vector<var>& args);
	var fopen(const std::vector<var>& args);
	var typeof(const std::vector<var>& args);
	var toint(const std::vector<var>& args);
	var tofloat(const std::vector<var>& args);
	var todouble(const std::vector<var>& args);
	var tochar(const std::vector<var>& args);
	var tostring(const std::vector<var>& args);
	var rand(const std::vector<var>& args);
}
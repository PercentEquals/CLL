#pragma once

// Author: Bartosz Niciak

#include "var.h"

#include <vector>

namespace cll
{
	std::vector<var> lexer(const std::string& l);
}
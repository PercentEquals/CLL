#pragma once

namespace cll
{
	size_t strspn(const char* string, const char* chars) 
	{
		size_t n;
		const char* p;
		for (n = 0; *string; string++, n++)
		{
			for (p = chars; *p && *p != *string; p++) {};
			if (!*p) break;
		}
		return n;
	}
}
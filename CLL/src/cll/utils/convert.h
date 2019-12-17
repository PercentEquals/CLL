#pragma once

namespace cll
{
	long long int fatoi(const char* str, const unsigned int& base = 10)
	{
		long long int val = 0;
		int sign = 1;

		while (*str)
		{
			if (*str == '-') sign = -1;

			if (*str >= '0' && *str <= '9') val = val * base + (*str++ - '0');
			else if (*str >= 'a' && *str <= 'z') val = val * base + (*str++ - 'a' + 10);
			else if (*str >= 'A' && *str <= 'Z') val = val * base + (*str++ - 'A' + 10);
			else str++;
		}

		return sign * val;
	}

	double fatof(const char* str)
	{
		double sign = 1.0;
		double pow = 10.0;
		double val = 0;

		if (*str == '-')
		{
			sign = -1.0;
			str++;
		}

		while(*str >= '0' && *str <= '9') val = val * 10.0 + (*str++ - '0');

		if (*str++ == '.')
		{
			while ((*str) >= '0' && (*str) <= '9') 
			{
				val += (*str++ - '0') / pow;
				pow *= 10.0;
			}
		}

		return sign * val;
	}

	std::string ctos(const unsigned int& c)
	{
		switch (c)
		{
			case 7:  return "\\a";
			case 8:  return "\\b";
			case 9:  return "\\t";
			case 10: return "\\n";
			case 11: return "\\v";
			case 12: return "\\f";
			case 13: return "\\r";
			case 92: return "\\\\";
			case 39: return "\\\'";
			case 34: return "\\\"";
			case 63: return "\\?";
			default: return std::string(1, c);
		}
	}
}
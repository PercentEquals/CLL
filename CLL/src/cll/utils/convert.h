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
}
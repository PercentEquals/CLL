#include "lexer.h"

// Author: Bartosz Niciak

#include "static.h"

namespace cll
{
	std::vector<var> lexer(const std::string& l)
	{
		std::vector<var> args; // Holds vars (token) list
		std::string buff = ""; // Holds token in buffor before pushing it to token list
		bool push = false; // Holds wheter to push actual token to list
		bool string = false; // Holds wheter token is a string (helpful to ignore special chars that create new tokens)
		bool chars = false; // Holds wheter token is a char (helpful to ignore special chars that create new tokens)
		bool comment = false; // Holds wheter token is a comment (helpful to ingore all following tokens as comments)
		unsigned int parenthesis = 0; // Holds wheter token is a parenthesis (helpful to ignore special chars that create new tokens)
		unsigned int array = 0; // Holds wheter token is an array (helpful to ignore special chars that create new tokens)
		std::string special = ""; // Holds sepcial char as token to be pushed (works like this: new x=10 -> new x = 10)
		std::string mult = "";

		args.reserve(50);
		special.reserve(3);
		buff.reserve(25);
		mult.reserve(3);

		for (auto it = l.begin(), end = l.end(); it < end; ++it)
		{
			// CHECKS FOR STRINGS
			if (string && *it == '"')
			{
				if (it != l.begin() && *(it - 1) != '\\') string = false;
				else buff.pop_back();
			}
			else if (!string && *it == '"') string = true;

			if (chars && *it == '\'')
			{
				if(it != l.begin() && *(it - 1) != '\\') chars = false;
				else buff.pop_back();
			}
			else if (!chars && *it == '\'') chars = true;

			// CHECKS FOR PARENTHESIS
			if (!string)
			{
				if (parenthesis && *it == ')') parenthesis--;
				else if (*it == '(') parenthesis++;

				if (array && *it == ']') array--;
				else if (*it == '[') array++;
			}

			// PUSHES TO BUFFOR
			if (string || parenthesis || array || chars) buff += *it;
			else
			{
				size_t symbols = lexer_symbols.find_first_of(*it);

				// CHECKS FOR SPECIAL SYMBOLS
				if (symbols != std::string::npos || *it == '\t')
				{
					push = true;

					if (*it != '\t' && *it != ' ')
					{
						special = *it;

						// CHECKS FOR MULTIPLE SPECIALS CHARS (==, !=, ...)
						if (it + 1 != end)
						{
							if (*(it + 1) != '\t' && *(it + 1) != ' ')
							{
								for (size_t i = 0; i < multi_symbols.size(); ++i)
								{
									mult = "";

									for (size_t ii = 0; ii < multi_symbols[i].length(); ++ii)
									{
										if (it + ii >= end) break;
										if (*(it + ii) == multi_symbols[i][ii]) mult += *(it + ii);
									}

									if (mult == multi_symbols[i]) special = mult;

									if (special.length() > 1)
									{
										it += special.length() - 1;
										break;
									}
								}
							}
						}
					}
				}

				// CHECKS FOR COMMENTS
				if (special == "//")
				{
					special = "";
					push = true;
					comment = true;
				}

				if (*it != '\t' && symbols == std::string::npos && !comment) buff += *it;
				else push = true;

				if (push)
				{
					if (buff != "") args.emplace_back(buff);
					push = false;
					buff = "";

					// PUSHES SPECIAL CHARS TO THEIR OWN TOKEN
					if (comment) break;
					if (special != "") args.emplace_back(special);
					special = "";
				}
			}
		}

		if (buff != "") args.emplace_back(buff);
		if (special != "") args.emplace_back(special);

		return args;
	}
}
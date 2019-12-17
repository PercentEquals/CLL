#include "lexer.h"

// Author: Bartosz Niciak

#include "static.h"

namespace cll
{
	std::vector<var> lexer(const std::string& l)
	{
		std::vector<var> args; // Holds vars (token) list
		std::string buff(""); // Holds token in buffor before pushing it to token list
		bool push = false; // Holds whether to push actual token to list
		bool string = false; // Holds whether token is a string (helpful to ignore special chars that create new tokens)
		bool chars = false; // Holds whether token is a char (helpful to ignore special chars that create new tokens)
		bool comment = false; // Holds whether token is a comment (helpful to ingore all following tokens as comments)
		unsigned int parenthesis = 0; // Holds whether token is a parenthesis (helpful to ignore special chars that create new tokens)
		unsigned int array = 0; // Holds whether token is an array (helpful to ignore special chars that create new tokens)
		std::string special(""); // Holds special char as token to be pushed (works like this: x=10 -> x = 10)
		std::string mult("");

		args.reserve(50);
		special.reserve(3);
		buff.reserve(25);
		mult.reserve(3);

		for (auto it = l.begin(), end = l.end(); it < end; ++it)
		{
			// CHECKS FOR STRINGS
			if (!chars && string && *it == '"')
			{
				if (it != l.begin() && *(it - 1) != '\\') string = false;
				else if (it - 1 != l.begin() && *(it - 2) == '\\') string = false;
			}
			else if (!chars && !string && *it == '"') string = true;
			
			if (!string && chars && *it == '\'')
			{
				if (it != l.begin() && *(it - 1) != '\\') chars = false;
				else if (it - 1 != l.begin() && *(it - 2) == '\\') chars = false;
			}
			else if (!string && !chars && *it == '\'') chars = true;

			// CHECKS FOR PARENTHESIS
			if (!string && !chars)
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
									mult.clear();

									for (size_t ii = 0; ii < multi_symbols[i].length(); ++ii)
									{
										if (it + ii >= end) break;
										if (*(it + ii) == multi_symbols[i][ii]) mult += *(it + ii);
										else break;
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
					special.clear();
					push = true;
					comment = true;
				}

				if (*it != '\t' && symbols == std::string::npos && !comment) buff += *it;
				else push = true;

				if (push)
				{
					if (buff != "") args.emplace_back(buff);
					push = false;
					buff.clear();

					// PUSHES SPECIAL CHARS TO THEIR OWN TOKEN
					if (comment) break;
					if (special != "") args.emplace_back(special);
					special.clear();
				}
			}
		}

		// PUSHES LEFT OUT TOKENS
		if (buff != "") args.emplace_back(buff);
		if (special != "") args.emplace_back(special);

		return args;
	}
}
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

		for (size_t i = 0; i <= l.length(); ++i)
		{
			if (i != l.length())
			{
				// CHECKS FOR STRINGS
				if (string && l[i] == '"') string = false;
				else if (!string && l[i] == '"') string = true;

				if (chars && l[i] == '\'') chars = false;
				else if (!chars && l[i] == '\'') chars = true;

				// CHECKS FOR PARENTHESIS
				if (!string)
				{
					if (parenthesis && l[i] == ')') parenthesis--;
					else if (l[i] == '(') parenthesis++;

					if (array && l[i] == ']') array--;
					else if (l[i] == '[') array++;
				}

				// PUSHES TO BUFFOR
				if (string || parenthesis || array || chars) buff += l[i];
				else
				{
					// CHECKS FOR SPECIAL SYMBOLS
					if (std::string(1, l[i]).find_first_of(lexer_symbols) != std::string::npos || l[i] == '\t')
					{
						push = true;

						if (l[i] != '\t' && l[i] != ' ')
						{
							special = l[i];

							// CHECKS FOR MULTIPLE SPECIALS CHARS (==, !=, ...)
							if (i != l.length() - 1)
							{
								if (l[i + 1] != '\t' && l[i + 1] != ' ')
								{
									for (size_t ii = 0; ii < multi_symbols.size(); ++ii)
									{
										std::string mult = "";

										for (size_t iii = 0; iii < multi_symbols[ii].length(); ++iii)
										{
											if (i + iii >= l.length()) break;
											if (l[i + iii] == multi_symbols[ii][iii]) mult += l[i + iii];
										}

										if (mult == multi_symbols[ii]) special = mult;

										if (special.length() > 1)
										{
											i += special.length() - 1;
											break;
										}
									}
								}
							}
						}
					}

					// CHECKS FOR COMMENTS
					if (i + 1 != l.length())
					{
						if (l[i] == '/' && l[i + 1] == '/')
						{
							push = true;
							comment = true;
						}
					}

					if (l[i] != '\t' && std::string(1, l[i]).find_first_of(lexer_symbols) == std::string::npos && !comment) buff += l[i];
					else push = true;
				}
			}
			else push = true;

			// PUSHES ARGUMENTS TO TOKEN
			if (push)
			{
				if (buff != "") args.emplace_back(var(buff));
				push = false;
				buff = "";

				// PUSHES SPECIAL CHARS TO THEIR OWN TOKEN
				if (comment) break;
				if (special != "") args.emplace_back(var(special));
				special = "";
			}
		}

		return args;
	}
}
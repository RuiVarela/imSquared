// __!!rengine_copyright!!__ //

#include "Project.h"

namespace re
{
	std::vector<std::string> split(std::string const& input, std::string const& delimiter)
	{
		StringElements output;

		if (input.size() != 0)
		{
			std::string::size_type start = 0;

			do
			{
				std::string::size_type end = input.find(delimiter, start);
				if (end == std::string::npos)
				{
					end = input.size();
				}

				std::string selection = input.substr(start, end - start);
				if (selection != "")
				{
					output.push_back(selection);
				}

				start = end + delimiter.size();

			}
			while (start < input.size());

		}
		return output;
	}

	std::string extractToken(std::string const& input, std::string const& delimiter, Uint token_number)
	{
		if (input.size() != 0)
		{
			std::string::size_type start = 0;
			std::string::size_type end = start;

			Bool searching = true;

			do
			{
				end = input.find(delimiter, start);
				if (end == std::string::npos)
				{
					searching = false;
					end = input.size();
				}

				if (token_number == 0)
				{
					searching = false;
				}

				if (searching)
				{
					start = end + delimiter.size();
				}
			}
			while (searching);

			return input.substr(start, end - start);
		}
		return input;
	}

	void trimRight(std::string &string)
	{
		std::string whitespaces(" \t\f\v\n\r");
		std::string::size_type pos = string.find_last_not_of(whitespaces);

		if (pos != std::string::npos)
		{
			string.erase(pos + 1);
		}
		else
		{
			string.clear();
		}
	}

    void trimLeft(std::string &string)
    {
        std::string whitespaces(" \t\f\v\n\r");
        std::string::size_type pos = string.find_first_not_of(whitespaces);

        if (pos != std::string::npos)
        {
            string.erase(0, pos);
        }
        else
        {
            string.clear();
        }
    }

    std::string trim(TrimType const& trim_type, std::string string)
    {
        if ((trim_type == TrimLeft) || (trim_type == TrimBoth))
        {
            trimLeft(string);
        }

        if ((trim_type == TrimRight) || (trim_type == TrimBoth))
        {
            trimRight(string);
        }

        return string;
    }

    void trim(std::string &string)
    {
        trimLeft(string);
        trimRight(string);
    }

	void uppercase(std::string &string)
	{
		std::transform(string.begin(), string.end(), string.begin(), (Int(*)(Int)) toUppercase);
	}

	void lowercase(std::string &string)
	{
		std::transform(string.begin(), string.end(), string.begin(), (Int(*)(Int)) toLowercase);
	}

    void camelcase(std::string &line, bool remove_spaces)
    {
        bool active = true;

        for(size_t i = 0; i < line.size(); i++) {
            if(std::isalpha(line[i])) {
                if(active) {
                    line[i] = std::toupper(line[i]);
                    active = false;
                } else {
                    line[i] = std::tolower(line[i]);
                }
            } else if(line[i] == ' ') {
                active = true;
            }
        }

        if (remove_spaces) {
           replace(line, " ", "");
        }
    }

    std::string convertCase(CaseType const& case_type, std::string string)
    {
        if (case_type == Uppercase)
        {
            uppercase(string);
        }
        else if (case_type == Lowercase)
        {
            lowercase(string);
        }
        else if (case_type == CamelCase)
        {
            camelcase(string);
        }

        return string;
    }



	void replace(std::string &target, std::string const& match, std::string const& replace_string)
	{
        if (match.empty() || target.empty())
            return;

		std::string::size_type position = target.find(match);
		while (position != std::string::npos)
		{
			target.replace(position, match.size(), replace_string);
			position = target.find(match, position + replace_string.size());
		}
	}

    std::string replaceString(std::string target, std::string const& match, std::string const& replace_string)
    {
        replace(target, match, replace_string);
        return target;
    }


	Bool startsWith(std::string const& string, std::string const& prefix)
	{
		return string.find(prefix) == std::string::size_type(0);
	}

	Bool endsWith(std::string const& string, std::string const& sufix)
	{
		if (string.size() < sufix.size())
		{
			return false;
		}
		else
		{
			return string.rfind(sufix) == (string.size() - sufix.size());
		}
	}

	//
	// Number functions
	//

	Bool isNumber(std::string const& string)
	{
		return (isInteger(string) || isReal(string));
	}

	Bool isInteger(std::string const& string)
	{
		if (string.empty())
		{
			return false;
		}

		std::string::const_iterator i = string.begin();
		if (!isDigit(string[0]))
		{
			if ((string[0] != '-') &&  (string[0] != '+'))
			{
				return false;
			}
			else
			{
				if (++i == string.end())
				{
					return false;
				}
			}
		}

		while (i != string.end())
		{
			if (!isDigit(*i))
			{
				return false;
			}
			++i;
		}

		return true;
	}



	Bool isReal(std::string const& string)
	{
		if (string.empty())
		{
			return false;
		}

		std::string::const_iterator i = string.begin();
		Bool dot_found = false;

		if (!isDigit(string[0]))
		{
			if ((string[0] != '-') && (string[0] != '.') && (string[0] != '+'))
			{
				return false;
			}
			else
			{
				if (string[0] == '.')
				{
					dot_found = true;
				}

				if (++i == string.end())
				{
					return false;
				}
			}
		}

		while (i != string.end())
		{
			if (!isDigit(*i))
			{
				if (*i == '.')
				{
					if (dot_found)
					{
						return false;
					}
					else
					{
						dot_found = true;
					}

				}
				else
				{
					return false;
				}
			}
			++i;
		}

		return dot_found;
	}

	Bool isBoolean(std::string const& string)
	{
		if (isInteger(string))
		{
			return ((string.size() == 1) && ((string[0] == '0') ||  (string[0] == '1')) );
		}
		else
		{
			return ( (string == "true") || (string == "false") );
		}
	}

	Bool isDigit(Int character)
	{
		return std::isdigit(character) != 0;
	}

	Bool isAlphabetic(Int character)
	{
		return std::isalpha(character) != 0;
	}

	Bool isLowercase(Int character)
	{
		return std::islower(character) != 0;
	}

	Bool isUppercase(Int character)
	{
		return std::isupper(character) != 0;
	}

	Int toUppercase(Int character)
	{
		return std::toupper(character);
	}

	Int toLowercase(Int character)
	{
		return std::tolower(character);
	}

	Bool equalCaseInsensitive(std::string const& lhs, std::string const& rhs)
	{
		if (lhs.size() != rhs.size())
		{
			return false;
		}

		std::string::const_iterator litr = lhs.begin();
		std::string::const_iterator ritr = rhs.begin();

		while (litr != lhs.end())
		{
			if (toLowercase(*litr) != toLowercase(*ritr))
			{
				return false;
			}

			++litr;
			++ritr;
		}
		return true;
	}

	Bool equalCaseInsensitive(std::string const& lhs, Char const* rhs)
	{
		if (rhs == 0 || lhs.size() != strlen(rhs))
		{
			return false;
		}

		std::string::const_iterator litr = lhs.begin();
		Char const* cptr = rhs;

		while (litr != lhs.end())
		{
			if (toLowercase(*litr) != toLowercase(*cptr))
			{
				return false;
			}

			++litr;
			++cptr;
		}
		return true;
	}


    std::string wstringToString(const std::wstring input)
    {
        std::string converted;
        for (std::wstring::const_iterator current = input.begin(); current != input.end(); ++current)  {
            wchar_t element = *current;
            converted.push_back(char(element));
        }

        return converted;
    }

    std::wstring stringToWstring(const std::string input)
    {
        std::wstring converted;
        for (std::string::const_iterator current = input.begin(); current != input.end(); ++current)  {
            char element = *current;
            converted.push_back(wchar_t(element));
        }

        return converted;
    }

} // namespace re


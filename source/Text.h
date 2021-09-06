#pragma once

#include "Project.h"

namespace re
{
	enum CaseSensitivity
	{
		CaseSensitive,
		CaseInsensitive
	};


	enum CaseType
	{
		Uppercase,
        Lowercase,
        CamelCase
	};

	enum TrimType
	{
		TrimRight,
		TrimLeft,
		TrimBoth
	};

	template<typename OutputType, typename InputType>
	OutputType lexical_cast(InputType const& value);

	template<typename OutputType, typename InputType>
	OutputType lexical_cast(InputType const& value, OutputType const& default_value);

	typedef std::vector<std::string> StringElements;
	StringElements split(std::string const& input, std::string const& delimiter);

	std::string extractToken(std::string const& input, std::string const& delimiter, Uint token_number);

	void trimRight(std::string &string);
	void trimLeft(std::string &string);
	void trim(std::string &string);
	std::string trim(TrimType const& trim_type, std::string string);

	void uppercase(std::string &string);
	void lowercase(std::string &string);
    void camelcase(std::string &string, bool remove_spaces = false);
	std::string convertCase(CaseType const& case_type, std::string string);

	void replace(std::string &target, std::string const& match, std::string const& replace_string);
    std::string replaceString(std::string target, std::string const& match, std::string const& replace_string);

	Bool startsWith(std::string const& string, std::string const& prefix);
	Bool endsWith(std::string const& string, std::string const& sufix);

	Bool isNumber(std::string const& string);
	Bool isInteger(std::string const& string);
	Bool isReal(std::string const& string);
	Bool isBoolean(std::string const& string);

	Bool isDigit(Int character);
	Bool isAlphabetic(Int character);
	Bool isLowercase(Int character);
	Bool isUppercase(Int character);

	Int toUppercase(Int character);
	Int toLowercase(Int character);

	Bool equalCaseInsensitive(std::string const& lhs, std::string const& rhs);
	Bool equalCaseInsensitive(std::string const& lhs, Char const* rhs);

    std::string wstringToString(std::wstring const input);
    std::wstring stringToWstring(std::string const input);

    template<typename T>
    std::string separateElements(std::vector<T> const& data, std::string separator);

    template<typename... Args>
    std::string sfmt(const char* fmt, const Args&... args)
    {
        return tfm::format(fmt, args...);
    }

	//
	// Implementation
	//
	template<typename T>
    std::string separateElements(std::vector<T> const& data, std::string separator)
	{
		std::stringstream stream;

		if (std::numeric_limits<T>::is_specialized)
		{
			stream.precision(std::numeric_limits<T>::digits10 + 1);

			if (!std::numeric_limits<T>::is_integer)
			{
				stream.setf(std::ios::showpoint | std::ios::fixed);
			}
		}

        for (size_t i = 0; i != data.size(); ++i)
		{
			if (i > 0)
			{
				stream << separator;
			}

            stream << data[i];
		}

		return stream.str();
	}

	//
	// Implementation
	//
	template<typename OutputType, typename InputType>
	OutputType lexical_cast(InputType const& value)
	{
		std::stringstream stream;
		//skip leading whitespace characters on input
		stream.unsetf(std::ios::skipws);

		//set the correct precision
		if (std::numeric_limits<OutputType>::is_specialized)
		{
			stream.precision(std::numeric_limits<OutputType>::digits10 + 1);
		}
//		else if (std::numeric_limits<InputType>::is_specialized)
//		{
//			stream.precision(std::numeric_limits<InputType>::digits10 + 1);
//		}

		stream << value;

		OutputType output;
		stream >> output;
		return output;
	}

	template<typename OutputType, typename InputType>
	OutputType lexical_cast(InputType const& value, OutputType const& default_value)
	{
		std::stringstream stream;
		//skip leading whitespace characters on input
		stream.unsetf(std::ios::skipws);

		//set the correct precision
		if (std::numeric_limits<OutputType>::is_specialized)
		{
			stream.precision(std::numeric_limits<OutputType>::digits10 + 1);
		}
//		else if (std::numeric_limits<InputType>::is_specialized)
//		{
//			stream.precision(std::numeric_limits<InputType>::digits10 + 1);
//		}

		stream << value;

		OutputType output;
		stream >> output;

		if (!stream)
		{
			output = default_value;
		}

		return output;
	}



} // namespace rengine



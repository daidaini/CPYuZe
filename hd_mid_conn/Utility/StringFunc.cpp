#include "StringFunc.h"

using namespace std;

namespace str
{
	bool StartsWith(const std::string &src, const std::string &sub)
	{
		return src.find(sub) == 0;
	}

	bool EndsWith(const std::string &src, const std::string &sub)
	{
		return src.rfind(sub) == (src.length() - sub.length());
	}

	std::string Lower(const std::string &src)
	{
		string result = "";
		for (auto it = src.begin(); it != src.end(); it++)
		{
			if (*it >= 'A' && *it <= 'Z')
			{
				result.push_back(*it + ('a' - 'A'));
			}
			else
			{
				result.push_back(*it);
			}
		}
		return result;
	}

	std::string Upper(const std::string &src)
	{
		string result = "";
		for (auto it = src.begin(); it != src.end(); it++)
		{
			if (*it >= 'a' && *it <= 'z')
			{
				result.push_back(*it - ('a' - 'A'));
			}
			else
			{
				result.push_back(*it);
			}
		}
		return result;
	}

	std::string Lstrip(const std::string& s)
	{
		if (s.empty())
			return s;
		if (s.front() != ' ')
			return s;
		for (auto index = 0u; index < s.size(); index++)
		{
			if (s[index] != ' ')
			{
				return s.substr(index);
			}
		}
		return s;
	}

	std::string Rstrip(const std::string& s)
	{
		if (s.empty())
			return s;
		if (s.back() != ' ')
			return s;
		for (auto index = s.size() - 1; index >= 0; index--)
		{
			if (s[index] != ' ')
			{
				return s.substr(0, index + 1);
			}
		}
		return s;
	}

	std::string Strip(const std::string& s)
	{
		return Rstrip(Lstrip(s));
	}

	std::string StripQuote(const std::string& src)
	{
		if (StartsWith(src, "\"") && EndsWith(src, "\""))
		{
			return src.substr(1, src.size() - 2);
		}
		return src;
	}

	std::vector<std::string> Split(const std::string& s, char separator)
	{
		vector<string> result;
		size_t pos;
		string temp = s;
		while ((pos = temp.find_first_of(separator)) != string::npos)
		{
			if (pos > 0)
			{
				result.emplace_back(temp.substr(0, pos));
			}
			temp = temp.substr(pos + 1);
		}
		if (temp.length() > 0)
		{
			result.emplace_back(temp);
		}
		return result;
	}

	std::string FoldSpace(const std::string &src)
	{
		string result = "";
		char lastItem = '\0';
		for (auto it = src.begin(); it != src.end(); it++)
		{
			if (lastItem == ' ' && *it == ' ')
			{
				continue;
			}
			result.push_back(*it);
			lastItem = *it;
		}
		return Strip(result);
	}

	std::string CutFront(const std::string &src, int length)
	{
		if ((int)src.length() <= length)
		{
			return src;
		}
		return src.substr(src.length() - length, length);
	}

	std::string Escape(std::string src, char op)
	{
		size_t index = 0;
		while ((index = src.find(op, index)) != string::npos)
		{
			src.insert(index, 1, '\\');
			index += 2;
		}
		return src;
	}

	std::string Right(const std::string &src, int length)
	{
		if ((int)src.length() <= length)
		{
			return src;
		}
		return src.substr(src.length() - length, length);
	}

	std::string Left(const std::string &src, int length)
	{
		if ((int)src.length() <= length)
		{
			return src;
		}
		return src.substr(0, length);
	}

	std::string Remove(const std::string& src, char c)
	{
		string result;
		result.reserve(src.size());

		for (auto it : src)
		{
			if (it != c)
				result.push_back(it);
		}

		return result;
	}

	int Count(const std::string& src, char c)
	{
		int count = 0;
		for (auto it : src)
		{
			if (it == c)
				++count;
		}

		return count;
	}

	void ReplaceStr(std::string &src, const std::string& before, const std::string& after)
	{
		if (src.empty())
			return;

		size_t index = 0;
		do
		{
			auto it = src.find(before, index);
			if (it == src.npos)
				break;
			src.replace(it, before.length(), after);

			index = it + 1;

		} while (true);
	}

}
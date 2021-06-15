#ifndef STRING_FUNC_H
#define STRING_FUNC_H

#include <vector>
#include <string>

namespace str
{
	bool StartsWith(const std::string &src, const std::string &sub);
	bool EndsWith(const std::string &src, const std::string &sub);
	std::string Lower(const std::string &src);
	std::string Upper(const std::string &src);

	std::string Lstrip(const std::string& src);
	std::string Rstrip(const std::string& src);
	std::string Strip(const std::string& src);
	std::string StripQuote(const std::string& src);

	std::vector<std::string> Split(const std::string& src, char separator);

	std::string FoldSpace(const std::string &src);
	std::string CutFront(const std::string &src, int length);
	std::string Escape(std::string src, char op);

	std::string Right(const std::string &src, int length);
	std::string Left(const std::string &src, int length);

	std::string Remove(const std::string& src, char c);

	int Count(const std::string& src, char c);
	void ReplaceStr(std::string &src, const std::string& before, const std::string& after);
}

#endif

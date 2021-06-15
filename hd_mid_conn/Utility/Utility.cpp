#include "Utility.h"
#include <fstream>
#include <sstream>

std::vector<std::string> Utility::ReadLinesFromFile(const std::string &filename)
{
	std::ifstream fs;
	fs.open(filename);

	std::vector<std::string> lines;
	if (!fs.is_open())
	{
		return lines;
	}

	std::string oneline;
	while (getline(fs, oneline))
	{
		if (oneline.empty())
		{
			//¿ÕÐÐ²»Òª
			continue;	
		}

		lines.emplace_back(oneline);
	}
	fs.close();

	return lines;
}

std::string Utility::ReadContentsFromFile(const std::string &filename)
{
	std::fstream fs;
	fs.open(filename);
	if (!fs.is_open())
		return "";

	std::ostringstream oss;
	oss << fs.rdbuf();
	fs.close();
	return oss.str();

}
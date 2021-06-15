#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <assert.h>
#include <string>
#include <vector>

class Utility
{
public:

	static std::vector<std::string> ReadLinesFromFile(const std::string &filename);

	static std::string ReadContentsFromFile(const std::string &filename);
};


#endif

#include "utility.h"

#include <cstring>
#include <stdexcept>

std::vector<std::string>
util::parse_value(const std::string& value)
{
	std::vector<std::string> temp;
	std::size_t start = 0;
	std::size_t size = 0;

	do {
		std::size_t end = value.find_first_of(',', start);
		
		if (end != std::string::npos)
			size = end - start;
		else
			size = std::string::npos;

		temp.emplace_back(value.substr(start, size));

		start = end + 1;
	} while (size != std::string::npos);

	return temp;
}

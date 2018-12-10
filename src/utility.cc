#include "utility.h"

#include <cstring>
#include <stdexcept>

std::vector<std::pair<std::string, std::vector<std::string>>>
util::parse_commandline(const int count, const char** vector)
{
	std::vector<std::pair<std::string, std::vector<std::string>>> temp;
	for (unsigned i = 1; i < count; i++) {
		const char* arg = vector[i];
		if (std::strncmp(arg, "--", 2) == 0)
			temp.emplace_back(std::pair(&arg[2],
						    std::vector<std::string>{}));
		else if (not temp.empty())
			temp.back().second.emplace_back(arg);
		else
			throw std::logic_error{"found value without key"};
	}
	return temp;
}

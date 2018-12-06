#include "utility.h"

#include <cstring>
#include <stdexcept>

std::vector<std::pair<std::string, std::string>>
util::parse_commandline(const int count, char** vector)
{
	std::vector<std::pair<std::string, std::string>> temp;

	for (unsigned i = 1; i < count; i++) {
		const char* arg = vector[i];

		if (std::strncmp(arg, "--", 2) == 0)
			temp.emplace_back(std::make_pair(&arg[2],
							 vector[++i]));
		else
			throw std::logic_error{"found value without key"};
	}

	return temp;
}

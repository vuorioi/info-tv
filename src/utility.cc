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

bool
util::similar_str(const std::wstring_view& lhs,
		  const std::wstring_view& rhs)
{
	const auto lhs_len = lhs.length();
	const auto rhs_len = rhs.length();

	if (lhs_len < rhs_len) {
		if (rhs.find(lhs) != std::string_view::npos)
			return (lhs_len*100)/rhs_len >= 33;
		else
			return false;
	} else if (lhs_len > rhs_len) {
		if (lhs.find(rhs) != std::string_view::npos)
			return (rhs_len*100)/lhs_len >= 33;
		else
			return false;
	} else {
		return lhs == rhs;
	}
}

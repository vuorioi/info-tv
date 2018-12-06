#pragma once

#include <array>
#include <string>
#include <utility>
#include <vector>


namespace util {
	std::vector<std::pair<std::string, std::string>> parse_commandline(const int count,
									   char** vector);
	template<std::size_t Count>
	std::array<std::string, Count> parse_value(const std::string& value)
	{
		std::array<std::string, Count> temp;
		std::size_t start = 0;

		for (std::size_t i = 0; i < Count; i++) {
			std::size_t end = value.find_first_of(',', start);
			std::size_t size;
			
			if (end != std::string::npos)
				size = end - start;
			else
				size = std::string::npos;

			temp[i] = value.substr(start, size);

			start = end + 1;

			if (size == std::string::npos)
				break;
		}

		return temp;
	}
}

#pragma once

#include <string>
#include <utility>
#include <vector>


namespace util {
	/* parse_commandline - parse arguments
	 * @count: size of vector
	 * @vector: commandline options
	 *
	 * Returns a vector of pairs containing the argument name and a vector
	 * of values if any where passed for the argument.
	 *
	 * This function expects the commandline arguments to be formated as
	 * such:
	 *   --name0 value0 value1 --name1 value0 ...
	 */
	std::vector<std::pair<std::string,
			      std::vector<std::string>>>
	parse_commandline(const int count, const char** vector);
}

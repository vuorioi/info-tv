#pragma once

#include <string>
#include <string_view>
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

	/* similar_str - compare two strings to see if they are similar
	 * @lhs: string_view to the first string
	 * @rhs: string_view to the second string
	 *
	 * Returns true if the two strings are similar. Similarity is checked by
	 * seeing if the shorter string is a substring of the longer string and
	 * if the substring length is atleast 33% of the length of the longer
	 * string.
	 */
	bool similar_str(std::wstring_view lhs,
			 std::wstring_view rhs);
}

#include "utility.h"

#include <algorithm>
#include <cstring>
#include <iterator>
#include <numeric>
#include <stdexcept>

#include <boost/tokenizer.hpp>

static bool
tokens_are_similar(std::wstring_view lhs, std::wstring_view rhs)
{
	const auto str_length_cmp =
		[](std::wstring_view rhs, std::wstring_view lhs) {
		return rhs.length() < lhs.length();
	};

	auto [s, l] = std::minmax(rhs, lhs, str_length_cmp);

	// Check the longer token against the shorter token at
	// different positions and calculate the similairy for each
	// position (stop if similarity at one of the positions passes
	// the threshold)
	const unsigned similarity_threshold = (l.length() * 66)/100;

	for (auto l_iter = l.cbegin();
	     std::distance(l_iter, l.cend()) >= s.length();
	     ++l_iter) {
		auto s_iter = s.cbegin();
		const auto calc_similarity = [&iter = s_iter](unsigned count, wchar_t c) {
			if (tolower(*iter++) == tolower(c)) {
				count++;
			}

			return count;
		};

		const unsigned similarity = std::accumulate(l_iter,
                                                    l.cend(),
                                                    0,
                                                    calc_similarity);

		if (similarity >= similarity_threshold)
			return true;
	}

	return false;
}

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
util::similar_str(std::wstring_view lhs,
		  std::wstring_view rhs)
{
	const auto str_length_cmp =
		[](std::wstring_view rhs, std::wstring_view lhs) {
		return rhs.length() < lhs.length();
	};

	auto [s, l] = std::minmax(rhs, lhs, str_length_cmp);

	using separator_t = boost::char_separator<wchar_t>;
	using iterator_t = std::wstring::const_iterator;
	using string_t = std::wstring;

	constexpr wchar_t sep_str[] = L".,:; -/\\";

	boost::tokenizer<separator_t, iterator_t, string_t>
		s_tokens(s, separator_t{sep_str});

	boost::tokenizer<separator_t, iterator_t, string_t>
		l_tokens(l, separator_t{sep_str});

	// Compare the tokenized strings and calculate the number of similar
	// tokens. Stop if the number of similar tokens is greater than the
	// threshold
	const auto l_token_count = std::distance(l_tokens.begin(),
                                             l_tokens.end());
	const unsigned similar_threshold =
        std::clamp(static_cast<unsigned>((l_token_count*33)/100),
                   1U,
                   std::numeric_limits<unsigned>::max());

	unsigned similar_tokens = 0;
	auto l_token_start = l_tokens.begin();

	for (auto s_token = s_tokens.begin();
	     s_token != s_tokens.end();
	     ++s_token) {
		for (auto l_token = l_token_start;
		     l_token != l_tokens.end();
		     ++l_token) {
			if (tokens_are_similar(*s_token, *l_token)) {
				similar_tokens++;
				l_token_start = std::next(l_token);
				break;
			}
		}

		if (similar_tokens >= similar_threshold)
			return true;
	}

	return false;
}

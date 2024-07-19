#pragma once

#include <regex>
#include <vector>
#include <iterator>

namespace bsc
{
	namespace regex
	{
		inline float match_to_float(const std::cmatch& m)
		{
			return std::stof(m.str());
		}

		inline int match_to_int(const std::cmatch& m)
		{
			return std::stoi(m.str());
		}

		bool search_all_matches(const char* s, std::vector<std::cmatch>& outMatches, const std::regex& rgx);
	}
}
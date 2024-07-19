#include <BasicRegex.h>

namespace bsc
{
	namespace regex
	{	
		bool search_all_matches(const char* s, std::vector<std::cmatch>& outMatches, const std::regex& rgx)
		{
			std::cmatch match;
			while (std::regex_search(s, match, rgx))
			{
				outMatches.push_back(match);
				s = match.suffix().first;
			}

			return outMatches.size() > 0;
		}
	}
}
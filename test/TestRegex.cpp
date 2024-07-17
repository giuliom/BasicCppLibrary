#include <gtest/gtest.h>

#include <BasicRegex.h>

#define SUITE_NAME TestRegex

TEST(SUITE_NAME, SelectionSort)
{
    std::vector<int> control = { 0, 1,2 };
    std::vector<std::cmatch> matches;
    std::regex rgx(R"(\d+)");
    bsc::regex::search_all_matches(" 0, 1, 2", matches, rgx);

    EXPECT_EQ(matches.size(), control.size());

    for (int i=0; i < control.size(); i++)
    {
        EXPECT_EQ(bsc::regex::match_to_int(matches[i]), control[i]);
    }
}
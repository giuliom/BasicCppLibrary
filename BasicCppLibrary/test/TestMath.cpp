#include <gtest/gtest.h>

#include <BasicMath.h>

#define SUITE_NAME TestMath

TEST(SUITE_NAME, Fibonacci)
{
	std::vector<int> control = { 0, 1, 1, 2, 3, 5, 8, 13, 21, 34 };
	std::vector<int> result;

	for (int i = 0; i < 10; i++)
	{
		result.push_back(bsc::math::fibonacci<int>(i));
	}

	EXPECT_EQ(result, control);
}
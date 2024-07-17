#include <gtest/gtest.h>

#include <BasicEnum.h>

#define SUITE_NAME TestEnum

TEST(SUITE_NAME, Enum)
{
    enum class TEST_ENUM
    {
        A, B, C, COUNT
    };

    bsc::bit_enum<TEST_ENUM, TEST_ENUM::COUNT> be_test;

    be_test[TEST_ENUM::B] = true;

    be_test[TEST_ENUM::C] = be_test[TEST_ENUM::B];

    const std::bitset<3> control = 0b110;

    EXPECT_EQ(be_test.data(), control);
}
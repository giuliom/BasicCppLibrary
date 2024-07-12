#include <gtest/gtest.h>

#include <BasicChrono.h>
#include <thread>

#define SUITE_NAME TestChrono

using namespace std::chrono_literals;

TEST(SUITE_NAME, DurationConversion)
{
    const auto begin = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(10ms);
    const auto now = std::chrono::high_resolution_clock::now();

    const float milliseconds = bsc::chrono::convert_chrono_duration<float, std::chrono::milliseconds>(now - begin);
    EXPECT_TRUE(milliseconds > 0);
}
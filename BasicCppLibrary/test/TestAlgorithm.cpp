#include <gtest/gtest.h>

#include <BasicAlgorithm.h>

#define SUITE_NAME TestAlgorithm

TEST(SUITE_NAME, SelectionSort)
{
	std::vector<int> v = { 12, -1, 3, 0, 9, 5 };
    std::vector<int> control = { -1, 0, 3, 5, 9, 12 };
    bsc::selection_sort(v.begin(), v.end());

	EXPECT_TRUE(v == control);
}

TEST(SUITE_NAME, BubbleSort)
{
	std::vector<int> v = { 12, -1, 3, 0, 9, 5 };
	std::vector<int> control = { -1, 0, 3, 5, 9, 12 };
	bsc::bubble_sort(v.begin(), v.end());

	EXPECT_TRUE(v == control);
}

TEST(SUITE_NAME, MergeSort)
{
	std::vector<int> v = { 12, -1, 3, 0, 9, 5 };
	std::vector<int> control = { -1, 0, 3, 5, 9, 12 };
	bsc::merge_sort(v.begin(), v.end());

	EXPECT_TRUE(v == control);
}

TEST(SUITE_NAME, QuickSort)
{
	std::vector<int> v = { 12, -1, 3, 0, 9, 5 };
	std::vector<int> control = { -1, 0, 3, 5, 9, 12 };
	bsc::quick_sort(v.begin(), v.end());

	EXPECT_TRUE(v == control);
}

TEST(SUITE_NAME, BitRadixSort)
{
	std::vector<int> v = { 12, -1, 3, 0, 9, 5 };
	std::vector<int> control = { -1, 0, 3, 5, 9, 12 };
	bsc::bit_radix_sort(v.begin(), v.end());

	EXPECT_TRUE(v == control);
}

TEST(SUITE_NAME, BinarySearch)
{
	std::vector<int> v = { 12, -1, 3, 0, 9, 5 };
	auto it = bsc::binary_search(v, 9);

	EXPECT_FALSE(it == v.end());
}
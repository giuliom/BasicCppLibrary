// BasicCppLibrary.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "BasicAlgorithm.h"

using namespace bsc;

int main()
{
    //TODO unit testing

    std::vector<int> v = { 64, 34, 25, 12, -22, 90, -5 };
    std::vector<int> v2 = v;
    bubble_sort(v.begin(), v.end());

    auto res = binary_search(v2, 12);
    std::cin.get();

    return 0;
}
// BasicCppLibrary.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "BasicGlobal.h"
#include "BasicAlgorithm.h"
#include "BasicMath.h"
#include "BasicChrono.h"

using namespace std::chrono_literals;

int main()
{
    //TODO unit testing

    // --------------------------------------- Algorithm Demo ---------------------------------------

    std::vector<int> v = { -1, 0, 3, 5, 9, 12 };
    std::vector<int> v2 = v;
    bsc::bubble_sort(v.begin(), v.end());

    auto searched_it = bsc::binary_search(v2, 9);

    // --------------------------------------- Chrono Demo ------------------------------------------

    const auto begin = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(10ms);
    const auto now = std::chrono::high_resolution_clock::now();

    float milliseconds = 0;
    bsc::chrono::convert_chrono_duration<std::chrono::milliseconds>(now - begin, milliseconds);
    std::cout<<milliseconds<<" ms\n";

    // --------------------------------------- Math Demo --------------------------------------------

    uint n = 10u;
    std::cout<<std::endl;
    for (uint i = 0u; i < n; ++i)
    {
        std::cout<<bsc::math::fibonacci(i)<<" ";
    }
    std::cout << std::endl;

    std::cin.get();
    return 0;
}
// BasicCppLibrary.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "BasicGlobal.h"
#include "BasicAlgorithm.h"
#include "BasicMath.h"
#include "BasicChrono.h"
#include "BasicRegex.h"
#include "BasicMemory.h"

using namespace std::chrono_literals;

int main()
{
    //TODO unit testing
    std::cout<< "--- BasicCppLibrary Demo ---"<<std::endl;

    // --------------------------------------- Algorithm Demo ---------------------------------------

    std::cout << std::endl << "Algorithm Demo" << std::endl;
    std::vector<int> v = { -1, 0, 3, 5, 9, 12 };
    std::vector<int> v2 = v;
    bsc::bubble_sort(v.begin(), v.end());

    auto searched_it = bsc::binary_search(v2, 9);
    std::cout << *searched_it;
    std::cout << std::endl;

    // --------------------------------------- Chrono Demo ------------------------------------------

    std::cout << std::endl << "Chrono Demo" << std::endl;
    const auto begin = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(10ms);
    const auto now = std::chrono::high_resolution_clock::now();

    float milliseconds = bsc::chrono::convert_chrono_duration<float, std::chrono::milliseconds>(now - begin);
    std::cout<<milliseconds<<" ms\n";

    // --------------------------------------- Math Demo --------------------------------------------

    std::cout << std::endl << "Math Demo" << std::endl;
    uint n = 10u;
    for (uint i = 0u; i < n; ++i)
    {
        std::cout<<bsc::math::fibonacci(i)<<" ";
    }
    std::cout << std::endl;

    // --------------------------------------- Regex Demo --------------------------------------------

    std::cout << std::endl << "Regex Demo" << std::endl;
    std::vector<std::cmatch> matches;
    std::regex rgx(R"(\d+)");
    bsc::regex::search_all_matches(" 0, 1, 2", matches, rgx);

    for (auto& m : matches)
    {
        std::cout << bsc::regex::match_to_int(m) << " ";
    }
    std::cout << std::endl;

    // ------------------------------------------------------------------------------------------------

    std::cin.get();
    return 0;
}
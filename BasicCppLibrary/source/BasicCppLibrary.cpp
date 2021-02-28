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
    std::weak_ptr<int> ptr;
    {
        std::cout << std::endl << "Algorithm Demo" << std::endl;
        std::vector<int> v = { -1, 0, 3, 5, 9, 12 };
        std::vector<int> v2 = v;
        bsc::bubble_sort(v.begin(), v.end());

        auto searched_it = bsc::binary_search(v2, 9);
        std::cout << *searched_it;
        std::cout << std::endl;
    }

    // --------------------------------------- Chrono Demo ------------------------------------------

    {
        std::cout << std::endl << "Chrono Demo" << std::endl;
        const auto begin = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(10ms);
        const auto now = std::chrono::high_resolution_clock::now();

        float milliseconds = bsc::chrono::convert_chrono_duration<float, std::chrono::milliseconds>(now - begin);
        std::cout << milliseconds << " ms\n";
    }

    // --------------------------------------- Math Demo --------------------------------------------

    {
        std::cout << std::endl << "Math Demo" << std::endl;
        uint n = 10u;
        for (uint i = 0u; i < n; ++i)
        {
            std::cout << bsc::math::fibonacci(i) << " ";
        }
        std::cout << std::endl;
    }

    // --------------------------------------- Regex Demo --------------------------------------------

    {
        std::cout << std::endl << "Regex Demo" << std::endl;
        std::vector<std::cmatch> matches;
        std::regex rgx(R"(\d+)");
        bsc::regex::search_all_matches(" 0, 1, 2", matches, rgx);

        for (auto& m : matches)
        {
            std::cout << bsc::regex::match_to_int(m) << " ";
        }
        std::cout << std::endl;
    }

    // --------------------------------------- Memory Demo -----------------------------------------
    {
        std::cout << std::endl << "Memory Demo" << std::endl;

        const bsc::unique_ptr<int> size = new int(10);
        const int count = *size;

        int* my_ints = new int[count];

        std::cout << std::endl;

        for (int i = 0; i < count; ++i)
        {
            my_ints[i] = i;
            std::cout << my_ints[i] << " ";
        }

        delete[] my_ints;

        std::cout << std::endl;

        bsc::shared_ptr<float> sh0 = new float(3.14f);
        {
            bsc::shared_ptr<float> sh1 = sh0;
            std::vector<int, bsc::base_allocator<int>> base_allocator_vector(5, 1);

            std::cout << std::endl;

            for (const auto& i : base_allocator_vector)
            {
                std::cout << i << " ";
            }
        }

        std::cout << std::endl << *sh0;

        std::cout << std::endl;

#ifdef _DEBUG
        std::cout << "Allocations: " << bsc::base_allocator<int>::num_allocations() << " deallocations: " << bsc::base_allocator<int>::num_deallocations() << std::endl;
#endif // _DEBUG
    }

    // ------------------------------------------------------------------------------------------------

    std::cin.get();
    return 0;
}
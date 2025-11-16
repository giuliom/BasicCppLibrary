// BasicCppSample.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <iostream>
#include <vector>
#include <iterator>
#include "BasicGlobal.h"
#include "BasicAlgorithm.h"
#include "BasicMath.h"
#include "BasicChrono.h"
#include "BasicRegex.h"
#include "BasicMemory.h"
#include "BasicConcurrency.h"
#include "BasicEnum.h"

using namespace std::chrono_literals;

namespace
{
    std::mutex g_coutMtx;
}

int main()
{
    std::cout<< "--- BasicCppLibrary Sample Demos ---"<<std::endl;

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

        const float milliseconds = bsc::chrono::convert_chrono_duration<float, std::chrono::milliseconds>(now - begin);
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

    // --------------------------------------- Concurrency Demo -----------------------------------------

    {
        std::cout << std::endl << "Concurrency Demo" << std::endl;

        //TODO write proper demo
        bsc::lock_free_queue<uint> lfq;
        bsc::lock_free_ring_buffer<uint> lfrb(5);

        auto producer = [](auto* data_structure, uint iterations, const std::string& name)
            {
                while (iterations > 0)
                {
                    bool success = data_structure->produce(iterations);

                    if (success)
                    {
                        --iterations;
                    }
                }

                std::scoped_lock<std::mutex> lock(g_coutMtx);
                std::cout << std::endl << "Producer Thread " << name << ": data successfully produced" << std::endl;
            };

        auto consumer = [](auto* data_structure, uint expected_iterations, const std::string& name)
            {
                uint prev_value = expected_iterations + 1;

                while (expected_iterations > 0)
                {
                    uint value;
                    bool success = data_structure->consume(value);

                    if (success)
                    {
                        if (value != prev_value - 1)
                        {
                            std::cout << std::endl << "Consumer Thread " << name << ": Wrong data at iteration " << expected_iterations << std::endl;
                            return;
                        }

                        prev_value = value;
                        --expected_iterations;
                    }
                }

                std::scoped_lock<std::mutex> lock(g_coutMtx);
                std::cout << std::endl << "Consumer Thread " << name << ": data successfully consumed" << std::endl;
            };

        // Lock Free Queue Test
        auto lfq_producer = std::thread(producer, &lfq, 10u, "lfq_producer");
        auto lfq_consumer = std::thread(consumer, &lfq, 10u, "lfq_consumer");

        lfq_producer.join();
        lfq_consumer.join();

        // Lock Free Ring Buffer Test
        auto lfrb_producer = std::thread(producer, &lfrb, 10u, "lfrb_producer");
        auto lfrb_consumer = std::thread(consumer, &lfrb, 10u, "lfrb_consumer");

        lfrb_producer.join();
        lfrb_consumer.join();

        std::cin.get();
    }

    // --------------------------------------- Enum Class Demo -----------------------------------------

    {
        std::cout << std::endl << "Enum Class Demo" << std::endl;

        enum class TEST_ENUM
        {
            A, B, C, COUNT
        };

        bsc::bit_enum<TEST_ENUM, TEST_ENUM::COUNT> be_test;

        be_test[TEST_ENUM::B] = true;

        be_test[TEST_ENUM::C] = be_test[TEST_ENUM::B];

        std::cout << "Size: " << be_test.size() << std::endl;
    }

    // --------------------------------------- Math Demo -----------------------------------------

    {
        std::cout << std::endl << "Math Demo" << std::endl;

        std::cout << "First 10 Fibonacci Numbers: ";
        for (int i = 0; i < 10; i++)
        {
            std::cout << bsc::math::fibonacci<int>(i) << " ";
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
        bsc::weak_ptr<float> weak_sh0(sh0);
        {
            bsc::shared_ptr<float> sh1 = sh0;
            std::vector<int, bsc::base_allocator<int>> base_allocator_vector(5, 1);

            std::cout << std::endl;

            for (const auto& i : base_allocator_vector)
            {
                std::cout << i << " ";
            }
        }

        std::cout << std::endl << *sh0 << std::endl;

        sh0.reset();

        std::string expiration = weak_sh0.expired() ? "Expired shared_ptr" : std::to_string(*weak_sh0);
        std::cout << std::endl << expiration;

        std::cout << std::endl;

#ifdef _DEBUG
        std::cout << "Allocations: " << bsc::base_allocator<int>::num_allocations() << " deallocations: " << bsc::base_allocator<int>::num_deallocations() << std::endl;
#endif // _DEBUG
    }

    // ------------------------------------------------------------------------------------------------

    std::cin.get();
    return 0;
}
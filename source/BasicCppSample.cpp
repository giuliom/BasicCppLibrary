// BasicCppSample.cpp : Defines the entry point for the console application.
//

#include <print>
#include <iostream>
#include <vector>
#include <iterator>
#include <string_view>
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
    std::println("--- BasicCppLibrary Sample Demos ---");

    // --------------------------------------- Algorithm Demo ---------------------------------------
    std::weak_ptr<int> ptr;
    {
        std::println("\nAlgorithm Demo");
        std::vector<int> v = { -1, 0, 3, 5, 9, 12 };
        std::vector<int> v2 = v;
        bsc::bubble_sort(v.begin(), v.end());

        auto searched_it = bsc::binary_search(v2, 9);
        std::println("{}", *searched_it);
    }

    // --------------------------------------- Chrono Demo ------------------------------------------

    {
        std::println("\nChrono Demo");
        const auto begin = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(10ms);
        const auto now = std::chrono::high_resolution_clock::now();

        const float milliseconds = bsc::chrono::convert_chrono_duration<float, std::chrono::milliseconds>(now - begin);
        std::println("{} ms", milliseconds);
    }

    // --------------------------------------- Math Demo --------------------------------------------

    {
        std::println("\nMath Demo");
        uint n = 10u;
        for (uint i = 0u; i < n; ++i)
        {
            std::print("{} ", bsc::math::fibonacci(i));
        }
        std::println("");
    }

    // --------------------------------------- Regex Demo --------------------------------------------

    {
        std::println("\nRegex Demo");
        std::vector<std::cmatch> matches;
        std::regex rgx(R"(\d+)");
        bsc::regex::search_all_matches(" 0, 1, 2", matches, rgx);

        for (auto& m : matches)
        {
            std::print("{} ", bsc::regex::match_to_int(m));
        }
        std::println("");
    }

    // --------------------------------------- Concurrency Demo -----------------------------------------

    {
        std::println("\nConcurrency Demo");

        //TODO write proper demo
        bsc::lock_free_queue<uint> lfq;
        bsc::lock_free_ring_buffer<uint> lfrb(5);

        auto producer = [](auto* data_structure, uint iterations, std::string_view name)
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
                std::println("\nProducer Thread {}: data successfully produced", name);
            };

        auto consumer = [](auto* data_structure, uint expected_iterations, std::string_view name)
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
                            std::println("\nConsumer Thread {}: Wrong data at iteration {}", name, expected_iterations);
                            return;
                        }

                        prev_value = value;
                        --expected_iterations;
                    }
                }

                std::scoped_lock<std::mutex> lock(g_coutMtx);
                std::println("\nConsumer Thread {}: data successfully consumed", name);
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
        std::println("\nEnum Class Demo");

        enum class TEST_ENUM
        {
            A, B, C, COUNT
        };

        bsc::bit_enum<TEST_ENUM, TEST_ENUM::COUNT> be_test;

        be_test[TEST_ENUM::B] = true;

        be_test[TEST_ENUM::C] = be_test[TEST_ENUM::B];

        std::println("Size: {}", be_test.size());
    }

    // --------------------------------------- Math Demo -----------------------------------------

    {
        std::println("\nMath Demo");

        std::print("First 10 Fibonacci Numbers: ");
        for (int i = 0; i < 10; i++)
        {
            std::print("{} ", bsc::math::fibonacci<int>(i));
        }
        std::println("");
    }

    // --------------------------------------- Memory Demo -----------------------------------------
    
    {
        std::println("\nMemory Demo");

        const bsc::unique_ptr<int> size = new int(10);
        const int count = *size;

        int* my_ints = new int[count];

        std::println("");

        for (int i = 0; i < count; ++i)
        {
            my_ints[i] = i;
            std::print("{} ", my_ints[i]);
        }

        delete[] my_ints;

        std::println("");

        bsc::shared_ptr<float> sh0 = new float(3.14f);
        bsc::weak_ptr<float> weak_sh0(sh0);
        {
            bsc::shared_ptr<float> sh1 = sh0;
            std::vector<int, bsc::base_allocator<int>> base_allocator_vector(5, 1);

            std::println("");

            for (const auto& i : base_allocator_vector)
            {
                std::print("{} ", i);
            }
        }

        std::println("\n{}", *sh0);

        sh0.reset();

        std::string expiration = weak_sh0.expired() ? "Expired shared_ptr" : std::to_string(*weak_sh0);
        std::print("\n{}", expiration);

        std::println("");

#ifdef _DEBUG
        std::println("Allocations: {} deallocations: {}", bsc::base_allocator<int>::num_allocations(), bsc::base_allocator<int>::num_deallocations());
#endif // _DEBUG
    }

    // ------------------------------------------------------------------------------------------------

    std::cin.get();
    return 0;
}
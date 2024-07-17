#include <gtest/gtest.h>

#include <BasicGlobal.h>
#include <BasicConcurrency.h>

#define SUITE_NAME TestConcurrency

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
            EXPECT_TRUE(value == prev_value - 1);

            prev_value = value;
            --expected_iterations;
        }
    }
};

TEST(SUITE_NAME, LockFreeQueue)
{
    bsc::lock_free_queue<uint> lfq;

    auto lfq_producer = std::thread(producer, &lfq, 10u, "lfq_producer");
    auto lfq_consumer = std::thread(consumer, &lfq, 10u, "lfq_consumer");

    lfq_producer.join();
    lfq_consumer.join();
}

TEST(SUITE_NAME, LockFreeRingBuffer)
{
    bsc::lock_free_ring_buffer<uint> lfrb(5);

    auto lfrb_producer = std::thread(producer, &lfrb, 10u, "lfrb_producer");
    auto lfrb_consumer = std::thread(consumer, &lfrb, 10u, "lfrb_consumer");

    lfrb_producer.join();
    lfrb_consumer.join();
}
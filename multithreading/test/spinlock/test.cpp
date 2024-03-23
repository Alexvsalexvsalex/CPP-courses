#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <thread>
#include "spinlock.h"


size_t thread_count = 10;
size_t inc_every_thread = 100000;


BOOST_AUTO_TEST_CASE(fail_when_no_spinlock)
{
    std::vector<std::thread> threads;
    threads.reserve(thread_count);
    int result = 0;
    auto thread_increments = [&]() {
        for (size_t i = 0; i < inc_every_thread; ++i) {
            result += 1;
            if (result == 0) result += 1;  // Unreachable, but otherwise compiler will optimize loop
        }
    };
    for (size_t i = 0; i < thread_count; ++i) {
        threads.emplace_back(thread_increments);
    }
    for (auto& thread : threads) {
        thread.join();
    }
    auto expected = thread_count * inc_every_thread;
    std::cout << "Expected-not_" << expected << "   Actual_" << result << std::endl;
    BOOST_CHECK( result < expected );
}

BOOST_AUTO_TEST_CASE(pass_when_enabled_spinlock)
{
    mt::Spinlock spinlock;
    std::vector<std::thread> threads;
    threads.reserve(thread_count);
    int result = 0;
    auto thread_increments = [&]() {
        for (size_t i = 0; i < inc_every_thread; ++i) {
            spinlock.lock();
            result += 1;
            spinlock.unlock();
        }
    };
    for (size_t i = 0; i < thread_count; ++i) {
        threads.emplace_back(thread_increments);
    }
    for (auto& thread : threads) {
        thread.join();
    }
    auto expected = thread_count * inc_every_thread;
    std::cout << "Expected_" << expected << "   Actual_" << result << std::endl;
    BOOST_CHECK( result == expected );
}

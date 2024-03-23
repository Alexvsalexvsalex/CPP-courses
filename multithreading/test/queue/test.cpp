#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <thread>
#include "queue.h"

size_t thread_count = 8;
size_t insert_every_thread = 10000;

void test_single_thread(mt::IQueue& queue)
{
    size_t result = 0;
    for (int i = 0; i < insert_every_thread; ++i) {
        queue.push(i);
    }
    for (size_t i = 0; i < insert_every_thread; ++i) {
        result += *queue.pop();
    }
    queue.close();
    queue.push(1);
    BOOST_CHECK( !queue.pop().has_value() );
    auto expected = (insert_every_thread - 1) * insert_every_thread / 2;
    std::cout << "Expected_" << expected << "   Actual_" << result << std::endl;
    BOOST_CHECK( result == expected );
}

void test_multi_thread(mt::IQueue& queue)
{
    std::atomic_size_t result = 0;
    std::vector<std::thread> threads;
    threads.reserve(2 * thread_count);
    auto thread_push = [&]() {
        try{
            for (int i = 0; i < insert_every_thread; ++i) {
                queue.push(i);
            }
        } catch (const std::exception& exc) {
            std::cerr << "Push" << exc.what() << std::endl;
        }
    };
    auto thread_pop = [&]() {
        try {
            size_t local_result = 0;
            for (size_t i = 0; i < insert_every_thread; ++i) {
                local_result += *queue.pop();
            }
            result += local_result;
        } catch (const std::exception& exc) {
            std::cerr << "Pop" << exc.what() << std::endl;
        }
    };
    for (size_t i = 0; i < thread_count; ++i) {
        threads.emplace_back(thread_push);
        threads.emplace_back(thread_pop);
    }
    for (auto& thread : threads) {
        thread.join();
    }
    auto expected = thread_count * (insert_every_thread - 1) * insert_every_thread / 2;
    std::cout << "Expected_" << expected << "   Actual_" << result << std::endl;
    BOOST_CHECK( result == expected );
}

BOOST_AUTO_TEST_CASE(spinlock_queue_single_thread)
{
    mt::QueueWithSpinlock queue(10000);
    test_single_thread(queue);
}

BOOST_AUTO_TEST_CASE(spinlock_queue_multi_thread)
{
    mt::QueueWithSpinlock queue(500);
    test_multi_thread(queue);
}

BOOST_AUTO_TEST_CASE(cv_queue_single_thread)
{
    mt::QueueWithCondVar queue(10000);
    test_single_thread(queue);
}

BOOST_AUTO_TEST_CASE(cv_queue_multi_thread)
{
    mt::QueueWithCondVar queue(500);
    test_multi_thread(queue);
}

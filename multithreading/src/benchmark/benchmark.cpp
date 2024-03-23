#include "benchmark.h"
#include "queue.h"
#include <thread>
#include <vector>

namespace {
template<typename Queue>
void worker(Queue& in, Queue& out, std::function<int(int)> functor) {
    while (auto item = in.pop()) {
        int value = *item;
        out.push(functor(value));
    }
}

template<typename Queue>
void generator(Queue& in, size_t generator_iteration) {
    for (size_t i = 0; i < generator_iteration; i++) {
        in.push(static_cast<int>(i));
    }
}

template<typename Queue>
void reader(Queue& out) {
    while (out.pop()) {
    }
}

template<typename Queue>
void pipeline(size_t worker_count, size_t generator_iteration, size_t queue_capacity, std::function<int(int)> functor) {
    Queue input(queue_capacity);
    Queue output(queue_capacity);

    // worker threads process tasks
    std::vector<std::thread> worker_threads;
    for (size_t i = 0; i < worker_count; i++) {
        worker_threads.emplace_back(worker<Queue>, std::ref(input), std::ref(output), functor);
    }

    // generator thread generates tasks
    std::thread generator_thread(generator<Queue>, std::ref(input), generator_iteration);
    // reader thread reads task results
    std::thread reader_thread(reader<Queue>, std::ref(output));

    generator_thread.join();
    input.close();
    for (auto& worker_thread: worker_threads) {
        worker_thread.join();
    }
    output.close();
    reader_thread.join();
}
}

namespace bench {
    void benchmark_queue_spin(size_t worker_count, size_t generator_iteration, size_t queue_capacity, std::function<int(int)> functor) {
        pipeline<mt::QueueWithSpinlock>(worker_count, generator_iteration, queue_capacity, functor);
    }

    void benchmark_queue_cond_var(size_t worker_count, size_t generator_iteration, size_t queue_capacity, std::function<int(int)> functor) {
        pipeline<mt::QueueWithCondVar>(worker_count, generator_iteration, queue_capacity, functor);
    }
}
#pragma once
#include <functional>

namespace bench {
    void benchmark_queue_spin(size_t worker_count, size_t generator_iteration, size_t queue_capacity, std::function<int(int)> functor);

    void benchmark_queue_cond_var(size_t worker_count, size_t generator_iteration, size_t queue_capacity, std::function<int(int)> functor);
}
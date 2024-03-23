#include "benchmark.h"

#include <boost/program_options.hpp>
#include <chrono>
#include <iostream>

using namespace boost::program_options;
using namespace std::chrono;

auto task(int x) -> int {
    // Search of divisor
    for (int i = 2; i < x; ++i) {
        if (x % i == 0) {
            return i;
        }
    }
    return x;
}

int main(int argc, char **argv) {
    try {
        options_description desc{"Options"};
        desc.add_options()
                ("worker_count", value<size_t>()->default_value(1), "threads to calculate")
                ("generator_iteration", value<size_t>()->default_value(10), "task size power")
                ("queue_capacity", value<size_t>()->default_value(10), "queue size");

        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        auto t1 = std::chrono::high_resolution_clock::now();
        bench::benchmark_queue_spin(vm["worker_count"].as<size_t>(), 1 << vm["generator_iteration"].as<size_t>(), vm["queue_capacity"].as<size_t>(), task);
        auto t2 = std::chrono::high_resolution_clock::now();
        bench::benchmark_queue_cond_var(vm["worker_count"].as<size_t>(), 1 << vm["generator_iteration"].as<size_t>(), vm["queue_capacity"].as<size_t>(), task);
        auto t3 = std::chrono::high_resolution_clock::now();

        std::cout << "Spinlock time in ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << std::endl;
        std::cout << "Condvar time in ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count() << std::endl;
    }
    catch (const error &ex) {
        std::cerr << ex.what() << std::endl;
    }
    return 0;
}

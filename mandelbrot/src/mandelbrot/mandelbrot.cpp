#include "mandelbrot.hpp"

#include <functional>
#include <thread>

std::complex<double> f(std::complex<double> z, std::complex<double> c) {
    return z * z + c;
}

/*
 * Number of iteration to diverge.
 * max_iteration result means no diverge.
 */
size_t diverge(const std::complex<double> &c, size_t max_iteration, double max_length) {
    std::complex<double> z = 0;

    size_t iteration = 0;
    for (; iteration < max_iteration; iteration++) {
        z = f(z, c);

        if (std::abs(z) > max_length) {
            break;
        }
    }

    return iteration;
}

std::vector<size_t> mandelbrot::calculate(
        const std::vector<std::complex<double>> &points,
        size_t max_iteration,
        double max_length,
        size_t threads
) {
    size_t size = points.size();
    // Ceil the block_size value. So thread usage is at most threads variable.
    size_t block_size = (size + threads - 1) / threads;
    size_t threads_cnt = (size + block_size - 1) / block_size;

    std::vector<size_t> result(size); // It's better to allocate results in advance.
    std::vector<std::thread> all_threads;
    all_threads.reserve(threads_cnt);

    auto process_points = [&](size_t l, size_t r) {
        for (size_t i = l; i < r; ++i) {
            result[i] = diverge(points[i], max_iteration, max_length);
        }
    };

    for (size_t l = 0; l < size; l += block_size) {
        auto r = std::min(l + block_size, size);
        all_threads.emplace_back(process_points, l, r);
    }
    for (auto& thread : all_threads) {
        thread.join();
    }

    return result;
}

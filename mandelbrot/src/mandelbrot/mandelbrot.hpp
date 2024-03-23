#pragma once

#include <vector>
#include <complex>

namespace mandelbrot {
    std::vector<size_t> calculate(
            const std::vector<std::complex<double>> &points,
            size_t max_iteration,
            double max_length,
            size_t threads
    );
}

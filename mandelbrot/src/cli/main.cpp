#include "mandelbrot.hpp"

#include <boost/program_options.hpp>
#include <png++/png.hpp>
#include <chrono>

using namespace boost::program_options;
using namespace std::chrono;

struct points_t {
    std::vector<std::complex<double>> points;
    size_t w;
    size_t h;
};

points_t generate_points(size_t density) {
    double x_begin = -2;
    double x_end = 1;

    double y_begin = -1;
    double y_end = 1;

    double step = 1. / static_cast<double>(density);
    auto w = static_cast<size_t>((x_end - x_begin) * static_cast<double>(density));
    auto h = static_cast<size_t>((y_end - y_begin) * static_cast<double>(density));

    std::vector<std::complex<double>> points;
    points.reserve(w * h);

    // row wise
    for (size_t i = 0; i < h; i++) {
        double y = y_begin + static_cast<double>(i) * step;
        for (size_t j = 0; j < w; j++) {
            double x = x_begin + static_cast<double>(j) * step;
            points.emplace_back(x, y);
        }
    }

    return {std::move(points), w, h};
}

void save_picture(
        const std::string &filename,
        const std::vector<size_t> &mandelbrot,
        size_t w,
        size_t h,
        size_t max_iteration) {
    png::image<png::rgb_pixel> image(static_cast<png::uint_32>(w), static_cast<png::uint_32>(h));

    for (size_t i = 0; i < h; i++) {
        for (size_t j = 0; j < w; j++) {
            size_t value = mandelbrot[w * i + j];
            if (value == max_iteration) {
                image[i][j] = png::rgb_pixel(0, 0, 0);
            } else {
                image[i][j] = png::rgb_pixel(255, 255, 255);
            }
        }
    }

    image.write(filename);
}

void generate_mandelbrot(size_t threads, size_t density, const std::string &filename) {
    constexpr size_t max_iteration = 500;
    constexpr double max_length = 2.0;

    auto[points, w, h] = generate_points(density);

    auto t1 = high_resolution_clock::now();
    auto mandelbrot = mandelbrot::calculate(points, max_iteration, max_length, threads);
    auto t2 = high_resolution_clock::now();

    std::cout << "Time to compute: " << duration_cast<milliseconds>(t2 - t1).count() << " ms" << std::endl;

    save_picture(filename, mandelbrot, w, h, max_iteration);
}

int main(int argc, char **argv) {
    try {
        options_description desc{"Options"};

        desc.add_options()
                ("threads", value<size_t>()->default_value(1), "Threads to calculate")
                ("density", value<size_t>()->default_value(700), "Points density")
                ("file", value<std::string>()->default_value("img.png"), "Out file");

        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        generate_mandelbrot(
                vm["threads"].as<size_t>(),
                vm["density"].as<size_t>(),
                vm["file"].as<std::string>()
        );
    }
    catch (const error &ex) {
        std::cerr << ex.what() << std::endl;
    }
}

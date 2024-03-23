cmake_minimum_required(VERSION 3.10)
project(hw1)

include(cmake/conan.cmake)
conan_cmake_run(CONANFILE conanfile.py
        BASIC_SETUP CMAKE_TARGETS NO_OUTPUT_DIRS
        BUILD missing)

set(CMAKE_CXX_STANDARD 17)

add_library(mandelbrot src/mandelbrot/mandelbrot.cpp)
target_include_directories(mandelbrot PUBLIC src/mandelbrot)

add_executable(mandelbrot-cli src/cli/main.cpp)
target_link_libraries(mandelbrot-cli mandelbrot)
target_link_libraries(mandelbrot-cli CONAN_PKG::boost)
target_link_libraries(mandelbrot-cli CONAN_PKG::pngpp)

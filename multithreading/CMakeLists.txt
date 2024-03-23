cmake_minimum_required(VERSION 3.10)
project(hw1)

include(cmake/conan.cmake)
conan_cmake_run(CONANFILE conanfile.py
        BASIC_SETUP CMAKE_TARGETS NO_OUTPUT_DIRS
        BUILD missing)

set(CMAKE_CXX_STANDARD 17)

add_library(spinlock src/spinlock/spinlock.cpp)
target_include_directories(spinlock PUBLIC src/spinlock)

add_library(queue src/queue/queue.cpp)
target_include_directories(queue PUBLIC src/queue)
target_link_libraries(queue PUBLIC spinlock)

add_library(benchmark src/benchmark/benchmark.cpp)
target_include_directories(benchmark PUBLIC src/benchmark)
target_link_libraries(benchmark PUBLIC queue)

add_executable(benchmark-cli src/cli/main.cpp)
target_link_libraries(benchmark-cli benchmark)
target_link_libraries(benchmark-cli CONAN_PKG::boost)

enable_testing()
add_subdirectory(test)

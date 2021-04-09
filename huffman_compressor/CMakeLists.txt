cmake_minimum_required(VERSION 3.13)
project(cpp2018_4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")

add_library(huf-lib SHARED huffman_library/huffman_tree.h huffman_library/huffman_tree.cpp
        huffman_library/encoder.h huffman_library/encoder.cpp
        huffman_library/decoder.h huffman_library/decoder.cpp)
add_library(huf-file SHARED
        huffman_file_working/fwriter.h huffman_file_working/fwriter.cpp
        huffman_file_working/freader.h huffman_file_working/freader.cpp)
add_library(huf-comp SHARED
        huffman_utility/compressor.h huffman_utility/compressor.cpp)

add_executable(testing huffman_testing/testing.cpp)
target_link_libraries(testing huf-comp)
target_link_libraries(testing huf-file)
target_link_libraries(testing huf-lib)

add_executable(compressor huffman_utility/runner.cpp)
target_link_libraries(compressor huf-comp)
target_link_libraries(compressor huf-file)
target_link_libraries(compressor huf-lib)

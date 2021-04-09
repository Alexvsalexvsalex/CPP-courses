#ifndef TESTING_H
#define TESTING_H

#include "../huffman_utility/compressor.h"
#include "../huffman_file_working/freader.h"
#include "../huffman_file_working/fwriter.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <iomanip>

const size_t TEST_COUNT = 28;
const char *source_file = "testing.source";
const char *compressed_file = "testing.compressed";
const char *decompressed_file = "testing.decompressed";

int main();

void random_test(const char *file_name, std::pair<size_t, size_t> test_info);

void defined_test(const char *file_name, const char *str);

void test_failed(const char *message);

void test_passed(size_t encoding_time, size_t decoding_time, size_t size_unpacked, size_t size_packed);

template<typename F, typename T>
void test(F test_generate, T arg);

bool compare_files(const char *file_name1, const char *file_name2);

size_t file_size(const char *file_name);

void welcome();

void success();

void out_test_info(size_t text_size, size_t alphabet_size);

#endif //TESTING_H
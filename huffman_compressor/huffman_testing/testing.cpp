#include "testing.h"

int main() {
    welcome();
    test(defined_test, "");
    test(defined_test, "A");
    test(defined_test, "cpp-course is top");
    test(defined_test, "Bla-bla-bla");
    test(defined_test, "1234567890-=qwertyuiop[]asdfghjkl;"
                       "'zxcvbnm,./!@#$%^&*()_+QWERTYUIOP{}"
                       "ASDFGHJKL:ZXCVBNM<>?~`йцукенгшщзхъфы"
                       "вапролджэячсмитьбю.");
    for (size_t i = 0, text_size = 1, alphabet_size = 1;
         i < TEST_COUNT; ++i, text_size *= 2, alphabet_size += 2) {
        test(random_test, std::pair<size_t, size_t>(text_size, alphabet_size));
    }
    success();
    return 0;
}

template<typename F, typename T>
void test(F test_generate, T arg) {
    try {
        size_t unpacked_size = 0;
        size_t packed_size = 0;

        test_generate(source_file, arg);

        unpacked_size = file_size(source_file);
        auto start = std::chrono::high_resolution_clock::now();

        compress(source_file, compressed_file);

        auto encode_finish = std::chrono::high_resolution_clock::now();

        decompress(compressed_file, decompressed_file);

        auto decode_finish = std::chrono::high_resolution_clock::now();

        packed_size = file_size(compressed_file);

        if (compare_files(source_file, decompressed_file)) {
            test_passed(std::chrono::duration_cast<std::chrono::milliseconds>(encode_finish - start).count(),
                        std::chrono::duration_cast<std::chrono::milliseconds>(decode_finish - encode_finish).count(),
                        unpacked_size,
                        packed_size);
        } else {
            test_failed("DECODED FILE DOESN\'T EQUAL TO SOURCE ONE");
        }
    } catch (std::exception &exc) {
        test_failed("FATAL ERROR WHILE TESTING");
    }
}

void welcome() {
    std::cout << "Welcome to the testing system" << std::endl;
    std::cout << std::endl;
}

void test_failed(const char *message) {
    throw std::runtime_error(message);
}

void test_passed(size_t encoding_time, size_t decoding_time, size_t size_unpacked, size_t size_packed) {
    std::cout << "TEST PASSED :)" << std::endl;
    std::cout << "ENCODING TIME: " << encoding_time << " ms" << std::endl;
    std::cout << "DECODING TIME: " << decoding_time << " ms" << std::endl;
    std::cout << "SIZE BEFORE PACKING: " << std::fixed << (double) size_unpacked / 1024 / 1024 << " mb" << std::endl;
    std::cout << "SIZE AFTER PACKING: " << std::fixed << (double) size_packed / 1024 / 1024 << " mb" << std::endl;
    std::cout << std::endl;
}

bool compare_files(const char *file_name1, const char *file_name2) {
    freader fr1(file_name1);
    freader fr2(file_name2);
    while (true) {
        size_t sz1 = fr1.fill_buffer();
        size_t sz2 = fr2.fill_buffer();
        if (sz1 != sz2) {
            return false;
        }
        if (sz1 == 0) {
            return true;
        }
        for (size_t i = 0; i < sz1; ++i) {
            if (fr1.get_buffer()[i] != fr2.get_buffer()[i]) {
                return false;
            }
        }
    }
}

size_t file_size(const char *file_name) {
    FILE *file = std::fopen(file_name, "rb");
    std::fseek(file, 0, SEEK_END);
    size_t length = std::ftell(file);
    std::fclose(file);
    return length;
}

void random_test(const char *file_name, std::pair<size_t, size_t> test_info) {
    size_t text_size = test_info.first;
    size_t alphabet_size = test_info.second;
    out_test_info(text_size, alphabet_size);
    fwriter fw(file_name);
    for (size_t i = 0; i < text_size; ++i) {
        fw.write(rand() % alphabet_size);
    }
}

void defined_test(const char *file_name, const char *str) {
    out_test_info(strlen(str), 256);
    fwriter fw(file_name);
    fw.write(str);
}

void out_test_info(size_t text_size, size_t alphabet_size) {
    std::cout << "TEXT SIZE: " << text_size << "; ALPHABET_SIZE: " << alphabet_size << std::endl;
    std::cout << "TESTING..." << std::endl;
}

void success() {
    std::cout << "Congratulations, tests were passed!!!" << std::endl;
}

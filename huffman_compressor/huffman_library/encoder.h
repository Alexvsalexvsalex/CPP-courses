#ifndef ENCODER_H
#define ENCODER_H

#include "huffman_tree.h"
#include <cstddef>
#include <cstring>
#include <utility>
#include <queue>
#include <vector>

struct encoder {
    encoder();

    encoder(encoder &) = delete;

    encoder &operator=(const encoder &) = delete;

    void add_data(size_t data_size, const char *data);

    void clear_data();

    std::vector<char> get_service_information();

    void update_state();

    std::vector<bool> code_data(size_t data_size, const char *data);

private:
    size_t counter[ALPHABET_SIZE];
    std::vector<std::vector<bool>> codes;
};

#endif //ENCODER_H
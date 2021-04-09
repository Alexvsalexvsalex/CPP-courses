#include "encoder.h"

encoder::encoder() {
    codes.resize(ALPHABET_SIZE);
    for (size_t i = 0; i < ALPHABET_SIZE; ++i) {
        counter[i] = 0;
    }
}

void encoder::add_data(size_t data_size, const char *data) {
    for (size_t pos = 0; pos < data_size; ++pos) {
        ++counter[(unsigned char) data[pos]];
    }
}

void encoder::clear_data() {
    for (size_t a = 0; a < ALPHABET_SIZE; ++a) {
        counter[a] = 0;
    }
}

void encoder::update_state() {
    huffman_tree tree(counter);
    codes = tree.get_codes();
}

std::vector<char> encoder::get_service_information() {
    std::vector<char> s_inf;
    size_t n = 0;
    for (uint16_t i = 0; i < ALPHABET_SIZE; ++i) {
        n += !codes[i].empty();
    }
    s_inf.push_back(n > 255);
    s_inf.push_back(n);
    uint8_t last_block_size = 0;
    for (uint16_t i = 0; i < ALPHABET_SIZE; ++i) {
        if (codes[i].empty()) {
            continue;
        }
        s_inf.push_back(i);
        s_inf.push_back(codes[i].size());
        uint8_t micro_buffer = 0;
        uint8_t micro_buffer_counter = 0;
        for (bool bit : codes[i]) {
            ++micro_buffer_counter;
            micro_buffer = (micro_buffer << 1) + bit;
            if (micro_buffer_counter == 8) {
                s_inf.push_back(micro_buffer);
                micro_buffer_counter = 0;
                micro_buffer = 0;
            }
        }
        if (micro_buffer_counter > 0) {
            s_inf.push_back(micro_buffer);
        }
        last_block_size += codes[i].size() * counter[i];
    }
    last_block_size &= 7;
    if (last_block_size == 0) {
        last_block_size = 8;
    }
    s_inf.push_back(last_block_size);
    return s_inf;
}

std::vector<bool> encoder::code_data(size_t data_size, const char *data) {
    std::vector<bool> coded_data;
    for (size_t pos = 0; pos < data_size; ++pos) {
        u_char c = data[pos];
        for (bool bit : codes[c]) {
            coded_data.push_back(bit);
        }
    }
    return coded_data;
}

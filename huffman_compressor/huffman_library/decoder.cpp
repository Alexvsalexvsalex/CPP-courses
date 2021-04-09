#include "decoder.h"

decoder::decoder() {
    last_node = (void *) tree.root;
}

void decoder::check_final_iterator_pos() {
    if (last_node != tree.root) {
        throw std::runtime_error("Strange situation while decoding file");
    }
}

void decoder::add_code(const char &c, const std::vector<bool> &code) {
    tree.add_code(c, code);
}

std::vector<char> decoder::data_decode(const size_t &data_size, const char *data) {
    std::vector<bool> parsed_data;
    for (size_t i = 0; i < data_size; ++i) {
        for (size_t p = 7; p < 8; --p) {
            parsed_data.push_back((data[i] >> p) & 1);
        }
    }
    return tree.move(last_node, parsed_data);
}

std::vector<char> decoder::part_byte_decode(const size_t &bits, const char *byte) {
    std::vector<bool> parsed_data;
    for (size_t p = bits - 1; p < bits; --p) {
        parsed_data.push_back((*byte >> p) & 1);
    }
    return tree.move(last_node, parsed_data);
}

void decoder::check_tree() {
    tree.check_tree();
}

decoder::~decoder() = default;

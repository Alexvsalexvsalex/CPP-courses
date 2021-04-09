#ifndef DECODER_H
#define DECODER_H

#include <vector>
#include <stdexcept>
#include "huffman_tree.h"

struct decoder {
public:
    decoder(decoder &) = delete;

    decoder &operator=(const decoder &) = delete;

    void add_code(const char &c, const std::vector<bool> &code);

    decoder();

    ~decoder();

    std::vector<char> data_decode(const size_t &data_size, const char *data);

    std::vector<char> part_byte_decode(const size_t &bits, const char *byte);

    void check_final_iterator_pos();

    void check_tree();

private:
    huffman_tree tree;
    void *last_node;
};

#endif //DECODER_H
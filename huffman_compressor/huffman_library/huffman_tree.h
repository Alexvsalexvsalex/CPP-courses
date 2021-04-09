#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

#include <cstddef>
#include <cstring>
#include <cstdint>
#include <utility>
#include <queue>
#include <vector>
#include <stdexcept>

const size_t ALPHABET_SIZE = UINT8_MAX + 1;

struct huffman_tree {
private:
    struct node {
        char c = '\0';
        node *left = nullptr;
        node *right = nullptr;

        explicit node(char c);

        node(node *left, node *right);

        ~node();
    };

    void tour(const node *v, std::vector<std::vector<bool>> &ans, std::vector<bool> &current_code);

public:

    const node *root;

    huffman_tree();

    explicit huffman_tree(size_t *counter);

    huffman_tree(huffman_tree &) = delete;

    huffman_tree &operator=(const huffman_tree&) = delete;

    ~huffman_tree();

    std::vector<std::vector<bool>> get_codes();

    void add_code(const char &, const std::vector<bool> &);

    std::vector<char> move(void *&, const std::vector<bool> &);

    void check_tree();

    void rec_check_tree(const node *v);
};

#endif //HUFFMAN_TREE_H
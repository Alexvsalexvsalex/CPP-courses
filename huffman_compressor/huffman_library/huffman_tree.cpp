#include "huffman_tree.h"

huffman_tree::huffman_tree() {
    root = new node('\0');
}

huffman_tree::huffman_tree(size_t *counter) {
    std::priority_queue<std::pair<size_t, node *>,
            std::vector<std::pair<size_t, node *>>,
            std::greater<>> queue;
    for (size_t c = 0; c < ALPHABET_SIZE; ++c) {
        if (counter[c] != 0) {
            queue.push({counter[c], new node(c)});
        }
    }
    if (queue.empty()) {
        root = new node('\0');
        return;
    } else if (queue.size() == 1) {
        root = new node(queue.top().second, new node(queue.top().second->c + 1));
        return;
    } else {
        while (queue.size() > 1) {
            std::pair<size_t, node *> n1 = queue.top();
            queue.pop();
            std::pair<size_t, node *> n2 = queue.top();
            queue.pop();
            queue.push({n1.first + n2.first, new node(n1.second, n2.second)});
        }
        root = queue.top().second;
    }
}

huffman_tree::~huffman_tree() {
    delete root;
}

void huffman_tree::tour(const node *v, std::vector<std::vector<bool>> &ans,
                        std::vector<bool> &current_code) {
    if (v->left == nullptr && v->right == nullptr) {
        ans[(unsigned char) v->c] = current_code;
    } else {
        if (v->left != nullptr) {
            current_code.push_back(false);
            tour(v->left, ans, current_code);
            current_code.pop_back();
        }
        if (v->right != nullptr) {
            current_code.push_back(true);
            tour(v->right, ans, current_code);
            current_code.pop_back();
        }
    }
}

std::vector<std::vector<bool>> huffman_tree::get_codes() {
    std::vector<std::vector<bool>> ans;
    ans.resize(ALPHABET_SIZE);
    std::vector<bool> tmp;
    tour(root, ans, tmp);
    return ans;
}

void huffman_tree::add_code(const char &c, const std::vector<bool> &v) {
    node *current = const_cast<node *>(root);
    for (bool bit : v) {
        if (!bit) {
            if (current->left == nullptr) {
                current->left = new node('\0');
            }
            current = current->left;
        } else {
            if (current->right == nullptr) {
                current->right = new node('\0');
            }
            current = current->right;
        }
    }
    current->c = c;
    if (current->left != nullptr || current->right != nullptr) {
        throw std::runtime_error("Damaged input file");
    }
}

std::vector<char> huffman_tree::move(void *&current_node, const std::vector<bool> &path) {
    node *cur_node = (node *) current_node;
    std::vector<char> res;
    for (bool bit : path) {
        if (!bit) {
            cur_node = cur_node->left;
        } else {
            cur_node = cur_node->right;
        }
        if (cur_node->left == nullptr && cur_node->right == nullptr) {
            res.push_back(cur_node->c);
            cur_node = const_cast<node *>(root);
        }
    }
    current_node = cur_node;
    return res;
}

void huffman_tree::check_tree() {
    rec_check_tree(root);
}

void huffman_tree::rec_check_tree(const node *v) {
    if ((v->left != nullptr || v->right != nullptr) && v->c != '\0') {
        throw std::runtime_error("Damaged input file");
    }
    if ((v->left == nullptr && v->right != nullptr) ||
        (v->left != nullptr && v->right == nullptr)) {
        throw std::runtime_error("Damaged input file");
    }
    if (v->left != nullptr) {
        rec_check_tree(v->left);
    }
    if (v->right != nullptr) {
        rec_check_tree(v->right);
    }
}

huffman_tree::node::node(char c) {
    this->c = c;
}

huffman_tree::node::node(huffman_tree::node *left, huffman_tree::node *right) {
    this->left = left;
    this->right = right;
}

huffman_tree::node::~node() {
    delete left;
    delete right;
}

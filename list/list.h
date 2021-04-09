#ifndef EXAM2_LIST_H
#define EXAM2_LIST_H

#include <stdexcept>
#include <iostream>

template<typename T>
struct list {
    typedef T value_type;

    struct node_base {
    public:
        node_base *left;
        node_base *right;

        node_base() : left(nullptr), right(nullptr) {}

        node_base(node_base *l, node_base *r) : left(l), right(r) {}

        node_base(node_base const &oth) = default;

        node_base &operator=(node_base const &oth) = default;

        ~node_base() = default;
    };

    struct node : public node_base {
    public:
        value_type value;

        node() = delete;

        node(node_base *l, node_base *r, value_type const &val) : node_base(l, r), value(val) {}

        node(node const &oth) = delete;

        node &operator=(node const &oth) = delete;

        ~node() = default;
    };

    template<typename V>
    struct my_iterator : public std::iterator<std::bidirectional_iterator_tag, V> {
        my_iterator() = delete;

        my_iterator(nullptr_t) = delete;

        my_iterator(node_base *elem) {
            cur = elem;
        }

        my_iterator(node_base const *elem) {
            cur = const_cast<node_base *>(elem);
        }

        my_iterator(my_iterator<T> const &oth) {
            cur = oth.get_pointer();
        }

        my_iterator(my_iterator<T const> const &oth) {
            cur = oth.get_pointer();
        }

        my_iterator &operator=(my_iterator const &oth) = default;

        ~my_iterator() = default;

        my_iterator operator++(int) {
            my_iterator oth(cur);
            cur = cur->right;
            return oth;
        }

        my_iterator &operator++() {
            cur = cur->right;
            return *this;
        }

        my_iterator operator--(int) {
            my_iterator oth(cur);
            cur = cur->left;
            return oth;
        }

        my_iterator &operator--() {
            cur = cur->left;
            return *this;
        }

        V &operator*() const {
            return static_cast<node *>(cur)->value;
        }

        V *operator->() const {
            return &static_cast<node *>(cur)->value;
        }

        friend void swap(my_iterator &i1, my_iterator &i2) {
            std::swap(i1.cur, i2.cur);
        }

        node_base * const get_pointer() const{
            return cur;
        }

    private:
        node_base *cur;
    };

    template <typename T1, typename T2>
    friend bool operator!=(my_iterator<T1> const &one, my_iterator<T2> const &oth) {
        return !(one == oth);
    }

    template <typename T1, typename T2>
    friend bool operator==(my_iterator<T1> const &one, my_iterator<T2> const &oth) {
        return one.get_pointer() == oth.get_pointer();
    }

    using iterator = my_iterator<T>;

    using const_iterator = my_iterator<T const>;

    using reverse_iterator = std::reverse_iterator<iterator>;

    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    node_base tall;

    list() : tall() {
        tall.left = tall.right = &tall;
    }

    list(list const &oth) : tall() {
        tall.left = tall.right = &tall;
        try {
            node_base *cur = oth.tall.right;
            while (cur != &oth.tall) {
                push_back(static_cast<node *>(cur)->value);
                cur = cur->right;
            }
        } catch (...) {
            clear();
            throw;
        }
    }

    ~list() {
        clear();
    }

    list &operator=(list const &oth) {
        if (this == &oth) {
            return *this;
        }
        size_t add = 0;
        size_t prev_sz = size();
        try {
            node_base *cur = oth.tall.right;
            while (cur != &oth.tall) {
                push_back(static_cast<node *>(cur)->value);
                cur = cur->right;
                ++add;
            }
        } catch (...) {
            while (add > 0) {
                pop_back();
                --add;
            }
            throw;
        }
        while (prev_sz > 0) {
            --prev_sz;
            pop_front();
        }
        return *this;
    }

    bool empty() const {
        return tall.left == &tall && tall.right == &tall;
    }

    void clear() {
        node_base *current = tall.right;
        while (current != &tall) {
            node_base *next = current->right;
            delete static_cast<node *>(current);
            current = next;
        }
        tall.left = &tall;
        tall.right = &tall;
    }

    void push_front(value_type const &elem) {
        insert(begin(), elem);
    }

    void push_back(value_type const &elem) {
        insert(end(), elem);
    }

    void pop_front() {
        erase(tall.right);
    }

    void pop_back() {
        erase(tall.left);
    }

    value_type &front() {
        return static_cast<node *>(tall.right)->value;
    }

    value_type &back() {
        return static_cast<node *>(tall.left)->value;
    }

    value_type const &front() const {
        return static_cast<node *>(tall.right)->value;
    }

    value_type const &back() const {
        return static_cast<node *>(tall.left)->value;
    }

    iterator begin() {
        return iterator(tall.right);
    }

    iterator end() {
        return iterator(&tall);
    }

    const_iterator begin() const {
        return const_iterator(tall.right);
    }

    const_iterator end() const {
        return const_iterator(&tall);
    }

    reverse_iterator rbegin() {
        return reverse_iterator(&tall);
    }

    reverse_iterator rend() {
        return reverse_iterator(tall.right);
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(&tall);
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(tall.right);
    }

    const_iterator insert(const_iterator pos, value_type const &val) {
        node_base *l = pos.get_pointer()->left;
        node_base *r = pos.get_pointer();
        node_base *e = new node(l, r, val);
        l->right = e;
        r->left = e;
        return e;
    }

    const_iterator erase(const_iterator pos) {
        node_base *del = pos.get_pointer();
        node_base *l = del->left;
        node_base *r = del->right;
        l->right = r;
        r->left = l;
        delete static_cast<node *>(del);
        return r;
    }

    size_t size() {
        size_t sz = 0;
        node_base *current = tall.right;
        while (current != &tall) {
            ++sz;
            current = current->right;
        }
        return sz;
    }

    void splice(iterator it, list &l1, iterator iterator1, iterator iterator2) {
        if (iterator1 == iterator2) {
            return;
        }
        node_base *before = it.get_pointer()->left;
        node_base *after = it.get_pointer();
        node_base *start = iterator1.get_pointer();
        node_base *bef_start = start->left;
        node_base *finish = iterator2.get_pointer()->left;
        node_base *aft_finish = finish->right;

        before->right = start;
        after->left = finish;

        aft_finish->left = bef_start;
        bef_start->right = aft_finish;

        start->left = before;
        finish->right = after;
    }

    friend void swap(list &l1, list &l2) {
        node_base &nb1 = l1.tall;
        node_base &nb2 = l2.tall;
        node_base nb3 = nb2;
        if (nb1.left != &nb1 && nb1.right != &nb1) {
            nb2.left = nb1.left;
            nb2.right = nb1.right;
            nb2.left->right = &nb2;
            nb2.right->left = &nb2;
        } else {
            nb2.left = &nb2;
            nb2.right = &nb2;
        }
        if (nb3.left != &nb2 && nb3.right != &nb2) {
            nb1.left = nb3.left;
            nb1.right = nb3.right;
            nb1.left->right = &nb1;
            nb1.right->left = &nb1;
        } else {
            nb1.left = &nb1;
            nb1.right = &nb1;
        }
    }

    friend std::ostream &operator<<(std::ostream &os, list &l) {
        os << "List - ";
        node_base *current = l.tall.right;
        while (current != &l.tall) {
            node_base *next = current->right;
            os << static_cast<node*>(current)->value << " ";
            current = next;
        }
        os << std::endl;
        return os;
    }
};

#endif //EXAM2_LIST_H

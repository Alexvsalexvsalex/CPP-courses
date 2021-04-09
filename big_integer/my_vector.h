#include <cstdint>
#include <cstddef>
#include <memory>
#include <vector>

typedef uint64_t my_type;
const size_t bitsInDigit = 64;
const my_type def0 = 0x0000000000000000;
const my_type defF = 0xffffffffffffffff;

struct my_vector {
    my_vector();

    ~my_vector();

    void push_back(my_type a);

    void resize(my_type nw_sz);

    void resize(my_type nw_sz, my_type def_fill);

    my_type &operator[](size_t pos);

    const my_type &operator[](size_t pos) const;

    my_vector& operator=(const my_vector &mv);

    size_t size() const;

private:
    union {
        my_type short_number;
        std::shared_ptr<std::vector<my_type>> array;
    };
    bool is_short_number;
    void take_own_copy();
    void force_creating_big_number();
};
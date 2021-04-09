#include <cstring>
#include <algorithm>
#include <iostream>
#include "my_vector.h"

my_vector::my_vector() {
    short_number = 0;
    is_short_number = true;
}

my_vector::~my_vector() {
    if (!is_short_number) {
        array.~shared_ptr();
    }
}

void my_vector::push_back(my_type a) {
    if (is_short_number) {
        force_creating_big_number();
    } else {
        take_own_copy();
    }
    array->push_back(a);
}

void my_vector::resize(size_t nw_sz) {
    resize(nw_sz, def0);
}

void my_vector::resize(size_t nw_sz, my_type def_fill) {
    if (nw_sz == 1) {
        if (!is_short_number) {
            my_type sn = (*array)[0];
            array.~shared_ptr();
            short_number = sn;
            is_short_number = true;
        }
    } else {
        if (is_short_number) {
            force_creating_big_number();
        } else {
            take_own_copy();
        }
        array->resize(nw_sz, def_fill);
    }
}

my_type &my_vector::operator[](size_t pos) {
    if (is_short_number) {
        return short_number;
    } else {
        take_own_copy();
        return (*array)[pos];
    }
}

my_vector &my_vector::operator=(const my_vector &mv) {
    if (!is_short_number) {
        array.~shared_ptr();
    }
    if (mv.is_short_number) {
        short_number = mv.short_number;
    } else {
        new(&array) std::shared_ptr(mv.array);
    }
    is_short_number = mv.is_short_number;
    return *this;
}

const my_type &my_vector::operator[](size_t pos) const {
    if (is_short_number) {
        return short_number;
    } else {
        return (*array)[pos];
    }
}

void my_vector::force_creating_big_number() {
    if (is_short_number) {
        new(&array) std::shared_ptr<std::vector<my_type>>(new std::vector<my_type>(1, short_number));
        is_short_number = false;
    }
}

void my_vector::take_own_copy() {
    if (!is_short_number && array.use_count() > 1) {
        array.reset(new std::vector<my_type>(*array.get()));
    }
}

size_t my_vector::size() const {
    if (is_short_number) {
        return 1;
    } else {
        return array->size();
    }
}
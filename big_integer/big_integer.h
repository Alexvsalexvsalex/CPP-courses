#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <iosfwd>
#include <my_vector.h>
#include <cstdint>
#include <cstddef>

struct big_integer {

    big_integer();

    big_integer(big_integer const &other);

    big_integer(my_type a);

    explicit big_integer(std::string const &str);

    ~big_integer();

    my_type get_digit(size_t ind) const;

    bool get_sign() const;

    big_integer &operator=(big_integer const &other);

    big_integer &operator+=(big_integer const &rhs);

    big_integer &operator-=(big_integer const &rhs);

    big_integer &operator*=(big_integer const &rhs);

    big_integer &operator/=(big_integer const &rhs);

    big_integer &operator%=(big_integer const &rhs);

    big_integer &operator&=(big_integer const &rhs);

    big_integer &operator|=(big_integer const &rhs);

    big_integer &operator^=(big_integer const &rhs);

    big_integer &operator<<=(int rhs);

    big_integer &operator>>=(int rhs);

    big_integer operator+() const;

    big_integer operator-() const;

    big_integer operator~() const;

    big_integer &operator++();

    big_integer operator++(int);

    big_integer &operator--();

    big_integer operator--(int);

    friend int64_t compare(big_integer const &a, big_integer const &b);

    friend bool operator==(big_integer const &a, big_integer const &b);

    friend bool operator!=(big_integer const &a, big_integer const &b);

    friend bool operator<(big_integer const &a, big_integer const &b);

    friend bool operator>(big_integer const &a, big_integer const &b);

    friend bool operator<=(big_integer const &a, big_integer const &b);

    friend bool operator>=(big_integer const &a, big_integer const &b);

    friend std::string to_string(big_integer const &a);

    size_t size() const;

    my_type get_empty_block() const;

    my_type get_block_by_sign(bool sign) const;

private:

    my_vector digits;

    void set_digit(size_t ind, my_type val);

    void fix_size();

    friend int64_t
    compare(big_integer const &a, big_integer const &b, size_t a_l, size_t a_r, size_t b_l, size_t b_r, bool abs);

    static big_integer mul_to_short(const big_integer& a, uint64_t b);

    static big_integer div_to_short(const big_integer& a, uint64_t b);
};


big_integer operator+(big_integer a, big_integer const &b);

big_integer operator-(big_integer a, big_integer const &b);

big_integer operator*(big_integer a, big_integer const &b);

big_integer operator/(big_integer a, big_integer const &b);

big_integer operator%(big_integer a, big_integer const &b);

big_integer operator&(big_integer a, big_integer const &b);

big_integer operator|(big_integer a, big_integer const &b);

big_integer operator^(big_integer a, big_integer const &b);

big_integer operator<<(big_integer a, int b);

big_integer operator>>(big_integer a, int b);

bool operator==(big_integer const &a, big_integer const &b);

bool operator!=(big_integer const &a, big_integer const &b);

bool operator<(big_integer const &a, big_integer const &b);

bool operator>(big_integer const &a, big_integer const &b);

bool operator<=(big_integer const &a, big_integer const &b);

bool operator>=(big_integer const &a, big_integer const &b);

std::string to_string(big_integer const &a);

std::ostream &operator<<(std::ostream &s, big_integer const &a);

#endif // BIG_INTEGER_H

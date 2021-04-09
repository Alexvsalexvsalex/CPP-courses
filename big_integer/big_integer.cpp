#include "big_integer.h"
#include <iostream>

#include <cstring>
#include <stdexcept>

big_integer::big_integer() {
    digits.resize(1);
    digits[0] = def0;
}

big_integer::big_integer(big_integer const &other) {
    digits = other.digits;
}

big_integer::big_integer(my_type a) {
    digits.resize(1);
    digits[0] = a;
}

big_integer::big_integer(std::string const &str) {
    digits.resize(1);
    std::string my_copy_str = str;
    bool s = (my_copy_str[0] == '-');
    size_t startPos = s, len = my_copy_str.size();
    bool f = true;
    while (startPos < len) {
        my_type block = 0;
        for (size_t j = 0; j < bitsInDigit; ++j) {
            my_type bit = 0;
            if (startPos < len) {
                bit = (my_copy_str[len - 1U] - 48U) & 1U;
                my_type num = 0;
                for (size_t pos = startPos; pos < len; ++pos) {
                    num = 10 * num + (my_copy_str[pos] - '0');
                    my_copy_str[pos] = (num >> 1U) + 48U;
                    num &= 1U;
                }
                while (startPos < len && my_copy_str[startPos] == '0') {
                    ++startPos;
                }
            } else {
                break;
            }
            block |= (bit << j);
        }
        if (f) {
            digits[0] = block;
            f = false;
        } else {
            digits.push_back(block);
        }
    }
    if (get_sign()) {
        digits.push_back(0);
    }
    if (s) {
        *this = -*this;
    }
}

big_integer::~big_integer() {

}

my_type big_integer::get_digit(size_t ind) const {
    return ind < size() ? digits[ind] : get_empty_block();
}

void big_integer::set_digit(size_t ind, my_type val) {
    if (ind >= size()) {
        digits.resize(ind + 1, get_empty_block());
    }
    digits[ind] = val;
}

big_integer &big_integer::operator=(big_integer const &other) {
    digits = other.digits;
    return *this;
}

big_integer &big_integer::operator+=(big_integer const &rhs) {
    bool a_sign = get_sign(), b_sign = rhs.get_sign();
    size_t a_sz = size(), b_sz = rhs.size();
    my_type p = 0;
    digits.resize(a_sz > b_sz ? a_sz : b_sz);
    for (size_t i = 0; i < b_sz; ++i) {
        my_type fp = i < a_sz ? get_digit(i) : get_block_by_sign(a_sign), sp = rhs.get_digit(i);
        my_type resp = fp + sp + p;
        if ((UINT64_MAX == sp && (fp > 0 || p > 0)) || (UINT64_MAX - fp < sp + p)) {
            p = 1;
        } else {
            p = 0;
        }
        set_digit(i, resp);
    }
    size_t b_def = rhs.get_empty_block();
    for (size_t i = b_sz; i < a_sz; ++i) {
        my_type fp = get_digit(i);
        my_type resp = fp + b_def + p;
        if ((UINT64_MAX == b_def && (fp > 0 || p > 0)) || (UINT64_MAX - fp < b_def + p)) {
            p = 1;
        } else {
            p = 0;
        }
        set_digit(i, resp);
    }
    my_type p_to_sign_bit = ((p << 1U) +
                             get_sign() - a_sign - b_sign) == 1U;
    if (p != p_to_sign_bit) {
        // Overflow
        set_digit(size(), get_block_by_sign(p));
    }
    fix_size();
    return *this;
}

big_integer &big_integer::operator-=(big_integer const &rhs) {
    return *this += (-rhs);
}

/*
    Booth's multiplication algorithm

    big_integer &big_integer::operator*=(big_integer const &rhs) {
    size_t b_bit_sz = rhs.size() * bitsInDigit;
    big_integer k = (*this << (b_bit_sz + 1));
    big_integer s = ((-*this) << (b_bit_sz + 1));
    *this = rhs;
    digits.push_back(0);
    *this <<= 1;
    for (size_t i = 0; i < b_bit_sz; ++i) {
        my_type sw = (get_digit(0) & 3U);
        if (sw == 1) {
            *this += k;
        } else if (sw == 2) {
            *this += s;
        }
        *this >>= 1;
    }
    *this >>= 1;
    fix_size();
    return *this;
}*/

big_integer big_integer::mul_to_short(const big_integer &a, uint64_t b) {
    big_integer res;
    size_t a_sz = a.size();
    res.digits.resize(a_sz + 1);
    my_type p = 0;
    for (size_t pos = 0; pos < a_sz; ++pos) {
        my_type mul1 = b;
        my_type mul2 = a.get_digit(pos);
        my_type res_part1 = 0, res_part2 = 0;
        asm("xor %%rdx, %%rdx\n\t"
            "mov %2, %%rax\n\t"
            "mul %3\n\t":
        "=a" (res_part2), "=d" (res_part1):
        "c" (mul1), "b" (mul2):);
        if (UINT64_MAX - p < res_part2) {
            ++res_part1;
        }
        res_part2 += p;
        p = res_part1;
        res.set_digit(pos, res_part2);
    }
    res.set_digit(a_sz, p);
    res.fix_size();
    return res;
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    bool sign = get_sign() ^rhs.get_sign();
    if (get_sign()) {
        *this = -*this;
    }
    big_integer b = rhs;
    if (b.get_sign()) {
        b = -b;
    }
    size_t b_sz = b.size();
    big_integer res;
    for (size_t pos_b = 0; pos_b < b_sz; ++pos_b) {
        res += mul_to_short(*this, b.get_digit(pos_b)) << (pos_b * bitsInDigit);
    }
    if (sign) {
        *this = -res;
    } else {
        *this = res;
    }
    fix_size();
    return *this;
}

big_integer big_integer::div_to_short(const big_integer &a, uint64_t div) {
    big_integer res;
    my_type rem = 0;
    size_t a_sz = a.size();
    for (size_t i = a_sz - 1; i < a_sz; --i) {
        my_type dig = a.get_digit(i);
        my_type q_div = 0;
        my_type r_div = 0;
        asm("mov %2, %%rax\n\t"
            "mov %3, %%rdx\n\t"
            "div %4\n\t":
        "=a" (q_div), "=d" (r_div):
        "r" (dig), "r" (rem), "r" (div) :);
        res.set_digit(i, q_div);
        rem = r_div;
    }
    return res;
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    big_integer b = rhs;
    bool a_sign = (*this).get_sign(), b_sign = b.get_sign();
    bool sign = a_sign ^b_sign;
    if (a_sign) {
        *this = -*this;
    }
    if (b_sign) {
        b = -b;
    }
    size_t b_sz = b.size();
    if (compare(*this, b) < 0) {
        return *this = 0;
    }
    big_integer res;
    if (b_sz == 1) {
        res = div_to_short(*this, b.get_digit(0));
        res.digits.push_back(0);
    } else {
        my_type lst_dig_b = b.get_digit(b_sz - 1);
        my_type f = INT32_MAX > lst_dig_b ? INT32_MAX / (lst_dig_b + 1) : 1;
        *this = mul_to_short(*this, f);
        b = mul_to_short(b, f);
        size_t a_sz = (*this).size();
        big_integer largest_two_ull_in_div = b >> (bitsInDigit * (b_sz - 2));
        for (size_t i = a_sz - b_sz; i < a_sz - b_sz + 1; --i) {
            my_type bin_s_l = 0, bin_s_r = UINT64_MAX;
            while (bin_s_r - bin_s_l > 1) {
                my_type bin_s_m = bin_s_l / 2 + bin_s_r / 2 + (bin_s_l & bin_s_r & 1U);
                big_integer mid = bin_s_m;
                if (bin_s_m > INT64_MAX) {
                    mid.digits.push_back(0);
                }
                big_integer checking = largest_two_ull_in_div * mid;
                if (compare(*this, checking, i + b_sz - 2, i + b_sz + 1, 0, 3, true) >= 0) {
                    bin_s_l = bin_s_m;
                } else {
                    bin_s_r = bin_s_m;
                }
            }
            big_integer sub = mul_to_short(b, bin_s_l);
            sub.digits.push_back(0);
            if (compare(*this, sub, i, i + b_sz + 1, 0, sub.size(), true) < 0) {
                sub -= b;
                --bin_s_l;
            } else if (bin_s_r == UINT64_MAX && compare(*this, sub + b, i, i + b_sz + 1, 0, sub.size(), true) >= 0) {
                sub += b;
                ++bin_s_l;
            }
            res.set_digit(i, bin_s_l);
            *this -= sub << (i * bitsInDigit);
        }
    }
    res.fix_size();
    if (sign) {
        return *this = -res;
    } else {
        return *this = res;
    }
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    return *this -= (*this / rhs) * rhs;
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    size_t a_sz = size(), b_sz = rhs.size();
    size_t mx_sz = a_sz > b_sz ? a_sz : b_sz;
    for (size_t i = 0; i < mx_sz; ++i) {
        set_digit(i, get_digit(i) & rhs.get_digit(i));
    }
    fix_size();
    return *this;
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    size_t a_sz = size(), b_sz = rhs.size();
    size_t mx_sz = a_sz > b_sz ? a_sz : b_sz;
    for (size_t i = 0; i < mx_sz; ++i) {
        set_digit(i, get_digit(i) | rhs.get_digit(i));
    }
    fix_size();
    return *this;
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    size_t a_sz = size(), b_sz = rhs.size();
    size_t mx_sz = a_sz > b_sz ? a_sz : b_sz;
    for (size_t i = 0; i < mx_sz; ++i) {
        set_digit(i, get_digit(i) ^ rhs.get_digit(i));
    }
    fix_size();
    return *this;
}

big_integer &big_integer::operator<<=(int rhs) {
    if (rhs == 0) {
        return *this;
    }
    size_t shiftBlocks = rhs / bitsInDigit;
    size_t shiftBits = rhs % bitsInDigit;
    size_t res_size = size() + shiftBlocks + (shiftBits > 0);
    digits.resize(res_size, get_empty_block());
    my_type def_block = def0;
    for (size_t i = res_size - 1; i < res_size; --i) {
        my_type cur_block;
        my_type prv_block;
        if (i >= shiftBlocks) {
            cur_block = get_digit(i - shiftBlocks);
            if (i >= shiftBlocks + 1) {
                prv_block = get_digit(i - shiftBlocks - 1);
            } else {
                prv_block = def_block;
            }
        } else {
            cur_block = def_block;
            prv_block = def_block;
        }
        set_digit(i, (cur_block << shiftBits) + (shiftBits == 0 ? 0 : prv_block >> (bitsInDigit - shiftBits)));
    }
    fix_size();
    return *this;
}

big_integer &big_integer::operator>>=(int rhs) {
    if (rhs == 0) {
        return *this;
    }
    size_t shiftBlocks = rhs / bitsInDigit;
    size_t shiftBits = rhs % bitsInDigit;
    my_type def_block = get_empty_block();
    size_t sz = size();
    for (size_t i = 0; i < sz; ++i) {
        my_type cur_block;
        my_type nxt_block;
        if (i + shiftBlocks < sz) {
            cur_block = get_digit(i + shiftBlocks);
            if (i + shiftBlocks + 1 < sz) {
                nxt_block = get_digit(i + shiftBlocks + 1);
            } else {
                nxt_block = def_block;
            }
        } else {
            cur_block = def_block;
            nxt_block = def_block;
        }
        set_digit(i, (shiftBits == 0 ? 0 : nxt_block << (bitsInDigit - shiftBits)) + (cur_block >> shiftBits));
    }
    fix_size();
    return *this;
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    return ++(~(*this));
}

big_integer big_integer::operator~() const {
    big_integer r = *this;
    for (size_t i = 0; i < r.size(); ++i) {
        r.digits[i] = ~r.get_digit(i);
    }
    return r;
}

big_integer &big_integer::operator++() {
    return *this = *this + 1;
}

big_integer big_integer::operator++(int) {
    big_integer r = *this;
    ++*this;
    return r;
}

big_integer &big_integer::operator--() {
    return *this = *this - 1;
}

big_integer big_integer::operator--(int) {
    big_integer r = *this;
    --*this;
    return r;
}

size_t big_integer::size() const {
    return digits.size();
}

my_type big_integer::get_block_by_sign(bool sign) const {
    return sign ? defF : def0;
}

my_type big_integer::get_empty_block() const {
    return get_block_by_sign(get_sign());
}

big_integer operator+(big_integer a, big_integer const &b) {
    return a += b;
}

big_integer operator-(big_integer a, big_integer const &b) {
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const &b) {
    return a *= b;
}

big_integer operator/(big_integer a, big_integer const &b) {
    return a /= b;
}

big_integer operator%(big_integer a, big_integer const &b) {
    return a %= b;
}

big_integer operator&(big_integer a, big_integer const &b) {
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const &b) {
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const &b) {
    return a ^= b;
}

big_integer operator<<(big_integer a, int b) {
    return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
    return a >>= b;
}

int64_t compare(big_integer const &a, big_integer const &b) {
    return compare(a, b, 0, a.size(), 0, b.size(), false);
}

int64_t compare(big_integer const &a, big_integer const &b,
                size_t a_l, size_t a_r, size_t b_l, size_t b_r, bool abs) {
    int64_t res = 1;
    if (!abs) {
        if (a.get_sign() && b.get_sign()) {
            res = -1;
        } else if (a.get_sign() && !b.get_sign()) {
            return 1;
        } else if (!a.get_sign() && b.get_sign()) {
            return -1;
        }
    }
    my_type def_block = a.get_empty_block();
    size_t a_len = a_r - a_l, b_len = b_r - b_l;
    size_t mx_len = b_len > a_len ? b_len : a_len;
    for (size_t i = mx_len - 1; i < mx_len; --i) {
        my_type a_d = a_len > i ? a.get_digit(i + a_l) : def_block;
        my_type b_d = b_len > i ? b.get_digit(i + b_l) : def_block;
        if (a_d < b_d) {
            return -res;
        }
        if (a_d > b_d) {
            return res;
        }
    }
    return 0;
}

bool operator==(big_integer const &a, big_integer const &b) {
    return compare(a, b) == 0;
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return compare(a, b) != 0;
}

bool operator<(big_integer const &a, big_integer const &b) {
    return compare(a, b) < 0;
}

bool operator>(big_integer const &a, big_integer const &b) {
    return compare(a, b) > 0;
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return compare(a, b) <= 0;
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return compare(a, b) >= 0;
}

std::string to_string(big_integer const &a) {
    std::string tmp = "0";
    bool s = a.get_sign();
    big_integer m = a;
    if (s) {
        m = -m;
    }
    size_t a_sz = a.size();
    for (size_t i = a_sz - 1; i < a_sz; --i) {
        for (size_t j = bitsInDigit - 1; j < bitsInDigit; --j) {
            my_type p = (m.get_digit(i) >> j) & 1U;
            for (size_t pos = 0; pos < tmp.size(); ++pos) {
                my_type num = ((tmp[pos] - '0') << 1) + p;
                if (num > 9) {
                    num -= 10;
                    p = 1;
                } else {
                    p = 0;
                }
                tmp[pos] = ('0' + num);
            }
            if (p > 0) {
                tmp.push_back('0' + p);
            }
        }
    }
    if (s) {
        tmp.push_back('-');
    }

    size_t sz = tmp.length();
    for (size_t i = 0; i * 2 < sz; i++) {
        std::swap(tmp[i], tmp[sz - i - 1]);
    }
    return tmp;
}

bool big_integer::get_sign() const {
    return (digits[size() - 1U] >> (bitsInDigit - 1)) & 1U;
}

void big_integer::fix_size() {
    size_t cur_sz = size();
    size_t act_sz = 1;
    for (size_t i = cur_sz - 1; i < cur_sz; --i) {
        my_type dig = get_digit(i);
        if (dig != get_empty_block()) {
            if (get_sign() == ((dig >> (bitsInDigit - 1U)) & 1U)) {
                act_sz = i + 1;
            } else {
                act_sz = i + 2;
            }
            break;
        }
    }
    digits.resize(act_sz);
}


std::ostream &operator<<(std::ostream &s, big_integer const &a) {
    return s << to_string(a);
}

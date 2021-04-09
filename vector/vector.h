
#include <iostream>

template<typename T>
struct vector {
    // No data
    // service_info = nullptr;

    // Small data
    // Small object is valid
    // service_info = &small_obj;

    // Big data
    // capacity - service_info[0]
    // size - service_info[1]
    // count links - service_info[2]
    // data begins from service_info[3] ~ array[0]
public:
    typedef T value_type;
    typedef T *iterator;
    typedef T const *const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;


    vector() : service_info(nullptr) {}

    vector(vector const &rhs) {
        if (rhs.empty()) {
            service_info = nullptr;
        } else if (rhs.is_small()) {
            new(&small_obj) value_type(rhs.small_obj);
            service_info = (size_t *) &small_obj;
        } else {
            service_info = rhs.service_info;
            array = rhs.array;
            ++service_info[2];
        }
    }

    vector &operator=(vector const &rhs) {
        if (this == &rhs) {
            return *this;
        }
        if (rhs.empty()) {
            free_data();
            service_info = nullptr;
        } else if (rhs.is_small()) {
            if (is_null()) {
                new(&small_obj) value_type(rhs.small_obj);
                service_info = (size_t *) &small_obj;
            } else if (is_small()) {
                small_obj = rhs.small_obj;
            } else {
                size_t *save_pointer = service_info;
                new(&small_obj) value_type(rhs.small_obj);
                service_info = (size_t *) &small_obj;
                free_service_info(save_pointer);
            }
        } else {
            free_data();
            service_info = rhs.service_info;
            array = rhs.array;
            ++service_info[2];
        }
        return *this;
    }

    template<typename InputIterator>
    vector(InputIterator first, InputIterator last) : service_info(nullptr) {
        try{
            while (first != last) {
                push_back(*first);
                ++first;
            }
        } catch (...) {
            free_data();
            throw;
        }
    }

    template<typename InputIterator>
    void assign(InputIterator first, InputIterator last) {
        vector oth(first, last);
        swap(*this, oth);
    }

    value_type &operator[](size_t pos) {
        if (pos < size()) {
            if (capacity() == 1) {
                return small_obj;
            } else {
                if (take_control()) {
                    return array[pos];
                } else {
                    throw std::runtime_error("Couldn't get access for element modifying");
                }
            }
        } else {
            throw std::runtime_error("Position greater than size");
        }
    }

    value_type const &operator[](size_t pos) const {
        if (pos < size()) {
            if (capacity() == 1) {
                return small_obj;
            } else {
                return array[pos];
            }
        } else {
            throw std::runtime_error("Position greater than size");
        }
    }

    value_type &front() {
        if (!empty()) {
            return (*this)[0];
        } else {
            throw std::runtime_error("Empty vector");
        }
    }

    value_type &back() {
        if (!empty()) {
            return (*this)[size() - 1];
        } else {
            throw std::runtime_error("Empty vector");
        }
    }

    value_type const &front() const {
        if (!empty()) {
            return (*this)[0];
        } else {
            throw std::runtime_error("Empty vector");
        }
    }

    value_type const &back() const {
        if (!empty()) {
            return (*this)[size() - 1];
        } else {
            throw std::runtime_error("Empty vector");
        }
    }

    bool push_back(value_type const& obj) {
        if (is_null()) {
            new(&small_obj) value_type(obj);
            service_info = (size_t *) &small_obj;
        } else {
            if (begin() <= &obj && &obj < end()) {
                value_type K = obj;
                take_control();
                reserve(size() + 1);
                new(array + size()) value_type(K);
            } else {
                take_control();
                reserve(size() + 1);
                new(array + size()) value_type(obj);
            }
            ++service_info[1];
        }
        return true;
    }

    void pop_back() {
        if (size() > 0) {
            if (is_small()) {
                small_obj.~value_type();
                service_info = nullptr;
            } else {
                array[--service_info[1]].~value_type();
            }
        } else {
            throw std::runtime_error("Empty vector");
        }
    }

    value_type *data() {
        return begin();
    }

    value_type const *data() const {
        return begin();
    }

    bool empty() const {
        return size() == 0;
    }

    size_t size() const {
        if (is_null()) {
            return 0;
        } else if (is_small()) {
            return 1;
        } else {
            return service_info[1];
        }
    }

    void reserve(size_t cap) {
        if (cap > capacity()) {
            if (is_null() || is_small()) {
                trans_to_array(2 * cap);
            } else {
                change_array(2 * cap);
            }
        }
    }

    size_t capacity() const noexcept {
        if (is_null()) {
            return 0;
        } else if (is_small()) {
            return 1;
        } else {
            return service_info[0];
        }
    }

    void shrink_to_fit() {
        if (!is_null() && !is_small()) {
            change_array(size());
        }
    }

    void resize(size_t sz, value_type const &obj) {
        reserve(sz);
        while (sz < size()) {
            pop_back();
        }
        while (sz > size()) {
            push_back(obj);
        }
    }

    void clear() {
        while (size() > 0) {
            pop_back();
        }
    }

    ~vector() {
        free_data();
    }

    iterator begin() {
        if (is_null()) {
            return nullptr;
        }
        if (is_small()) {
            return &small_obj;
        }
        return array;
    }

    const_iterator begin() const {
        if (is_null()) {
            return nullptr;
        }
        if (is_small()) {
            return &small_obj;
        }
        return array;
    }

    iterator end() {
        if (is_null()) {
            return nullptr;
        }
        return begin() + size();
    }

    const_iterator end() const {
        if (is_null()) {
            return nullptr;
        }
        return begin() + size();
    }

    reverse_iterator rbegin() {
        if (is_null()) {
            return (reverse_iterator) nullptr;
        }
        size_t sz = size();
        if (sz == 0) {
            return (reverse_iterator) nullptr;
        } else {
            return (reverse_iterator) (end());
        }
    }

    const_reverse_iterator rbegin() const {
        if (is_null()) {
            return (const_reverse_iterator) nullptr;
        }
        size_t sz = size();
        if (sz == 0) {
            return (const_reverse_iterator) nullptr;
        } else {
            return (const_reverse_iterator) (end());
        }
    }

    reverse_iterator rend() {
        if (is_null()) {
            return (reverse_iterator) nullptr;
        }
        return rbegin() + size();
    }

    const_reverse_iterator rend() const {
        if (is_null()) {
            return (const_reverse_iterator) nullptr;
        }
        return rbegin() + size();
    }

    iterator insert(const_iterator pos, value_type const &val) {
        const_iterator finish = end();
        size_t count_after = 0;
        while (pos < finish) {
            --finish;
            ++count_after;
        }
        if (!push_back(val)) {
            return end();
        }
        iterator it = end() - 1;
        while (count_after > 0) {
            --count_after;
            std::swap(*(it - 1), *it);
            --it;
        }
        return it;
    }

    iterator erase(const_iterator pos) {
        const_iterator finish = end() - 1;
        iterator pos2 = (iterator) pos;
        iterator res = nullptr;
        while (pos2 < finish) {
            std::swap(*pos2, *(pos2 + 1));
            if (res == nullptr) {
                res = pos2;
            }
            ++pos2;
        }
        pop_back();
        return res;
    }

    iterator erase(const_iterator first, const_iterator last) {
        const_iterator finish = end();
        iterator elems_after_last = (iterator) last;
        iterator space = (iterator) first;
        size_t del_count = 0;
        while (first < last) {
            ++first;
            ++del_count;
        }
        iterator res = nullptr;
        while (elems_after_last < finish) {
            std::swap(*space, *elems_after_last);
            if (res == nullptr) {
                res = space;
            }
            ++elems_after_last;
            ++space;
        }
        while (del_count > 0) {
            pop_back();
            --del_count;
        }
        return res;
    }

    friend int64_t compare(vector const &v1, vector const &v2) {
        size_t sz1 = v1.size(), sz2 = v2.size();
        for (size_t i = 0; i < sz1 && i < sz2; ++i) {
            if (v1[i] < v2[i]) {
                return -1;
            }
            if (v2[i] < v1[i]) {
                return 1;
            }
        }
        if (sz1 < sz2) {
            return -1;
        }
        if (sz2 < sz1) {
            return 1;
        }
        return 0;
    }

    friend bool operator==(vector const &v1, vector const &v2) {
        return compare(v1, v2) == 0;
    }

    friend bool operator!=(vector const &v1, vector const &v2) {
        return compare(v1, v2) != 0;
    }

    friend bool operator>(vector const &v1, vector const &v2) {
        return compare(v1, v2) > 0;
    }

    friend bool operator<(vector const &v1, vector const &v2) {
        return compare(v1, v2) < 0;
    }

    friend bool operator>=(vector const &v1, vector const &v2) {
        return compare(v1, v2) >= 0;
    }

    friend bool operator<=(vector const &v1, vector const &v2) {
        return compare(v1, v2) <= 0;
    }

    friend void swap(vector &v1, vector &v2) {
        vector v3 = v1;
        v1 = v2;
        v2 = v3;
    }

    friend std::ostream &operator<<(std::ostream &os, vector const &v) {
        os << "Out vector: ";
        for (size_t i = 0; i < v.size(); ++i) {
            os << v[i] << " ";
        }
        os << std::endl;
        return os;
    }

private:
    union {
        value_type small_obj;
        T *array;
    };
    size_t *service_info;

    bool is_small() const {
        return (value_type *) service_info == &small_obj;
    }

    bool is_null() const {
        return service_info == nullptr;
    }

    void trans_to_array(size_t nw_cap) {
        if (is_null()) {
            void *data_2 = malloc(sizeof(size_t) * 3 + sizeof(value_type) * nw_cap);
            if (data_2 == nullptr) {
                throw std::runtime_error("Wrong allocate");
            }
            service_info = (size_t *) data_2;
            service_info[0] = nw_cap;
            service_info[1] = 0;
            service_info[2] = 1;
            array = (value_type *) (service_info + 3);
        } else if (is_small()) {
            if (nw_cap == 0) {
                nw_cap = 1;
            }
            void *data_2 = malloc(sizeof(size_t) * 3 + sizeof(value_type) * nw_cap);
            if (data_2 == nullptr) {
                throw std::runtime_error("Wrong allocate");
            }
            try {
                new((size_t *) data_2 + 3) value_type(small_obj);
            } catch (...) {
                free(data_2);
                throw std::runtime_error("Wrong creating object copy");
            }
            service_info = (size_t *) data_2;
            service_info[0] = nw_cap;
            service_info[1] = 1;
            service_info[2] = 1;
            small_obj.~value_type();
            array = (value_type *) (service_info + 3);
        }
    }

    void free_data() {
        if (is_null()) {

        } else if (is_small()) {
            small_obj.~value_type();
        } else {
            free_service_info(service_info);
        }
        service_info = nullptr;
    }

    void free_service_info(size_t* s_i) {
        if (--s_i[2] == 0) {
            size_t sz = size();
            for (size_t elem = 0; elem < sz; ++elem) {
                (*((value_type *)(s_i + 3) + elem)).~value_type();
            }
            free((void *) s_i);
        }
    }

    bool take_control() {
        if (links_count() > 1) {
            try {
                change_array(capacity());
            } catch (...) {
                return false;
            }
        }
        return true;
    }

    void change_array(size_t nw_capacity) {
        size_t *s_i = (size_t *) get_array_copy(nw_capacity);
        if (s_i == nullptr) {
            throw std::runtime_error("Wrong allocate");
        }
        value_type *arr = (value_type *) (s_i + 3);
        free_data();
        service_info = s_i;
        array = arr;
    }

    void *get_array_copy(size_t custom_capacity) {
        size_t rhs_cap = custom_capacity;
        size_t rhs_sz = size();
        if (rhs_cap < rhs_sz) {
            rhs_sz = rhs_cap;
        }
        void *data_2 = malloc(sizeof(size_t) * 3 + sizeof(value_type) * rhs_cap);
        if (data_2 == nullptr) {
            return nullptr;
        }
        size_t *s_i = (size_t *) data_2;
        value_type *arr = (value_type *) (s_i + 3);
        s_i[0] = rhs_cap;
        s_i[1] = 0;
        s_i[2] = 1;
        try {
            for (size_t elem = 0; elem < rhs_sz; ++elem) {
                new(arr + elem) value_type(array[elem]);
                ++s_i[1];
            }
        } catch (...) {
            for (size_t elem = 0; elem < s_i[1]; ++elem) {
                arr[elem].~value_type();
            }
            free(data_2);
            return nullptr;
        }
        return data_2;
    }

    size_t links_count() const {
        if (is_null() || is_small()) {
            return 1;
        } else {
            return service_info[2];
        }
    }
};
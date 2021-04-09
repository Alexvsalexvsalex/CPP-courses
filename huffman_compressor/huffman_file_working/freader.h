#ifndef FREADER_H
#define FREADER_H

#include <cstdio>
#include <cstdint>
#include <vector>
#include <stdexcept>

const size_t READ_BUFFER_SIZE = 1024 * 16;

struct freader {
private:
    FILE *file;
    size_t current_pos;
    size_t buffer_size;
    char *buffer;
public:
    freader(freader &) = delete;

    freader &operator=(const freader &) = delete;

    explicit freader(const char *file_name);

    char const *get_buffer() const {
        return buffer;
    }

    ~freader();

    void reopen(const char *file_name);

    size_t fill_buffer();

    size_t get_buffer_size();

    size_t get_current_pos();

    bool is_next_char();

    char get_next_char();

    void close();

    void open(const char *file_name);
};

#endif //FREADER_H
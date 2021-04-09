#ifndef FWRITER_H
#define FWRITER_H

#include <cstdint>
#include <cstdio>
#include <vector>
#include <stdexcept>
#include <cstring>

const size_t WRITE_BUFFER_SIZE = 1024 * 16;

struct fwriter {
private:
    FILE *file;
    size_t buffer_size;
    size_t micro_buffer_counter;
    char micro_buffer;
    char *buffer;
public:
    fwriter(fwriter &) = delete;

    fwriter&operator=(const fwriter&) = delete;

    explicit fwriter(const char *file_name);

    ~fwriter();

    void write(char c);

    void write(const std::vector<char> &c);

    void write(const std::vector<bool> &c);

    void write(const char *str);

    void flush();

    void micro_buffer_flush();

    void close();

    void open(const char *file_name);

    size_t get_buffer_size();
};

#endif //FWRITER_H
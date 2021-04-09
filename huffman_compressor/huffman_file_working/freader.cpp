#include "freader.h"

freader::freader(const char *file_name) {
    open(file_name);
    current_pos = 0;
    buffer_size = 0;
    buffer = new char[READ_BUFFER_SIZE];
}

freader::~freader() {
    close();
    delete[] buffer;
}

void freader::reopen(const char *file_name) {
    close();
    open(file_name);
    current_pos = 0;
    buffer_size = 0;
}

size_t freader::fill_buffer() {
    current_pos = 0;
    return buffer_size = fread(buffer, sizeof(char), READ_BUFFER_SIZE, file);
}

size_t freader::get_buffer_size() {
    return buffer_size;
}

bool freader::is_next_char() {
    if (current_pos == buffer_size) {
        buffer_size = fill_buffer();
    }
    return current_pos < buffer_size;
}

char freader::get_next_char() {
    if (!is_next_char()) {
        throw std::runtime_error("Error while reading input file");
    }
    return buffer[current_pos++];
}

void freader::close() {
    if (ferror(file)) {
        clearerr(file);
        throw std::runtime_error("Error in input file");
    }
    fclose(file);
}

void freader::open(const char *file_name) {
    file = std::fopen(file_name, "r");
    if (file == nullptr) {
        throw std::runtime_error("Couldn't open file");
    }
}

size_t freader::get_current_pos() {
    return current_pos;
}

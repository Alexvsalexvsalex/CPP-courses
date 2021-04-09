#include "fwriter.h"

fwriter::fwriter(const char *file_name) {
    open(file_name);
    buffer_size = 0;
    micro_buffer_counter = 0;
    micro_buffer = '\0';
    buffer = new char[WRITE_BUFFER_SIZE];
}

void fwriter::write(const std::vector<char> &c) {
    if (!c.empty()) {
        micro_buffer_flush();
        flush();
        fwrite(&c[0], sizeof(char), c.size(), file);
    }
}

void fwriter::write(char c) {
    micro_buffer_flush();
    buffer[buffer_size++] = c;
    if (buffer_size == WRITE_BUFFER_SIZE) {
        flush();
    }
}

void fwriter::flush() {
    if (buffer_size > 0) {
        fwrite(buffer, sizeof(char), buffer_size, file);
        buffer_size = 0;
    }
}

void fwriter::open(const char *file_name) {
    file = std::fopen(file_name, "w");
    if (file == nullptr) {
        throw std::runtime_error("Couldn't open file");
    }
}

fwriter::~fwriter() {
    close();
    delete[] buffer;
}

void fwriter::close() {
    micro_buffer_flush();
    flush();
    if (ferror(file)) {
        clearerr(file);
        throw std::runtime_error("Error in output file");
    }
    fclose(file);
}

size_t fwriter::get_buffer_size() {
    return buffer_size;
}

void fwriter::write(const char *c) {
    micro_buffer_flush();
    flush();
    fwrite(c, sizeof(char), strlen(c), file);
}

void fwriter::write(const std::vector<bool> &c) {
    for (bool bit : c) {
        ++micro_buffer_counter;
        micro_buffer = (micro_buffer << 1) + bit;
        if (micro_buffer_counter == 8) {
            micro_buffer_flush();
        }
    }
}

void fwriter::micro_buffer_flush() {
    if (micro_buffer_counter > 0) {
        micro_buffer_counter = 0;
        write(micro_buffer);
        micro_buffer = '\0';
    }
}


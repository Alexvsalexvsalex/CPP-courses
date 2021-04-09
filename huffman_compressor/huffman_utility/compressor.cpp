#include "compressor.h"

void compress(const char *source, const char *destination) {
    freader input(source);
    fwriter output(destination);
    // Counting
    encoder en;
    while (input.fill_buffer() > 0) {
        en.add_data(input.get_buffer_size(), input.get_buffer());
    }
    input.reopen(source);
    en.update_state();

    // Writing service information
    output.write(en.get_service_information());

    // Encoding
    while (input.fill_buffer() > 0) {
        output.write(en.code_data(input.get_buffer_size(), input.get_buffer()));
    }
    output.flush();
}

void decompress(const char *source, const char *destination) {
    freader input(source);
    fwriter output(destination);
    // Loading service information
    decoder de;
    size_t n = (unsigned char)input.get_next_char() * 256 +
            (unsigned char)input.get_next_char();
    for (size_t i = 0; i < n; ++i) {
        std::vector<bool> code;
        char c = input.get_next_char();
        size_t code_size = input.get_next_char();
        for (size_t j = 0; j < code_size; j += 8) {
            uint8_t part_code = input.get_next_char();
            size_t micro_pos = code_size - j > 8 ? 7 : code_size - j - 1;
            for (; micro_pos < 8; --micro_pos) {
                code.push_back((part_code >> micro_pos) & 1);
            }
        }
        de.add_code(c, code);
    }
    de.check_tree();
    // Decoding
    uint8_t size_last_byte = input.get_next_char();
    char last_byte;
    if (input.is_next_char()) {
        output.write(de.data_decode(input.get_buffer_size() - 1 - input.get_current_pos(),
                                    input.get_buffer() + input.get_current_pos()));
        last_byte = input.get_buffer()[input.get_buffer_size() - 1];
        while (input.fill_buffer() > 0) {
            output.write(de.data_decode(1, &last_byte));
            output.write(de.data_decode(input.get_buffer_size() - 1, input.get_buffer()));
            last_byte = input.get_buffer()[input.get_buffer_size() - 1];
        }
        output.write(de.part_byte_decode(size_last_byte, &last_byte));
    }
    de.check_final_iterator_pos();
}

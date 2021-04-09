#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <cstddef>
#include <cassert>
#include <cstdio>
#include <string>
#include <iostream>
#include "../huffman_library/encoder.h"
#include "../huffman_library/decoder.h"
#include "../huffman_file_working/freader.h"
#include "../huffman_file_working/fwriter.h"

void compress(const char *source, const char *destination);

void decompress(const char *source, const char *destination);

#endif //COMPRESSOR_H
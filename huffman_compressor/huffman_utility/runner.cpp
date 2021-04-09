#include "runner.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr <<
                  "For correct running write key -e (encoding) or -d (decoding), source and destination"
                  << std::endl;
        return 0;
    }

    try {
        std::string mode = argv[1];
        if (mode == "-e") {
            compress(argv[2], argv[3]);
        } else if (mode == "-d") {
            decompress(argv[2], argv[3]);
        } else {
            std::cerr << "Wrong key" << std::endl;
        }
    } catch (std::exception &exc) {
        std::cerr << exc.what() << std::endl;
    }
    return 0;
}
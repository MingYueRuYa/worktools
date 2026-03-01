/*
 * unzip_demo.cpp - CLI for ZIP extraction
 * Usage: unzip_demo <file.zip> [output_dir]
 */

#include "zip_extractor.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: " << (argv[0] ? argv[0] : "unzip_demo") << " <file.zip> [output_dir]\n";
        std::cerr << "Place zlib1.dll in the same directory or in PATH.\n";
        return 1;
    }

    std::string zip_path(argv[1]);
    std::string out_dir = (argc == 3) ? argv[2] : ".";

    ZipExtractor ex;
    if (!ex.load()) return 1;

    std::cout << "Extracting " << zip_path << " to " << out_dir << "\n";
    return ex.extract(zip_path, out_dir) ? 0 : 1;
}

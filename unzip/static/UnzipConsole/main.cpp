/**
 * VS2019 Win32 console - Extract ZIP using zlib/minizip.
 * Usage: UnzipConsole.exe <zip_path> [output_dir]
 */

#include "ZipExtractor.h"
#include <cstdio>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::printf("Usage: %s <zip_file> [output_dir]\n", argv[0]);
        std::printf("Example: %s test.zip\n", argv[0]);
        std::printf("         %s test.zip C:\\output\n", argv[0]);
        return 1;
    }

    std::string zipPath = argv[1];
    std::string outDir = (argc >= 3) ? argv[2] : ".";

    std::string error;
    if (!zip::Extract(zipPath, outDir, &error)) {
        std::fprintf(stderr, "Error: %s\n", error.c_str());
        return 1;
    }

    std::printf("Done.\n");
    return 0;
}

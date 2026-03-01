#pragma once

#include <memory>
#include <string>

/** ZIP extractor for Windows, using zlib1.dll loaded at runtime. */
class ZipExtractor {
public:
    ZipExtractor();
    ~ZipExtractor();

    bool load();

    bool extract(const std::string& zip_path, const std::string& out_dir = ".");

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

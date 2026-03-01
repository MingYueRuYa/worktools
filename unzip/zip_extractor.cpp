#include "zip_extractor.h"

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <cstdint>

namespace {

using Byte = unsigned char;
using uInt = unsigned int;
using uLong = unsigned long;
using voidpf = void*;
using alloc_func = void (*)(voidpf opaque, uInt items, uInt size);
using free_func = void (*)(voidpf opaque, voidpf address);

struct z_stream {
    const Byte* next_in;
    uInt avail_in;
    uLong total_in;
    Byte* next_out;
    uInt avail_out;
    uLong total_out;
    const char* msg;
    void* internal_state;
    alloc_func zalloc;
    free_func zfree;
    voidpf opaque;
    int data_type;
    uLong adler;
    uLong reserved;
};

constexpr int Z_OK = 0;
constexpr int Z_STREAM_END = 1;
constexpr int Z_NEED_DICT = 2;
constexpr int Z_DATA_ERROR = -3;
constexpr int Z_MEM_ERROR = -4;
constexpr int Z_NO_FLUSH = 0;

constexpr size_t CHUNK = 256 * 1024;

using inflateInit2_t = int (*)(z_stream* strm, int windowBits, const char* version, int stream_size);
using inflate_t = int (*)(z_stream* strm, int flush);
using inflateEnd_t = int (*)(z_stream* strm);

} // namespace

class ZlibLoader {
public:
    inflateInit2_t inflateInit2 = nullptr;
    inflate_t inflate = nullptr;
    inflateEnd_t inflateEnd = nullptr;

    bool load() {
        handle_.reset(LoadLibraryA("zlib1.dll"));
        if (!handle_) {
            std::cerr << "Error: cannot load zlib1.dll\n";
            return false;
        }
        auto* h = static_cast<HMODULE>(handle_.get());
        inflateInit2  = reinterpret_cast<inflateInit2_t>(GetProcAddress(h, "inflateInit2_"));
        inflate       = reinterpret_cast<inflate_t>(GetProcAddress(h, "inflate"));
        inflateEnd    = reinterpret_cast<inflateEnd_t>(GetProcAddress(h, "inflateEnd"));
        if (!inflateInit2 || !inflate || !inflateEnd) {
            std::cerr << "Error: cannot get inflate functions from DLL\n";
            return false;
        }
        return true;
    }

private:
    struct FreeLibraryDeleter {
        void operator()(void* h) const { if (h) FreeLibrary(static_cast<HMODULE>(h)); }
    };
    std::unique_ptr<void, FreeLibraryDeleter> handle_;
};

static int inflateBuffer(ZlibLoader& zlib, const Byte* in, size_t in_len, std::ostream& dest, int windowBits = -15) {
    std::vector<Byte> out_buf(CHUNK);
    z_stream strm{};
    strm.zalloc = nullptr;
    strm.zfree = nullptr;
    strm.opaque = nullptr;
    strm.next_in = const_cast<Byte*>(in);
    strm.avail_in = static_cast<uInt>(in_len);

    int ret = zlib.inflateInit2(&strm, windowBits, "1.2.13", static_cast<int>(sizeof(z_stream)));
    if (ret != Z_OK) return ret;

    auto cleanup = [&zlib, &strm]() { zlib.inflateEnd(&strm); };

    do {
        strm.avail_out = CHUNK;
        strm.next_out = out_buf.data();
        ret = zlib.inflate(&strm, Z_NO_FLUSH);
        if (ret == Z_NEED_DICT) ret = Z_DATA_ERROR;
        if (ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
            cleanup();
            return ret;
        }
        unsigned have = CHUNK - strm.avail_out;
        dest.write(reinterpret_cast<const char*>(out_buf.data()), have);
        if (dest.fail()) {
            cleanup();
            return -1;
        }
    } while (ret != Z_STREAM_END);

    cleanup();
    return Z_OK;
}

namespace zip {

    static void createDirectories(std::string path) {
        for (size_t i = 0; i < path.size(); ++i)
            if (path[i] == '/') path[i] = '\\';
        if (path.empty()) return;
        for (size_t i = 0; i < path.size(); ++i) {
            if (path[i] == '\\') {
                std::string sub = path.substr(0, i);
                if (!sub.empty()) CreateDirectoryA(sub.c_str(), 0);
            }
        }
        CreateDirectoryA(path.c_str(), 0);
    }

    static void createParentDirs(const std::string& filepath) {
        std::string p = filepath;
        for (size_t i = 0; i < p.size(); ++i)
            if (p[i] == '/') p[i] = '\\';
        size_t last = p.find_last_of('\\');
        if (last != std::string::npos)
            createDirectories(p.substr(0, last));
    }

    static std::string joinPath(const std::string& out_dir, const std::string& filename) {
        std::string p = out_dir;
        if (!p.empty() && p[p.size() - 1] != '\\' && p[p.size() - 1] != '/') p += '\\';
        p += filename;
        for (size_t i = 0; i < p.size(); ++i)
            if (p[i] == '/') p[i] = '\\';
        return p;
    }

    constexpr uint32_t EOCD_SIG = 0x06054b50;
    constexpr uint32_t CEN_SIG = 0x02014b50;
    constexpr uint32_t LOC_SIG = 0x04034b50;
    constexpr uint16_t METHOD_STORED = 0;
    constexpr uint16_t METHOD_DEFLATE = 8;

    static uint16_t read16(const uint8_t* p) {
        return static_cast<uint16_t>(p[0]) | (static_cast<uint16_t>(p[1]) << 8);
    }
    static uint32_t read32(const uint8_t* p) {
        return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) |
               (static_cast<uint32_t>(p[2]) << 16) | (static_cast<uint32_t>(p[3]) << 24);
    }
}

static bool extractZip(ZlibLoader& zlib, const std::string& zip_path, const std::string& out_dir) {
    std::ifstream f(zip_path, std::ios::binary | std::ios::ate);
    if (!f) {
        std::cerr << "Cannot open ZIP: " << zip_path << '\n';
        return false;
    }
    std::streamsize file_len = f.tellg();
    if (file_len < 22) {
        std::cerr << "File too small to be ZIP\n";
        return false;
    }

    const std::streamsize to_read = (file_len < 65557) ? file_len : std::streamsize(65557);
    const size_t search_max = static_cast<size_t>(to_read);
    std::vector<uint8_t> tail(search_max);
    f.seekg(file_len - search_max);
    f.read(reinterpret_cast<char*>(tail.data()), search_max);
    if (f.gcount() != static_cast<std::streamsize>(search_max)) {
        std::cerr << "Cannot read ZIP tail\n";
        return false;
    }

    size_t eocd_off = 0;
    for (size_t i = search_max; i >= 4; --i) {
        if (zip::read32(&tail[i - 4]) == zip::EOCD_SIG) {
            eocd_off = (file_len - search_max) + (i - 4);
            break;
        }
    }
    if (eocd_off == 0) {
        std::cerr << "ZIP: End of central directory not found\n";
        return false;
    }

    f.seekg(eocd_off + 4);
    uint8_t eocd[18];
    f.read(reinterpret_cast<char*>(eocd), 18);
    if (f.gcount() != 18) return false;

    uint16_t num_entries = zip::read16(eocd + 8);
    uint32_t cen_offset = zip::read32(eocd + 12);

    f.seekg(cen_offset);
    std::vector<uint8_t> cen_buf(46);
    bool ok = true;

    for (uint16_t i = 0; i < num_entries && ok; ++i) {
        f.read(reinterpret_cast<char*>(cen_buf.data()), 46);
        if (f.gcount() != 46) { ok = false; break; }
        if (zip::read32(cen_buf.data()) != zip::CEN_SIG) { ok = false; break; }

        uint16_t method = zip::read16(cen_buf.data() + 10);
        uint32_t comp_size = zip::read32(cen_buf.data() + 20);
        uint32_t uncomp_size = zip::read32(cen_buf.data() + 24);
        uint16_t fn_len = zip::read16(cen_buf.data() + 28);
        uint16_t extra_len = zip::read16(cen_buf.data() + 30);
        uint16_t comment_len = zip::read16(cen_buf.data() + 32);
        uint32_t local_off = zip::read32(cen_buf.data() + 42);

        std::string filename(fn_len, '\0');
        f.read(&filename[0], fn_len);
        if (f.gcount() != fn_len) { ok = false; break; }
        f.seekg(f.tellg() + std::streamoff(extra_len + comment_len));
        const std::streamoff next_cen = f.tellg();

        if (!filename.empty() && (filename[filename.size() - 1] == '/' || filename[filename.size() - 1] == '\\')) {
            zip::createDirectories(zip::joinPath(out_dir, filename));
            continue;
        }

        std::string out_path = zip::joinPath(out_dir, filename);
        zip::createParentDirs(out_path);

        uint8_t local[30];
        f.seekg(local_off);
        f.read(reinterpret_cast<char*>(local), 30);
        if (f.gcount() != 30 || zip::read32(local) != zip::LOC_SIG) { ok = false; break; }
        uint16_t local_fn = zip::read16(local + 26);
        uint16_t local_extra = zip::read16(local + 28);
        std::streamoff data_off = local_off + 30 + local_fn + local_extra;

        if (method == zip::METHOD_STORED) {
            if (comp_size != uncomp_size) { ok = false; break; }
            std::ofstream out(out_path.c_str(), std::ios::binary);
            if (!out) { std::cerr << "Cannot create: " << out_path << '\n'; ok = false; break; }
            f.seekg(data_off);
            std::vector<uint8_t> buf(comp_size);
            f.read(reinterpret_cast<char*>(buf.data()), comp_size);
            if (f.gcount() != static_cast<std::streamsize>(comp_size)) { ok = false; break; }
            out.write(reinterpret_cast<const char*>(buf.data()), comp_size);
            std::cout << "  " << filename << '\n';
        } else if (method == zip::METHOD_DEFLATE) {
            f.seekg(data_off);
            std::vector<Byte> comp_buf(comp_size);
            f.read(reinterpret_cast<char*>(comp_buf.data()), comp_size);
            if (f.gcount() != static_cast<std::streamsize>(comp_size)) { ok = false; break; }

            std::ofstream out(out_path.c_str(), std::ios::binary);
            if (!out) { std::cerr << "Cannot create: " << out_path << '\n'; ok = false; break; }
            int ret = inflateBuffer(zlib, comp_buf.data(), comp_size, out, -15);
            if (ret == Z_DATA_ERROR) {
                out.close();
                out.open(out_path.c_str(), std::ios::binary | std::ios::trunc);
                if (out) ret = inflateBuffer(zlib, comp_buf.data(), comp_size, out, 15);
            }
            if (ret != Z_OK) {
                std::cerr << "Inflate failed for " << filename << " code " << ret << '\n';
                ok = false;
            } else {
                std::cout << "  " << filename << '\n';
            }
        } else {
            std::cerr << "Unsupported compression method " << method << " for " << filename << '\n';
            ok = false;
        }
        f.seekg(next_cen);
    }

    return ok;
}

// --- ZipExtractor implementation ---

struct ZipExtractor::Impl {
    ZlibLoader zlib;
};

ZipExtractor::ZipExtractor() : impl_(new Impl()) {}

ZipExtractor::~ZipExtractor() = default;

bool ZipExtractor::load() {
    return impl_->zlib.load();
}

bool ZipExtractor::extract(const std::string& zip_path, const std::string& out_dir) {
    return extractZip(impl_->zlib, zip_path, out_dir.empty() ? "." : out_dir);
}

/**
 * ZipExtractor - C++11 implementation.
 */

#include "ZipExtractor.h"
#include "minizip/unzip.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>

# include <direct.h>
# define MKDIR(path) _mkdir(path)
# define SEP "\\"
#include <errno.h>

namespace {

constexpr size_t kChunkSize = 64 * 1024;
constexpr size_t kMaxPathLen = 512;

bool EnsureDir(const char* path) {
    if (!path || path[0] == '\0') return true;
    std::vector<char> buf(path, path + std::strlen(path) + 1);
    if (buf.size() > kMaxPathLen) return false;

    for (size_t i = 1; i < buf.size(); ++i) {
        if (buf[i] == '/' || buf[i] == '\\') {
            buf[i] = '\0';
            if (MKDIR(buf.data()) != 0 && errno != EEXIST) {
                buf[i] = SEP[0];
                return false;
            }
            buf[i] = SEP[0];
        }
    }
    return MKDIR(buf.data()) == 0 || errno == EEXIST;
}

std::string GetDirPart(const std::string& filepath) {
    std::string::size_type p = filepath.find_last_of("/\\");
    if (p == std::string::npos) return std::string();
    return filepath.substr(0, p);
}

void SetError(std::string* outError, const char* fmt, ...) {
    if (!outError) return;
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    std::vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    *outError = buf;
}

} // anonymous namespace

namespace zip {

bool Extract(const std::string& zipPath,
             const std::string& outDir,
             std::string* outError) {
    unzFile uf = unzOpen64(zipPath.c_str());
    if (!uf) {
        SetError(outError, "Cannot open ZIP file: %s", zipPath.c_str());
        return false;
    }

    unz_global_info64 gi;
    if (unzGetGlobalInfo64(uf, &gi) != UNZ_OK) {
        SetError(outError, "Failed to read ZIP info: %s", zipPath.c_str());
        unzClose(uf);
        return false;
    }

    std::vector<char> buf(kChunkSize);
    std::string outPath;
    std::string dirPart;
    outPath.reserve(kMaxPathLen);
    dirPart.reserve(kMaxPathLen);

    for (uLong i = 0; i < gi.number_entry; ++i) {
        char filenameInZip[256];
        unz_file_info64 fileInfo;
        int err = unzGetCurrentFileInfo64(uf, &fileInfo,
                                         filenameInZip, sizeof(filenameInZip),
                                         nullptr, 0, nullptr, 0);
        if (err != UNZ_OK) {
            unzGoToNextFile(uf);
            continue;
        }

        outPath = outDir;
        if (!outPath.empty() && outPath.back() != '/' && outPath.back() != '\\')
            outPath += SEP;
        outPath += filenameInZip;

        size_t len = std::strlen(filenameInZip);
        bool isDir = (len > 0 && (filenameInZip[len - 1] == '/' || filenameInZip[len - 1] == '\\'));

        if (isDir) {
            if (!EnsureDir(outPath.c_str())) {
                SetError(outError, "Failed to create directory: %s", outPath.c_str());
                unzClose(uf);
                return false;
            }
        } else {
            dirPart = GetDirPart(outPath);
            if (!dirPart.empty() && !EnsureDir(dirPart.c_str())) {
                SetError(outError, "Failed to create directory: %s", dirPart.c_str());
                unzClose(uf);
                return false;
            }

            err = unzOpenCurrentFile(uf);
            if (err != UNZ_OK) {
                unzCloseCurrentFile(uf);
                unzGoToNextFile(uf);
                continue;
            }

            std::FILE* fout = std::fopen(outPath.c_str(), "wb");
            if (!fout) {
                SetError(outError, "Cannot create file: %s", outPath.c_str());
                unzCloseCurrentFile(uf);
                unzClose(uf);
                return false;
            }

            bool readError = false;
            do {
                err = unzReadCurrentFile(uf, buf.data(), static_cast<unsigned>(buf.size()));
                if (err < 0) {
                    SetError(outError, "Read failed for entry: %s (err=%d)", filenameInZip, err);
                    readError = true;
                    break;
                }
                if (err > 0) {
                    if (std::fwrite(buf.data(), 1, static_cast<size_t>(err), fout) != static_cast<size_t>(err)) {
                        SetError(outError, "Write failed: %s", outPath.c_str());
                        readError = true;
                        break;
                    }
                }
            } while (err > 0);

            std::fclose(fout);
            unzCloseCurrentFile(uf);
            if (readError) {
                unzClose(uf);
                return false;
            }
        }

        if (i + 1 < gi.number_entry) {
            err = unzGoToNextFile(uf);
            if (err != UNZ_OK && err != UNZ_END_OF_LIST_OF_FILE) {
                SetError(outError, "Failed to go to next entry (err=%d)", err);
                unzClose(uf);
                return false;
            }
        }
    }

    unzClose(uf);
    return true;
}

} // namespace zip

#pragma once

#include <string>

namespace zip {

bool Extract(const std::string& zipPath,
             const std::string& outDir,
             std::string* outError = nullptr);

} // namespace zip

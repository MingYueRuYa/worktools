#pragma once

#include <QString>

class ExecHelper {
 public:
  enum Architecture { ARCH_UNKNOWN, ARCH_X86, ARCH_X64 };

 public:
  ExecHelper() = default;
  ~ExecHelper() = default;
  ExecHelper(const ExecHelper& left) = delete;
  ExecHelper& operator=(const ExecHelper& left) = delete;

  Architecture detect_arch(const QString& filePath);
};

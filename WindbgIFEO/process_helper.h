#pragma once

#include <map>
#include <string>

class ProcessHelper {
 public:
  struct ProcessInfo {
    std::wstring name;
    int pid;
  };

  using process_map = std::map<int, ProcessInfo>;

 public:
  ProcessHelper() = default;
  ~ProcessHelper() = default;
  ProcessHelper& operator=(const ProcessHelper& left) = delete;
  ProcessHelper(const ProcessHelper& left) = delete;

  static process_map EnumAllProcess();
};

#pragma once

#include <map>
#include <string>
#include <vector>

using std::wstring;
using std::vector;

class EnvHelper
{
public:
  EnvHelper();
  ~EnvHelper();

  EnvHelper& operator=(const EnvHelper& left) = delete;
  EnvHelper(const EnvHelper& left) = delete;
  std::map<wstring, wstring> get_env() const;

private:
  bool _init_env();
  std::vector<std::wstring> _split(const std::wstring& str, wchar_t delimiter) const;

private:
  vector<wstring> _vec_env;


};


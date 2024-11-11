#pragma once

#include <map>
#include <string>
#include <vector>
#include <windows.h>

using std::wstring;
using std::vector;

class EnvHelper
{
public:
  static std::vector<std::wstring> split(const std::wstring& str, wchar_t delimiter);

public:
  EnvHelper();
  ~EnvHelper();

  EnvHelper& operator=(const EnvHelper& left) = delete;
  EnvHelper(const EnvHelper& left) = delete;
  std::map<wstring, wstring> get_env() const;
  std::pair<wstring, wstring> find(const wstring& name) const;
  bool set_key(LPCWSTR varName, LPCWSTR varValue);
  bool del_key(const std::wstring& valueName);

private:
  bool _init_env();
  void _convert_2_map();

private:
  vector<wstring> _vec_env;
  std::map<wstring, wstring> _map_env;
  const wstring _reg_path = LR"(SYSTEM\CurrentControlSet\Control\Session Manager\Environment)";
};


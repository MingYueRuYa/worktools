#include "EnvHelper.h"
#include <iostream>
#include <windows.h>
#include <iostream>
#include <string>

EnvHelper::EnvHelper()
{
  this->_init_env();
  this->_convert_2_map();
}

EnvHelper::~EnvHelper()
{
}

std::pair<wstring, wstring> EnvHelper::find(const wstring& name) const
{
  auto ifind = this->_map_env.find(name);
  if (ifind == this->_map_env.end())
  {
    return {};
  }

  return { ifind->first, ifind->second };
}

std::map<wstring, wstring> EnvHelper::get_env() const
{
  return this->_map_env;
}

bool EnvHelper::_init_env()
{
  LPWCH envStrings = GetEnvironmentStringsW();
  if (envStrings == NULL) {
    return false;
  }

  // 遍历环境变量
  LPWCH envVar = envStrings;
  while (*envVar) {
    // 从环境变量字符串中提取键和值
    std::wcout << envVar << std::endl;

    // 移动到下一个环境变量
    envVar += wcslen(envVar) + 1;
    wstring var = envVar;
    _vec_env.push_back(var);
  }

  // 释放环境变量字符串指针
  FreeEnvironmentStringsW(envStrings);
  return true;
}

void EnvHelper::_convert_2_map()
{
  for (const wstring& item : _vec_env) {
    if (item.empty()) {
      continue;
    }
    if (item[0] == '=') {
      continue;
    }
    std::vector<wstring> values = split(item, L'=');
    if (values.empty())
    {
      continue;
    }
    _map_env[values[0]] = values[1];
  }
}

std::vector<wstring> EnvHelper::split(const wstring& str, wchar_t delimiter) {
  std::vector<wstring> tokens;
  size_t start = 0;
  size_t end = str.find(delimiter);

  while (end != wstring::npos) {
    tokens.push_back(str.substr(start, end - start));
    start = end + 1;
    end = str.find(delimiter, start);
  }

  // 添加最后一个子串
  tokens.push_back(str.substr(start));
  return tokens;
}

bool EnvHelper::set_key(LPCWSTR varName, LPCWSTR varValue) {
  HKEY hKey;
  LONG result;

  // 打开注册表项
  result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _reg_path.c_str(), 0, KEY_SET_VALUE, &hKey);
  if (result != ERROR_SUCCESS) {
    return false;
  }

  // 设置环境变量
  result = RegSetValueEx(hKey,
    varName,
    0, REG_SZ, reinterpret_cast<const BYTE*>(varValue), (wcslen(varValue) + 1) * sizeof(wchar_t));
  RegCloseKey(hKey);
  if (result != ERROR_SUCCESS) {
    return false;
  }

  // 通知系统环境变量已经更改
  SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, reinterpret_cast<LPARAM>(L"Environment"), SMTO_ABORTIFHUNG, 5000, nullptr);
  return true;
}

bool EnvHelper::del_key(const std::wstring& valueName) {
  HKEY hKey;
  LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _reg_path.c_str(), 0, KEY_SET_VALUE, &hKey);

  if (result != ERROR_SUCCESS) {
    return false;
  }

  result = RegDeleteValue(hKey, valueName.c_str());
  if (result != ERROR_SUCCESS) {
    RegCloseKey(hKey);
    return false;
  }

  RegCloseKey(hKey);
  return true;
}

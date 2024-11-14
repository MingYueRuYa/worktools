#include "EnvHelper.h"
#include <iostream>
#include <windows.h>
#include <iostream>
#include <string>

EnvHelper::EnvHelper()
{
  this->reset();
}

EnvHelper::~EnvHelper()
{
}

void EnvHelper::reset()
{
  this->_init_env();
  //this->_convert_2_map();
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
  this->_vec_env.clear();
  this->_map_env.clear();
  this->_query_values();
  //LPWCH envStrings = GetEnvironmentStringsW();
  //if (envStrings == NULL) {
  //  return false;
  //}

  //// 遍历环境变量
  //LPWCH envVar = envStrings;
  //while (*envVar) {
  //  // 从环境变量字符串中提取键和值
  //  std::wcout << envVar << std::endl;

  //  // 移动到下一个环境变量
  //  envVar += wcslen(envVar) + 1;
  //  wstring var = envVar;
  //  _vec_env.push_back(var);
  //}

  //// 释放环境变量字符串指针
  //FreeEnvironmentStringsW(envStrings);
  return true;
}

void EnvHelper::_convert_2_map()
{
  _map_env.clear();
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

void EnvHelper::_query_values()
{
  HKEY hKey;
  // Open the specified key.
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _reg_path.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
    _enum_keys(hKey);
  }
  else {
    //_tprintf(TEXT("Error opening key.\n"));
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

void EnvHelper::_enum_keys(HKEY hKey) {
  const int MAX_VALUE_NAME = 16383;
  const int MAX_KEY_LENGTH = 255;
  TCHAR    achValue[MAX_VALUE_NAME];
  DWORD    cchValue = MAX_VALUE_NAME;
  BYTE     achData[MAX_VALUE_NAME];
  DWORD    cchData = MAX_VALUE_NAME;
  DWORD    cValues;              // number of values for key 
  DWORD    cbMaxValueData;       // size of largest data component
  DWORD    cchMaxValue;          // size of largest value name 
  DWORD    retCode;
  DWORD    i, j;
  DWORD    Type;

  // Get the class name and the value count. 
  retCode = RegQueryInfoKey(
    hKey,                    // key handle 
    NULL,                    // buffer for class name 
    NULL,                    // size of class string 
    NULL,                    // reserved 
    NULL,                    // number of subkeys 
    NULL,                    // longest subkey size 
    NULL,                    // longest class string 
    &cValues,                // number of values for this key 
    &cchMaxValue,            // longest value name 
    &cbMaxValueData,         // longest value data 
    NULL,                    // security descriptor 
    NULL);                   // last write time 

// Enumerate the values.
  if (cValues) {
    //_tprintf(TEXT("\nNumber of values: %d\n"), cValues);
    for (i = 0, retCode = ERROR_SUCCESS; i < cValues; i++) {
      cchValue = MAX_VALUE_NAME;
      achValue[0] = '\0';
      cchData = MAX_VALUE_NAME;
      achData[0] = '\0';
      retCode = RegEnumValue(hKey, i,
        achValue,
        &cchValue,
        NULL,
        &Type,
        achData,
        &cchData);

      if (retCode == ERROR_SUCCESS) {
        switch (Type) {
        case REG_SZ:
        case REG_EXPAND_SZ:
          //_tprintf(TEXT("  Data: %s\n"), (TCHAR*)achData);
          _map_env[achValue] = std::wstring((wchar_t*)achData);
          break;
        case REG_DWORD:
          _map_env[achValue] = std::to_wstring((DWORD)achData);
          break;
          // Add cases for other types as needed
        default:
          //_tprintf(TEXT("  Data: (type %d, size %d)\n"), Type, cchData);
          break;
        }
      }
    }
  }
  RegCloseKey(hKey);
}

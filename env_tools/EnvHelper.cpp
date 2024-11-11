#include "EnvHelper.h"
#include <iostream>
#include <windows.h>
#include <iostream>
#include <string>

EnvHelper::EnvHelper()
{
  this->_init_env();
}

EnvHelper::~EnvHelper()
{

}

std::map<wstring, wstring> EnvHelper::get_env() const
{
  std::map<wstring, wstring> result = {};
  for (const wstring& item : _vec_env) {
    if (item.empty()) {
      continue;
    }
    if (item[0] == '=') {
      continue;
    }
    std::vector<wstring> values = _split(item, L'=');
    if (values.empty())
    {
      continue;
    }
    result[values[0]] = values[1];
  }
  return result;
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


std::vector<wstring> EnvHelper::_split(const wstring& str, wchar_t delimiter) const {
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

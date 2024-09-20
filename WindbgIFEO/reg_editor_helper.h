#pragma once

#include <string>

#include <Windows.h>

class RegEditorHelper {
 public:
  RegEditorHelper() = default;
  ~RegEditorHelper() = default;
  RegEditorHelper(const RegEditorHelper& left) = delete;
  RegEditorHelper& operator=(const RegEditorHelper& left) = delete;
  bool send_value(const std::wstring& value, int loop_count = 60);

 public:
  static BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam);

 private:
  HWND _find_reg_wnd();
  bool _find_edit_wnd();
};

#include "reg_editor_helper.h"

#include <thread>
#include <vector>

std::vector<HWND> g_child_windows;

bool RegEditorHelper::send_value(const std::wstring& value, int loop_count) {
  bool result = false;
  int start_loop = 0;
  do {
    start_loop += 1;
    result = this->_find_edit_wnd();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  } while (!result && start_loop < loop_count);

  if (!result) {
    return false;
  }

  const wchar_t class_name[] = L"Edit";
  wchar_t name[256] = {0};
  HWND edit_hwnd = NULL;
  for (HWND wnd : g_child_windows) {
    ::GetClassNameW(wnd, name, 256);
    if (wcscmp(name, class_name) != 0) {
      memset(name, 0, 256);
      continue;
    }
    edit_hwnd = wnd;
    break;
  }

  if (edit_hwnd == NULL) {
    return false;
  }

  SendMessageW(edit_hwnd, WM_SETTEXT, 0, (LPARAM)value.c_str());
  PostMessageW(edit_hwnd, WM_KEYDOWN, VK_RETURN, 0);
  PostMessageW(edit_hwnd, WM_KEYUP, VK_RETURN, 0);

  return true;
}

HWND RegEditorHelper::_find_reg_wnd() {
  const wchar_t reg_caption[] = L"Registry Editor";
  const wchar_t reg_class[] = L"RegEdit_RegEdit";
  HWND hwnd = FindWindowW(reg_class, reg_caption);
  return hwnd;
}

bool RegEditorHelper::_find_edit_wnd() {
  HWND reg_wnd = this->_find_reg_wnd();
  if (reg_wnd == NULL) {
    return false;
  }

  ::EnumChildWindows(reg_wnd, EnumChildProc, 0);
  return true;
}

BOOL CALLBACK RegEditorHelper::EnumChildProc(HWND hwnd, LPARAM lParam) {
  g_child_windows.push_back(hwnd);
  return TRUE;
}

#include "process_helper.h"

#include <windows.h>

#include <psapi.h>
#include <tlhelp32.h>

#pragma comment(lib, "psapi.lib")

ProcessHelper::process_map ProcessHelper::EnumAllProcess() {
  process_map pm;
  DWORD process_id[1024], cbNeeded, cProcesses;
  unsigned int i;

  // 获取所有进程的 PID
  if (!EnumProcesses(process_id, sizeof(process_id), &cbNeeded)) {
    return {};
  }

  // 计算返回的进程数量
  cProcesses = cbNeeded / sizeof(DWORD);

  // 遍历所有进程
  for (i = 0; i < cProcesses; i++) {
    if (0 == process_id[i]) {
      continue;
    }

    DWORD processID = process_id[i];
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

    // 获取进程句柄
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                  FALSE, processID);

    if (NULL == hProcess) {
      continue;
    }

    HMODULE hMod;
    DWORD cbNeeded;

    if (0 == EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
      continue;
    }
    GetModuleBaseName(hProcess, hMod, szProcessName,
                      sizeof(szProcessName) / sizeof(TCHAR));
    ProcessInfo info = {szProcessName, processID};
    pm[processID] = info;
    // 关闭进程句柄
    CloseHandle(hProcess);
  }

  return pm;
}

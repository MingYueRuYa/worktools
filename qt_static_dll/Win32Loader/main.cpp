#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
typedef void (__cdecl *ShowDialogFunc)(const char* title, const char* message);

static HMODULE g_hDll = nullptr;
static ShowDialogFunc g_showDialog = nullptr;

static void UnloadDll()
{
    if (g_hDll) {
        FreeLibrary(g_hDll);
        g_hDll = nullptr;
        g_showDialog = nullptr;
    }
}

static bool LoadDllAndGetShowDialog()
{
    if (g_showDialog)
        return true;

    wchar_t exePath[MAX_PATH];
    if (!GetModuleFileNameW(nullptr, exePath, MAX_PATH)) {
        MessageBoxW(nullptr, L"无法获取程序路径。", L"错误", MB_ICONERROR);
        return false;
    }

    // 去掉 exe 文件名，得到 exe 所在目录
    wchar_t* lastSlash = wcsrchr(exePath, L'\\');
    if (lastSlash)
        *lastSlash = L'\0';
    else
        exePath[0] = L'\0';

    // 优先：同目录下 QtStaticDll.dll
    wchar_t dllPath[MAX_PATH];
    swprintf_s(dllPath, L"%s\\QtStaticDll.dll", exePath);

    g_hDll = LoadLibraryW(dllPath);
    if (!g_hDll) {
        // 备选：上级目录的 QtStaticDll\release 或 QtStaticDll\debug
        swprintf_s(dllPath, L"%s\\..\\QtStaticDll\\release\\QtStaticDll.dll", exePath);
        g_hDll = LoadLibraryW(dllPath);
    }
    if (!g_hDll) {
        swprintf_s(dllPath, L"%s\\..\\QtStaticDll\\debug\\QtStaticDll.dll", exePath);
        g_hDll = LoadLibraryW(dllPath);
    }

    if (!g_hDll) {
        DWORD err = GetLastError();
        wchar_t msg[256];
        swprintf_s(msg, L"无法加载 QtStaticDll.dll，错误码: %lu\n请将 DLL 放到 exe 同目录或 QtStaticDll\\release 下。", err);
        MessageBoxW(nullptr, msg, L"加载失败", MB_ICONERROR);
        return false;
    }

    g_showDialog = (ShowDialogFunc)GetProcAddress(g_hDll, "qt_static_dll_show_dialog");
    if (!g_showDialog) {
        MessageBoxW(nullptr, L"在 DLL 中未找到 qt_static_dll_show_dialog。", L"错误", MB_ICONERROR);
        UnloadDll();
        return false;
    }
    return true;
}

static void OnButtonClick(HWND hwnd)
{
    (void)hwnd;
    if (!LoadDllAndGetShowDialog())
        return;
    g_showDialog("来自 Win32 加载器", "已通过加载的 Qt 静态 DLL 弹出此对话框。");
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_COMMAND:
        if (LOWORD(wParam) == 1)
            OnButtonClick(hwnd);
        break;
    case WM_DESTROY:
        UnloadDll();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"Win32LoaderClass";

    if (!RegisterClassExW(&wc)) {
        MessageBoxW(nullptr, L"窗口类注册失败。", L"错误", MB_ICONERROR);
        return 1;
    }

    HWND hwnd = CreateWindowExW(
        0, wc.lpszClassName, L"加载 Qt 静态 DLL",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 200,
        nullptr, nullptr, hInstance, nullptr
    );
    if (!hwnd) {
        MessageBoxW(nullptr, L"窗口创建失败。", L"错误", MB_ICONERROR);
        return 1;
    }

    HWND btn = CreateWindowW(
        L"BUTTON", L"加载 DLL 并弹出对话框",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        100, 60, 200, 40, hwnd, (HMENU)1, hInstance, nullptr
    );
    if (!btn) {
        MessageBoxW(nullptr, L"按钮创建失败。", L"错误", MB_ICONERROR);
        DestroyWindow(hwnd);
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

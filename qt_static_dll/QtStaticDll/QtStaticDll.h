#pragma once

#ifdef QT_STATICDLL_EXPORTS
#define QTSTATICDLL_API __declspec(dllexport)
#else
#define QTSTATICDLL_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** 返回 Qt 版本字符串，调用方不得 free */
QTSTATICDLL_API const char* qt_static_dll_get_qt_version(void);

/** 返回一条由 Qt 生成的问候消息，调用方需 free 返回的指针 */
QTSTATICDLL_API char* qt_static_dll_hello_message(const char* name);

/** 显示一个信息对话框。title 为标题，message 为内容；均可为 NULL，默认显示“提示”和空内容 */
QTSTATICDLL_API void qt_static_dll_show_dialog(const char* title, const char* message);

#ifdef __cplusplus
}
#endif

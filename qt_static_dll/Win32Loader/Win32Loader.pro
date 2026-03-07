# 纯 Win32 项目，不依赖 Qt；用于加载 QtStaticDll.dll 并调用弹窗
TEMPLATE = app
CONFIG  -= app_bundle console

# Windows：GUI 子系统，无控制台窗口
win32: QMAKE_LFLAGS += -mwindows

SOURCES += main.cpp

# 链接 Win32 库
win32: LIBS += -luser32 -lkernel32 -lgdi32

# 输出 exe 名称
TARGET = Win32Loader

# 可选：构建后复制 DLL 到 exe 目录，便于直接运行
# win32 {
#     DLLPATH = $$PWD/../QtStaticDll/release/QtStaticDll.dll
#     QMAKE_POST_LINK += $${QMAKE_COPY} $$shell_path($$DLLPATH) $$shell_path($$OUT_PWD/) $$escape_expand(\n\t)
# }


win32 {

# Windows平台：链接Qt安装目录下的ssllib
LIBS += -L$$[QT_INSTALL_LIBS] -llibssl
}

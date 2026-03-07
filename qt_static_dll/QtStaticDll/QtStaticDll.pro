QT       += core gui widgets
TEMPLATE  = lib
CONFIG   += dll static
# static：链接 Qt 静态库，需使用静态编译的 Qt Kit；若用动态 Qt 请去掉 static
DEFINES  += QT_STATICDLL_EXPORTS

SOURCES  += \
    dllmain.cpp \
    QtStaticDll.cpp

HEADERS  += \
    QtStaticDll.h

# Windows：生成 QtStaticDll.dll（及导入库 QtStaticDll.lib）
win32 {
    QMAKE_TARGET.arch = $$QT_ARCH

# Windows平台：链接Qt安装目录下的ssllib
LIBS += -L$$[QT_INSTALL_LIBS] -llibssl
# 或者直接指定完整路径
# LIBS += $$[QT_INSTALL_LIBS]/ssllib.lib
}

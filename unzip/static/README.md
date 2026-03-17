# UnzipConsole - VS2019 Win32 控制台解压 ZIP

使用 **zlib.lib** 静态库和 minizip 解压 ZIP 文件的 Win32 控制台示例项目。

## 环境要求

- Visual Studio 2019（或 2017/2022，平台工具集可改为对应版本）
- 已编译好的 **zlib** Win32 静态库：`zlib.lib`
- zlib 头文件：`zlib.h`、`zconf.h`

## 目录结构（准备 zlib）

在解决方案目录下准备如下结构（或通过项目属性指定你自己的路径）：

```
zlib_static/
├── UnzipConsole.sln
├── UnzipConsole/
│   ├── UnzipConsole.vcxproj
│   ├── main.cpp
│   └── minizip/
│       ├── ioapi.c, ioapi.h
│       ├── unzip.c, unzip.h
├── include/          ← 放置 zlib 头文件
│   ├── zlib.h
│   └── zconf.h
└── lib/              ← 放置 Win32 静态库
    └── zlib.lib
```

- **include**：把 zlib 源码中的 `zlib.h`、`zconf.h` 复制到 `include/`，或把 zlib 的 include 路径加到项目的“附加包含目录”。
- **lib**：把针对 Win32 编译得到的 `zlib.lib` 放到 `lib/`，或把其所在目录加到“附加库目录”，并确保“附加依赖项”中有 `zlib.lib`。

## 如何编译 zlib.lib（Win32）

若尚未有 zlib.lib，可：

1. 从 [zlib 官网](https://zlib.net/) 或 [GitHub](https://github.com/madler/zlib) 下载源码。
2. 用 VS2019 打开 `contrib/vstudio/vc14/zlibvc.sln`（或 vc15 等），选择 **Win32** 配置，编译生成静态库；或使用 NMake/Makefile 生成 Win32 的 `zlib.lib`。
3. 将生成的 `zlib.lib` 放到本项目的 `lib/`，头文件放到 `include/`。

## 编译与运行

1. 用 Visual Studio 2019 打开 `UnzipConsole.sln`。
2. 选择配置 **Debug|Win32** 或 **Release|Win32**，生成解决方案。
3. 可执行文件在 `bin\Debug\UnzipConsole.exe` 或 `bin\Release\UnzipConsole.exe`。

## 用法

```text
UnzipConsole.exe <zip文件路径> [解压目录]
```

- 只传 zip 路径：解压到当前目录。
- 第二个参数为解压目录：解压到指定目录。

示例：

```cmd
UnzipConsole.exe test.zip
UnzipConsole.exe test.zip C:\output
```

## 说明

- 本项目使用 zlib 官方的 **minizip**（contrib/minizip）中的 `unzip`、`ioapi` 源码，与 `zlib.lib` 一起完成 ZIP 解压。
- 已通过 `NOUNCRYPT` 关闭加密 ZIP 支持，仅支持未加密的 ZIP。
- 若需支持加密或更多选项，可参考 minizip 文档并修改预定义与链接方式。

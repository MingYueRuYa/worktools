# Qt 静态库编译为 DLL（Visual Studio / Qt Creator）

本工程演示如何将 **Qt 静态库** 链接进一个 **DLL**，生成一个不依赖 Qt 运行时 DLL 的动态库。支持 **Visual Studio** 与 **Qt Creator** 两种构建方式。

## 前置条件

1. **Qt 静态编译版本**  
   官方预编译包通常只有动态库，静态库需自行从源码编译 Qt，例如：

   ```bat
   cd qt-everywhere-src-5.15.2
   configure -static -prefix C:\Qt\5.15.2\msvc2019_64_static ...
   nmake
   nmake install
   ```

2. **Visual Studio 2019/2022**，且平台工具集与您编译 Qt 时使用的 MSVC 一致（如 v143）。

## 配置 Qt 路径

在 **QtStaticDll** 项目目录下编辑 `QtPaths.props`，将 `QtDir` 改为您本机 Qt 静态安装目录：

```xml
<PropertyGroup>
  <QtDir>C:\Qt\5.15.2\msvc2019_64_static</QtDir>
</PropertyGroup>
```

或在 VS 中：**项目属性 → 配置属性 → C/C++ → 常规 → 附加包含目录** 和 **链接器 → 常规 → 附加库目录** 中手动指定 Qt 的 `include`、`lib` 路径。

## 编译

### 方式一：Visual Studio

1. 用 Visual Studio 打开 `QtStaticDll.sln`。
2. 选择配置（Debug/Release）和平台（x86/x64），需与您的 Qt 静态库架构一致。
3. 生成解决方案。

输出 DLL 在：`bin\<Platform>\<Configuration>\QtStaticDll.dll`。

### 方式二：Qt Creator

1. 用 Qt Creator 打开 **`qt_static_dll.pro`**（或直接打开 `QtStaticDll/QtStaticDll.pro`）。
2. 在“项目”里选择 **Kit**：请使用基于 **静态编译 Qt** 的 Kit（若只有动态 Qt，需先在 Qt 源码中 `configure -static` 编译出静态 Qt，再在 Qt Creator 中添加该 Qt 的 Kit）。
3. 选择 Debug 或 Release，点击“构建”。

生成的 `QtStaticDll.dll` 在构建目录下（如 `build-QtStaticDll-Desktop_Qt_xxx/Release/`）。若 .pro 中未使用静态 Qt，请去掉 `QtStaticDll.pro` 里的 `CONFIG += static`。

## 导出接口说明

DLL 提供 C 接口，便于其他语言或未链接 Qt 的程序调用：

- `qt_static_dll_get_qt_version()`  
  返回 Qt 版本字符串，只读，无需释放。

- `qt_static_dll_hello_message(const char* name)`  
  返回一条问候字符串，调用方需对返回的指针调用 `free()`。

- `qt_static_dll_show_dialog(const char* title, const char* message)`  
  弹出信息对话框；`title`/`message` 可为 NULL。

## 使用注意

- 静态链接 Qt 会显著增大 DLL 体积，并需注意 Qt 的许可证（LGPL/商业）。
- 若您的 Qt 静态库带有调试后缀（如 `Qt5Cored.lib`），本工程已按 Debug 用 `d` 后缀、Release 用无后缀配置；若实际库名不同，请在项目属性中调整 **链接器 → 输入 → 附加依赖项**。

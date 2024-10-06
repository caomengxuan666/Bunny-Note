# 基于QWebEngine的现代CPP的Markdown 编辑器

一个使用 C++ 和 Qt 构建的简洁高效的 Markdown 编辑器。该应用程序提供流畅的写作体验，具有实时预览、语法高亮和可自定义的主题。

## 功能特性

- **实时预览**：在您输入时自动更新预览面板。
- **语法高亮**：使用 Highlight.js 支持代码块高亮显示。
- **多种主题**：可选择浅色、深色、Solarized 浅色和 Solarized 深色主题。
- **文件管理**：
  - 打开并编辑现有的 Markdown 文件。
  - 在应用程序中创建新文件。
  - 轻松删除不需要的文件。
- **标签式界面**：支持多标签，您可以同时处理多个文档。
- **可定制的编辑器**：
  - 更改字体类型和大小。
  - 设置制表符宽度以匹配您的编码风格。
- **插入图片**：快速将图片插入到您的 Markdown 文档中。
- **自动保存**：每隔 10 秒自动保存您的工作，防止数据丢失。
- **分屏视图调整**：通过自定义的分隔条手柄调整编辑器和预览窗格的大小，提升用户体验。

## 截图

![Uploading {880F839F-AB02-4a2c-8618-7640CF172785}.png…]()


*编辑器的示例，具有实时预览和语法高亮功能。*

## 入门指南

### 先决条件

- **Qt 框架**：确保您已安装 Qt（建议版本 5.12 或更高）。
- **C++ 编译器**：支持 C++17 的编译器（例如 GCC、Clang、MSVC）。

### 克隆仓库

```bash
git clone https://github.com/caomengxuan666/WebEngine_QMarkDownEditor_cpp.git
```
cd markdown-editor
构建指南
在 Qt Creator 中打开项目：

启动 Qt Creator。
点击“打开项目”，选择 markdown-editor.pro 文件。
配置项目：

选择适合您的开发环境的 Kit。
确保编译器和 Qt 版本设置正确。
构建并运行：

点击“运行”按钮，构建并启动应用程序。
命令行运行
bash
复制代码
# 进入项目目录
cd markdown-editor

# 创建构建目录
mkdir build && cd build

# 使用 cmake 生成 Makefile
cmake ..

# 构建项目
make

# 运行应用程序
./markdown-editor
使用说明
打开文件：
使用左侧的文件列表打开当前目录中的现有 Markdown 文件。
您也可以通过“文件”>“打开文件”从其他目录打开文件。
创建新文件：
前往“文件”>“新建文件”或按 Ctrl+N。
输入所需的文件名（无需扩展名）。
保存文件：
使用“文件”>“保存文件”或按 Ctrl+S 保存您的工作。
使用“文件”>“另存为”将当前文档保存为新名称。
删除文件：
从列表中选择一个文件，然后选择“文件”>“删除文件”或按 Ctrl+D。
插入图片：
将光标放在您希望插入图片的位置。
前往“文件”>“插入图片”或按 Ctrl+I。
选择要插入的图片文件。
更改主题：
从“主题”菜单中选择您喜欢的主题。
调整字体：
选择“文件”>“设置字体”或按 Ctrl+P，更改编辑器的字体和大小。
自定义
主题
应用程序支持四种主题：

Light（浅色）
Dark（深色）
Solarized Light（Solarized 浅色）
Solarized Dark（Solarized 深色）
您可以通过修改 MainWindow.cpp 中的 updatePalette 和 applyThemeToAllTabs 函数来添加更多主题。

语法高亮样式
语法高亮使用 Highlight.js 的 GitHub 风格。要更改样式：

从 Highlight.js 样式库 下载所需的 CSS 文件。
在 loadMarkdown 函数中更新 highlightCss 变量，使用新的 CSS 文件路径。
项目结构
main.cpp：应用程序的入口点。
MainWindow.h / MainWindow.cpp：包含主窗口的逻辑、UI 设置和事件处理。
HtmlConverter.hpp：用于将 Markdown 转换为 HTML 的工具类。
settings.h：管理应用程序设置，如主题、字体和上次打开的文件。
resources/：包含图片和其他资源文件。
screenshots/：存放在 README 中使用的图片。
依赖项
Qt：用于 GUI 框架和 Web 引擎组件。
Highlight.js：用于代码块的语法高亮显示。
cmark：用于 Markdown 解析的库。
贡献
欢迎贡献
致谢
Qt 文档：提供了全面的指南和示例。
Highlight.js：提供了简单的方法添加语法高亮。
cmark：用于 Markdown 解析的库。
联系方式
QQ:2507560089

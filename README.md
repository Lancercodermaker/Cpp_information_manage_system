# 超市商品库存管理系统

这是一个基于 C++17 的控制台信息管理系统，用于管理超市商品库存。项目面向课程设计要求实现，覆盖商品信息的添加、删除、修改、查询、统计、排序、文件保存和异常输入处理。

系统将商品抽象为统一父类，并派生出食品、日用品和电子产品三类实体；库存数据使用 `std::vector<std::shared_ptr<Product>>` 批量管理；程序启动时从 CSV 文件读取历史数据，保存或正常退出时写回文件，支持多次运行后继续处理已有数据。

## 功能说明

- 添加食品、日用品和电子产品。
- 删除、修改和查询商品信息。
- 按商品编号精确查询，按商品名称关键字模糊查询。
- 显示全部商品列表。
- 统计商品总数、总库存量、库存成本和潜在销售额。
- 查询低库存商品。
- 按价格或库存排序。
- 从 `data/products.csv` 自动读取数据，并保存修改后的数据。
- 对菜单编号、价格、库存等输入进行合法性检查，输入错误时提示重新输入。
- 读取 CSV 时跳过损坏记录，尽量保留其他有效数据。

## 项目结构

```text
.
├── include/                 头文件目录
├── src/                     主程序源码目录
├── tests/                   自动化测试源码
├── scripts/                 Windows 构建和测试脚本
├── data/products.csv        商品数据文件
├── docs/                    设计文档和使用说明
└── .vscode/                 VS Code 构建、调试和 IntelliSense 配置
```

## 环境要求

推荐环境：

- Windows。
- Visual Studio Code。
- VS Code 的 Microsoft C/C++ 扩展。
- Visual Studio 2022，并安装“使用 C++ 的桌面开发”工作负载。

说明：VS Code 的 C/C++ 扩展只负责代码补全、语法分析和调试集成，不包含 C++ 编译器。本项目默认使用 Visual Studio 2022 自带的 MSVC 编译器，构建脚本会自动查找并加载 MSVC 环境。

## 在 VS Code 中使用

完整步骤见：[在 VS Code 中编译、运行和调试](docs/vscode-usage-guide.md)。

最常用流程：

1. 用 VS Code 打开整个项目文件夹，而不是只打开单个 `.cpp` 文件。
2. 按 `Ctrl+Shift+B` 执行默认构建任务。
3. 构建成功后，在终端运行：

   ```powershell
   .\build\supermarket_inventory.exe
   ```

4. 如需调试，打开“运行和调试”面板，选择 `Debug supermarket_inventory (MSVC)`，然后按 `F5`。

## 命令行编译

使用项目自带的 MSVC 构建脚本：

```powershell
.\scripts\build_msvc.cmd
```

编译成功后会生成：

```text
build\supermarket_inventory.exe
```

如果本机安装了 MinGW-w64，也可以手动使用：

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude src/*.cpp -o supermarket_inventory.exe
```

## 运行程序

```powershell
.\build\supermarket_inventory.exe
```

程序启动后，根据菜单输入数字并按回车即可完成对应操作。建议从项目根目录运行程序，确保能正确读取和写入 `data/products.csv`。

## 数据保存

商品数据文件位于：

```text
data/products.csv
```

程序启动时会自动读取该文件；执行保存操作或正常退出时，会把当前库存数据写回该文件。请不要在 Excel 等软件占用 CSV 文件时保存程序数据，否则可能导致写入失败。

## 自动化测试

在 VS Code 中可以运行任务 `test inventory (MSVC)`，也可以在终端执行：

```powershell
.\scripts\test_msvc.cmd
```

如果本机安装了 MinGW-w64，也可以手动编译测试程序：

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_inventory.cpp src/Product.cpp src/CsvUtil.cpp src/InventoryManager.cpp -o test_inventory.exe
.\test_inventory.exe
```

测试程序使用标准库 `assert`。测试通过时不会出现断言失败；损坏 CSV 行测试会输出“跳过 1 行无效数据”的提示，这是用于验证恢复策略的预期日志。

## 常见问题

### 已安装 C/C++ 扩展，为什么仍然不能编译？

因为 VS Code 扩展不是编译器。扩展只提供编辑器能力，真正负责编译的是 MSVC、MinGW-w64、Clang 等工具链。本项目通过 `scripts\build_msvc.cmd` 调用 Visual Studio 2022 的 MSVC 工具链。

### 为什么 Visual Studio 2022 可以编译？

Visual Studio 2022 会在自己的开发环境中自动加载 MSVC、Windows SDK 和相关环境变量，所以即使没有把 `cl.exe` 加入系统环境变量，也可以在 Visual Studio 2022 中正常编译。

### 为什么 ESP32 工程可以通过 Espressif 扩展运行？

ESP-IDF 扩展使用的是 Espressif 提供的交叉编译工具链，例如 Xtensa 或 RISC-V GCC。它和普通 Windows C++ 项目使用的 MSVC 不是同一套编译器。

### 是否必须配置系统环境变量？

通常不需要。本项目的 MSVC 构建脚本会自动查找 Visual Studio 安装路径，并调用 `vcvars64.bat` 设置临时编译环境。只有在你希望在任意终端直接输入 `cl` 命令时，才需要额外配置环境。

## 设计文档

项目设计、测试和风险说明见：

- [需求分析、架构设计与测试报告](docs/design-and-test-report.md)

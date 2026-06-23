# 在 VS Code 中使用超市商品库存管理系统

本文说明如何在 Windows 和 VS Code 中编译、运行、调试及测试本项目。

## 一、准备环境

需要安装：

1. Visual Studio Code。
2. VS Code 扩展 **C/C++**，扩展发布者为 Microsoft。
3. Visual Studio 2022，并安装“使用 C++ 的桌面开发”工作负载。

> C/C++ 扩展只提供代码补全和调试支持，不包含 C++ 编译器。本项目使用 Visual Studio 2022 自带的 MSVC 编译器。构建脚本会自动查找 Visual Studio，因此通常不需要把 `cl.exe` 手动加入系统环境变量。

## 二、打开项目

1. 启动 VS Code。
2. 选择“文件” -> “打开文件夹”。
3. 打开项目根目录：

   ```text
   E:\周\cpp_homework
   ```

4. 如果 VS Code 询问是否信任此文件夹，选择“是，我信任作者”。

必须打开整个 `cpp_homework` 文件夹，不能只打开单个 `.cpp` 文件，否则 `.vscode` 中的构建和调试配置不会生效。

## 三、编译项目

按 `Ctrl+Shift+B`，VS Code 会执行默认任务：

```text
build supermarket_inventory (MSVC)
```

编译成功后生成：

```text
build\supermarket_inventory.exe
```

也可以打开 VS Code 集成终端，执行：

```powershell
.\scripts\build_msvc.cmd
```

## 四、运行系统

### 方法一：不调试运行

先按 `Ctrl+Shift+B` 完成编译，然后在集成终端执行：

```powershell
.\build\supermarket_inventory.exe
```

### 方法二：调试运行

1. 打开左侧“运行和调试”面板。
2. 选择 `Debug supermarket_inventory (MSVC)`。
3. 按 `F5`。

VS Code 会先自动编译，再在集成终端启动程序。可以在源码行号左侧单击设置断点，使用 `F10` 单步跳过、`F11` 单步进入、`Shift+F11` 单步跳出。

## 五、使用菜单

程序启动后，根据终端中的菜单输入数字并按回车。系统支持：

- 添加食品、日用品和电子产品；
- 删除和修改商品；
- 按编号或名称查询；
- 显示全部商品；
- 库存统计、低库存查询和排序；
- 保存数据并退出。

价格和库存等字段必须输入合法数字。输入格式错误时，程序会提示重新输入，不需要重启。

## 六、数据保存与恢复

商品数据保存在：

```text
data\products.csv
```

程序启动时自动读取该文件，执行保存操作或正常退出时写回文件。因此，下次运行可以继续处理上次的数据。

注意事项：

- 请从项目根目录运行程序；调试配置已自动将工作目录设置为项目根目录。
- 不要在 Excel 中打开 CSV 的同时让程序保存，否则文件可能被占用。
- 如果 CSV 中存在损坏记录，程序会跳过无效行并输出提示，其他有效数据仍会继续加载。
- 修改数据后应使用菜单中的保存或正常退出，避免直接关闭终端造成未保存数据丢失。

## 七、运行自动化测试

1. 按 `Ctrl+Shift+P` 打开命令面板。
2. 输入并选择“任务: 运行任务”。
3. 选择 `test inventory (MSVC)`。

也可以在集成终端执行：

```powershell
.\scripts\test_msvc.cmd
```

测试通过时命令正常结束。测试期间出现下面的输出属于预期行为，它用于验证损坏 CSV 行的恢复策略：

```text
提示：读取文件时跳过 1 行无效数据。
```

## 八、常见问题

### 提示找不到 C++ 编译工具

确认 Visual Studio Installer 中已经安装“使用 C++ 的桌面开发”，其中应包含 MSVC x64/x86 生成工具和 Windows SDK。安装后重启 VS Code，再执行构建。

### C/C++ 扩展已经安装，但仍不能编译

这是正常的职责区别：VS Code 的 C/C++ 扩展不是编译器。Visual Studio 2022 能编译，是因为它会自动加载自身的 MSVC 工具链环境；ESP-IDF 工程能编译，是因为 Espressif 扩展使用自己的交叉编译工具链。本项目通过 `scripts\build_msvc.cmd` 自动加载 Visual Studio 的 MSVC 环境。

### 代码下方出现红色波浪线，但项目能编译

打开命令面板并运行 `C/C++: Reset IntelliSense Database`，然后重新加载 VS Code 窗口。项目的 IntelliSense 配置位于 `.vscode\c_cpp_properties.json`。

### 按 F5 提示找不到可执行文件

先检查“终端”面板中的编译错误。也可以手动执行：

```powershell
.\scripts\build_msvc.cmd
```

确认 `build\supermarket_inventory.exe` 已生成后再次按 `F5`。

### 程序没有读取到原有数据

确认当前工作目录是项目根目录，并检查 `data\products.csv` 是否存在。使用项目自带的 `F5` 调试配置或从项目根目录运行命令，可以避免工作目录错误。

## 九、相关配置文件

| 文件 | 作用 |
| --- | --- |
| `.vscode\tasks.json` | 定义构建和测试任务 |
| `.vscode\launch.json` | 定义 F5 调试配置 |
| `.vscode\c_cpp_properties.json` | 定义 IntelliSense、头文件路径和 C++17 标准 |
| `scripts\build_msvc.cmd` | 自动定位 MSVC 并编译主程序 |
| `scripts\test_msvc.cmd` | 编译并运行自动化测试 |

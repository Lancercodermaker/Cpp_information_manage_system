# 超市商品库存管理系统

这是一个 C++17 控制台程序，用于管理超市商品库存。系统支持食品、日用品、电子产品三类商品，使用继承和多态建模，使用 `std::vector<std::shared_ptr<Product>>` 管理批量数据，并通过 CSV 文件保存和读取历史数据。

## 编译

当前工作区已配置为使用本机 Visual Studio 2022 的 MSVC 工具链。推荐在 VS Code 中按 `Ctrl+Shift+B` 执行默认构建任务，或使用下面命令：

```powershell
.\scripts\build_msvc.cmd
```

如果安装了 MinGW-w64，也可以使用：

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude src/*.cpp -o supermarket_inventory.exe
```

## 运行

```powershell
.\build\supermarket_inventory.exe
```

## 数据文件

程序从 `data/products.csv` 读取数据，并在保存或退出时写回。

## 自动化测试

VS Code 中可运行任务 `test inventory (MSVC)`，或使用：

```powershell
.\scripts\test_msvc.cmd
```

如果安装了 MinGW-w64，也可以使用：

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_inventory.cpp src/Product.cpp src/CsvUtil.cpp src/InventoryManager.cpp -o test_inventory.exe
.\test_inventory.exe
```

测试程序使用标准库 `assert`，通过时没有断言失败；损坏 CSV 行测试会输出一条“跳过 1 行无效数据”的预期日志。

# 超市商品库存管理系统实现计划

> **给执行者的说明：** 按任务逐步实现，每个任务完成后运行对应验证命令。若编译、运行或测试失败，不要猜测修复；先复现、读取错误、定位根因，再做最小修改。

**目标：** 实现一个 C++17 控制台版超市商品库存管理系统，覆盖面向对象建模、派生与多态、批量数据管理、CSV 文件持久化、鲁棒输入、查询统计和课程设计文档。

**架构：** 系统分为商品模型、库存管理、CSV 工具、输入校验、菜单界面和程序入口。商品使用 `Product` 抽象基类，派生出 `FoodProduct`、`DailyProduct`、`ElectronicProduct`，并由 `InventoryManager` 使用 `std::vector<std::shared_ptr<Product>>` 统一管理。

**技术栈：** C++17、STL、智能指针、CSV 文本文件、控制台菜单、标准库 `assert` 自动化测试。

---

## 一、需求分析

### 1. 项目背景

本项目用于管理超市商品库存信息。系统通过控制台菜单完成商品添加、删除、修改、查询、展示、统计、排序、库存预警、保存和读取文件等操作。项目主题贴合课程对“信息管理系统”的要求，数据结构清晰，适合展示类设计、继承、多态、文件操作和异常输入处理。

### 2. 用户角色

- 管理员：录入、维护、查询和统计商品数据。
- 课程验收者：检查系统是否满足面向对象、文件读写、批量数据管理、界面友好和鲁棒性要求。

### 3. 功能需求

- 支持添加食品、日用品、电子产品三类商品。
- 支持按商品编号删除商品。
- 支持按商品编号修改商品通用字段和类别专属字段。
- 支持按编号精确查询、按名称模糊查询、按类别筛选。
- 支持以表格形式显示全部商品。
- 支持统计商品数量、库存总件数、库存总成本、预计销售额、预计利润和分类数量。
- 支持输入库存阈值并显示低库存商品。
- 支持按售价或库存升序、降序显示。
- 支持启动时读取 `data/products.csv`。
- 支持主动保存和退出保存。
- 支持处理非数字输入、负数价格、负数库存、重复编号、空编号和损坏 CSV 行。

### 4. 非功能需求

- 使用 C++17 标准库完成，不依赖 Qt、EasyX 等第三方图形库。
- 使用面向对象范式设计实体和接口。
- 使用派生类和多态体现相似商品类别的差异。
- 使用 STL 容器和智能指针管理批量对象。
- 文件职责清晰，方便阅读和维护。
- 程序多次运行后可以继续处理之前保存的数据。
- 控制台界面简洁、清晰、美观。

---

## 二、架构设计

### 1. 文件结构

| 文件 | 职责 |
| --- | --- |
| `include/Product.h` | 定义商品基类和三个派生类接口 |
| `src/Product.cpp` | 实现商品字段、派生类、多态接口和表格显示 |
| `include/InventoryManager.h` | 定义库存管理类和统计结构体 |
| `src/InventoryManager.cpp` | 实现增删改查、统计、排序、预警和文件读写 |
| `include/CsvUtil.h` | 定义 CSV 工具函数 |
| `src/CsvUtil.cpp` | 实现 CSV 字段转义、拼接和解析 |
| `include/InputHelper.h` | 定义输入校验函数 |
| `src/InputHelper.cpp` | 实现整数、小数、字符串输入校验 |
| `include/Menu.h` | 定义菜单界面类 |
| `src/Menu.cpp` | 实现控制台菜单流程 |
| `src/main.cpp` | 程序入口，加载数据并启动菜单 |
| `tests/test_inventory.cpp` | 核心功能自动化测试 |
| `docs/design-and-test-report.md` | 课程设计与测试报告 |
| `README.md` | 编译、运行和测试说明 |
| `data/products.csv` | 商品数据文件 |

### 2. 类边界

- `Product` 只表示商品本身，保存通用字段并暴露多态接口，不负责文件读写和集合管理。
- `FoodProduct`、`DailyProduct`、`ElectronicProduct` 只负责各自类别的专属字段。
- `InventoryManager` 只负责商品集合的业务操作，不直接读取键盘输入。
- `CsvUtil` 只负责 CSV 文本格式，不了解商品业务规则。
- `InputHelper` 只负责输入鲁棒性，不操作商品集合。
- `Menu` 只负责交互流程，把用户输入转换为对 `InventoryManager` 的调用。
- `main` 只负责初始化、加载数据、启动菜单和退出保存。

### 3. 核心接口

`Product` 抽象基类提供：

```cpp
virtual std::string getCategory() const = 0;
virtual std::vector<std::string> getSpecialFields() const = 0;
virtual void updateSpecialFields(const std::vector<std::string>& fields) = 0;
virtual std::shared_ptr<Product> clone() const = 0;
std::vector<std::string> toCsvFields() const;
virtual void printRow(std::ostream& os) const;
```

`InventoryManager` 提供：

```cpp
bool addProduct(std::shared_ptr<Product> product);
bool removeById(const std::string& id);
std::shared_ptr<Product> findById(const std::string& id) const;
std::vector<std::shared_ptr<Product>> findByName(const std::string& keyword) const;
std::vector<std::shared_ptr<Product>> findByCategory(const std::string& category) const;
std::vector<std::shared_ptr<Product>> getLowStockProducts(int threshold) const;
std::vector<std::shared_ptr<Product>> sortedByPrice(bool ascending) const;
std::vector<std::shared_ptr<Product>> sortedByStock(bool ascending) const;
InventoryStats calculateStats() const;
bool loadFromFile(const std::string& path);
bool saveToFile(const std::string& path) const;
```

### 4. 数据文件格式

数据保存到 `data/products.csv`，第一行为表头，后续每行一个商品：

```csv
category,id,name,brand,costPrice,sellPrice,stock,field1,field2
Food,P001,Milk,Mengniu,3.500000,5.000000,100,2026-12-01,Inner Mongolia
Daily,D001,Detergent,BlueMoon,18.000000,29.900000,50,Liquid cleaner,Clothes
Electronic,E001,Kettle,Midea,65.000000,99.000000,20,12,1500
```

读取时根据 `category` 恢复为对应派生类对象。

---

## 三、风险与测试策略

### 1. 主要风险

- CSV 字段中含逗号或双引号，导致字段错位。
- 文件不存在、空文件或损坏行导致程序崩溃。
- 用户输入非数字，导致菜单或数值读取失败。
- 商品编号重复，破坏唯一性。
- 电子产品字段需要整数，格式错误时可能抛异常。
- 排序时误改原始数据顺序。
- 表格字段过长导致界面错位。

### 2. 风险控制

- `CsvUtil` 统一处理 CSV 转义和解析。
- `loadFromFile` 对无效行采用跳过策略。
- `InputHelper` 使用整行读取和严格解析。
- `InventoryManager::addProduct` 统一检查编号唯一性。
- 电子产品字段转换前检查格式。
- 排序函数返回拷贝后的指针数组，不改变原始集合。
- 商品表格使用固定列宽并截断过长字段。

### 3. 测试方式

项目提供两类测试：

- 自动化测试：`tests/test_inventory.cpp` 使用标准库 `assert`，覆盖 CSV、商品多态、库存业务和文件持久化。
- 手动测试：通过控制台菜单执行添加、删除、修改、查询、统计、预警、排序、保存和重启读取。

自动化测试编译命令：

```powershell
.\scripts\test_msvc.cmd
```

自动化测试运行命令：

```powershell
.\build\test_inventory.exe
```

主程序编译命令：

```powershell
.\scripts\build_msvc.cmd
```

主程序运行命令：

```powershell
.\build\supermarket_inventory.exe
```

---

## 四、可执行任务

### 任务 1：创建项目骨架

**文件：**

- 创建 `include/Product.h`
- 创建 `src/Product.cpp`
- 创建 `include/InventoryManager.h`
- 创建 `src/InventoryManager.cpp`
- 创建 `include/CsvUtil.h`
- 创建 `src/CsvUtil.cpp`
- 创建 `include/InputHelper.h`
- 创建 `src/InputHelper.cpp`
- 创建 `include/Menu.h`
- 创建 `src/Menu.cpp`
- 创建 `src/main.cpp`
- 创建 `tests/test_inventory.cpp`
- 创建 `data/products.csv`
- 创建 `README.md`

**步骤：**

- [ ] 创建 `include`、`src`、`tests`、`data`、`docs` 目录。
- [ ] 写入各头文件的类接口。
- [ ] 写入空的源文件，保证文件结构完整。
- [ ] 写入 `tests/test_inventory.cpp`，先让测试因为实现缺失而失败。
- [ ] 运行自动化测试编译命令，确认失败原因是未实现函数；若普通终端找不到 `cl`，通过 `VsDevCmd.bat` 加载 MSVC 环境。

### 任务 2：实现商品多态模型

**文件：**

- 修改 `include/Product.h`
- 修改 `src/Product.cpp`

**步骤：**

- [ ] 实现 `Product` 构造函数、getter 和 setter。
- [ ] 实现 `FoodProduct` 的类别、专属字段、字段更新和克隆。
- [ ] 实现 `DailyProduct` 的类别、专属字段、字段更新和克隆。
- [ ] 实现 `ElectronicProduct` 的类别、专属字段、字段更新和克隆。
- [ ] 实现 `toCsvFields()`。
- [ ] 实现 `printRow()`。
- [ ] 运行自动化测试，确认商品多态相关断言通过。

### 任务 3：实现 CSV 工具和文件持久化

**文件：**

- 修改 `include/CsvUtil.h`
- 修改 `src/CsvUtil.cpp`
- 修改 `src/InventoryManager.cpp`

**步骤：**

- [ ] 实现 `CsvUtil::escapeField()`。
- [ ] 实现 `CsvUtil::parseLine()`。
- [ ] 实现 `CsvUtil::joinLine()`。
- [ ] 实现 `InventoryManager::saveToFile()`，写入表头和商品行。
- [ ] 实现 `InventoryManager::loadFromFile()`，读取 CSV 并恢复派生类对象。
- [ ] 对损坏行执行跳过处理。
- [ ] 运行自动化测试，确认 CSV 和持久化测试通过。

### 任务 4：实现库存业务操作

**文件：**

- 修改 `src/InventoryManager.cpp`

**步骤：**

- [ ] 实现 `addProduct()`，拒绝空指针、空编号和重复编号。
- [ ] 实现 `removeById()`。
- [ ] 实现 `findById()`、`findByName()`、`findByCategory()`。
- [ ] 实现 `calculateStats()`。
- [ ] 实现 `countByCategory()`。
- [ ] 实现 `getLowStockProducts()`。
- [ ] 实现 `sortedByPrice()` 和 `sortedByStock()`。
- [ ] 运行自动化测试，确认库存业务断言通过。

### 任务 5：实现鲁棒输入工具

**文件：**

- 修改 `include/InputHelper.h`
- 修改 `src/InputHelper.cpp`

**步骤：**

- [ ] 实现 `readInt()`，支持范围检查和循环重输。
- [ ] 实现 `readDouble()`，拒绝负值和非法字符。
- [ ] 实现 `readNonEmptyLine()`，拒绝空字符串。
- [ ] 实现 `readOptionalLine()`，允许直接回车。
- [ ] 在菜单中输入 `abc`、`-1`、越界数字，确认系统重新提示。

### 任务 6：实现控制台菜单

**文件：**

- 修改 `include/Menu.h`
- 修改 `src/Menu.cpp`
- 修改 `src/main.cpp`

**步骤：**

- [ ] 实现主菜单显示。
- [ ] 实现添加商品流程。
- [ ] 实现删除商品流程。
- [ ] 实现修改商品流程。
- [ ] 实现查询商品流程。
- [ ] 实现显示全部商品。
- [ ] 实现库存统计。
- [ ] 实现库存预警。
- [ ] 实现排序显示。
- [ ] 实现保存和退出。
- [ ] 更新 `main()`，启动时读取文件，退出时保存文件。

### 任务 7：编写课程文档

**文件：**

- 创建 `docs/design-and-test-report.md`
- 修改 `README.md`

**步骤：**

- [ ] 写需求分析。
- [ ] 写架构设计。
- [ ] 写 UML 类图。
- [ ] 写数据流说明。
- [ ] 写关键实现说明。
- [ ] 写自动化测试说明。
- [ ] 写手动测试用例。
- [ ] 写风险分析。
- [ ] 在 README 中补充编译、运行、测试命令。

### 任务 8：最终验证

**步骤：**

- [ ] 运行主程序编译命令。
- [ ] 运行自动化测试编译命令。
- [ ] 运行 `test_inventory.exe`。
- [ ] 运行 `supermarket_inventory.exe`。
- [ ] 手动添加三类商品。
- [ ] 测试重复编号、负价格、负库存。
- [ ] 测试查询、统计、预警和排序。
- [ ] 保存后重启，确认数据仍存在。
- [ ] 在 CSV 中追加损坏行，确认程序不崩溃。
- [ ] 把实际测试结果记录到 `docs/design-and-test-report.md`。

---

## 五、计划自检

### 1. 架构边界检查

- 商品实体、库存业务、CSV 工具、输入校验、菜单界面和程序入口边界清晰。
- `Menu` 只负责交互，不直接维护商品集合。
- `InventoryManager` 不直接读取键盘输入。
- CSV 解析逻辑不散落在业务代码中。
- 排序、统计、查询均由库存管理类统一提供。

### 2. 测试方式检查

- 自动化测试覆盖核心业务和文件持久化。
- 手动测试覆盖菜单交互和输入鲁棒性。
- 损坏 CSV 行、重复编号、负数输入和保存重启均有测试路径。
- 当前机器已安装 Visual Studio 2022 C++ 工具链，但普通 PowerShell 未加载 MSVC 环境；工作区通过 `.vscode/tasks.json` 调用 `scripts/build_msvc.cmd` 和 `scripts/test_msvc.cmd`，脚本内部加载 `D:\C++\Common7\Tools\VsDevCmd.bat` 后编译。

### 3. 风险点检查

- CSV 特殊字符由 `CsvUtil` 处理。
- 文件缺失时允许空库存启动。
- 损坏行被跳过，不终止程序。
- 非法输入由 `InputHelper` 循环重试。
- 重复编号由 `InventoryManager` 拦截。
- 排序返回拷贝，不改变原集合顺序。

### 4. 规格覆盖检查

- 面向对象实体设计：由 `Product` 和派生类覆盖。
- 批量数据管理：由 `std::vector<std::shared_ptr<Product>>` 覆盖。
- 派生和多态：由三类商品派生类覆盖。
- 文件多次运行持久化：由 CSV 读取和保存覆盖。
- 输入交互鲁棒性：由 `InputHelper` 和菜单流程覆盖。
- 设计和测试报告：由 `docs/design-and-test-report.md` 覆盖。

### 5. 占位项检查

本计划没有保留含糊的后续实现项。每个任务都说明了文件、步骤和验证方式。

#include "Menu.h"

#include "InputHelper.h"

#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <utility>

namespace {
std::string categoryFromChoice(int choice) {
    if (choice == 1) {
        return "Food";
    }
    if (choice == 2) {
        return "Daily";
    }
    return "Electronic";
}

std::string categoryName(const std::string& category) {
    if (category == "Food") {
        return "食品";
    }
    if (category == "Daily") {
        return "日用品";
    }
    if (category == "Electronic") {
        return "电子产品";
    }
    return category;
}

bool parseDoubleStrict(const std::string& text, double& value) {
    try {
        std::size_t pos = 0;
        double parsed = std::stod(text, &pos);
        if (pos != text.size()) {
            return false;
        }
        value = parsed;
        return true;
    } catch (...) {
        return false;
    }
}

bool parseIntStrict(const std::string& text, int& value) {
    try {
        std::size_t pos = 0;
        int parsed = std::stoi(text, &pos);
        if (pos != text.size()) {
            return false;
        }
        value = parsed;
        return true;
    } catch (...) {
        return false;
    }
}

double readOptionalDouble(const std::string& prompt, double currentValue) {
    while (true) {
        const std::string text = InputHelper::readOptionalLine(prompt);
        if (text.empty()) {
            return currentValue;
        }
        double value = 0.0;
        if (parseDoubleStrict(text, value) && value >= 0.0) {
            return value;
        }
        std::cout << "输入无效，请重新输入。\n";
    }
}

int readOptionalInt(const std::string& prompt, int currentValue) {
    while (true) {
        const std::string text = InputHelper::readOptionalLine(prompt);
        if (text.empty()) {
            return currentValue;
        }
        int value = 0;
        if (parseIntStrict(text, value) && value >= 0) {
            return value;
        }
        std::cout << "输入无效，请重新输入。\n";
    }
}
}

Menu::Menu(InventoryManager& manager, std::string dataPath)
    : manager(manager), dataPath(std::move(dataPath)) {}

void Menu::showMainMenu() const {
    std::cout << "\n========== 超市商品库存管理系统 ==========\n"
              << "1. 添加商品\n"
              << "2. 删除商品\n"
              << "3. 修改商品\n"
              << "4. 查询商品\n"
              << "5. 显示全部商品\n"
              << "6. 库存统计\n"
              << "7. 库存预警\n"
              << "8. 排序显示\n"
              << "9. 保存数据\n"
              << "0. 退出系统\n"
              << "==========================================\n";
}

void Menu::run() {
    while (true) {
        showMainMenu();
        const int choice = InputHelper::readInt("请选择功能：", 0, 9);
        switch (choice) {
            case 1:
                addProductFlow();
                break;
            case 2:
                deleteProductFlow();
                break;
            case 3:
                updateProductFlow();
                break;
            case 4:
                searchProductFlow();
                break;
            case 5:
                showAllProducts();
                break;
            case 6:
                showStats();
                break;
            case 7:
                showLowStock();
                break;
            case 8:
                showSortedProducts();
                break;
            case 9:
                std::cout << (manager.saveToFile(dataPath) ? "保存成功。\n" : "保存失败。\n");
                break;
            case 0:
                manager.saveToFile(dataPath);
                std::cout << "数据已保存，系统退出。\n";
                return;
        }
    }
}

void Menu::addProductFlow() {
    std::cout << "\n请选择商品类别：\n"
              << "1. 食品\n"
              << "2. 日用品\n"
              << "3. 电子产品\n";
    const int categoryChoice = InputHelper::readInt("类别：", 1, 3);
    const std::string id = InputHelper::readNonEmptyLine("商品编号：");
    if (manager.findById(id) != nullptr) {
        std::cout << "添加失败：商品编号已存在。\n";
        return;
    }

    const std::string name = InputHelper::readNonEmptyLine("商品名称：");
    const std::string brand = InputHelper::readNonEmptyLine("品牌：");
    const double costPrice = InputHelper::readDouble("进价：", 0.0);
    const double sellPrice = InputHelper::readDouble("售价：", 0.0);
    const int stock = InputHelper::readInt("库存数量：", 0, 100000000);

    std::shared_ptr<Product> product;
    if (categoryChoice == 1) {
        const std::string expirationDate = InputHelper::readNonEmptyLine("保质期：");
        const std::string origin = InputHelper::readNonEmptyLine("产地：");
        product = std::make_shared<FoodProduct>(id, name, brand, costPrice, sellPrice, stock,
                                                expirationDate, origin);
    } else if (categoryChoice == 2) {
        const std::string material = InputHelper::readNonEmptyLine("材质：");
        const std::string usage = InputHelper::readNonEmptyLine("用途：");
        product = std::make_shared<DailyProduct>(id, name, brand, costPrice, sellPrice, stock,
                                                 material, usage);
    } else {
        const int warrantyMonths = InputHelper::readInt("保修月数：", 0, 10000);
        const int power = InputHelper::readInt("功率：", 0, 100000000);
        product = std::make_shared<ElectronicProduct>(id, name, brand, costPrice, sellPrice, stock,
                                                      warrantyMonths, power);
    }

    std::cout << (manager.addProduct(product) ? "添加成功。\n" : "添加失败。\n");
}

void Menu::deleteProductFlow() {
    const std::string id = InputHelper::readNonEmptyLine("请输入要删除的商品编号：");
    std::cout << (manager.removeById(id) ? "删除成功。\n" : "删除失败：未找到该编号。\n");
}

void Menu::updateProductFlow() {
    const std::string id = InputHelper::readNonEmptyLine("请输入要修改的商品编号：");
    std::shared_ptr<Product> product = manager.findById(id);
    if (!product) {
        std::cout << "修改失败：未找到该编号。\n";
        return;
    }

    std::cout << "直接回车表示保留原值。\n";
    const std::string name = InputHelper::readOptionalLine("商品名称(" + product->getName() + ")：");
    if (!name.empty()) {
        product->setName(name);
    }

    const std::string brand = InputHelper::readOptionalLine("品牌(" + product->getBrand() + ")：");
    if (!brand.empty()) {
        product->setBrand(brand);
    }

    product->setCostPrice(readOptionalDouble("进价：", product->getCostPrice()));
    product->setSellPrice(readOptionalDouble("售价：", product->getSellPrice()));
    product->setStock(readOptionalInt("库存数量：", product->getStock()));

    const std::vector<std::string> special = product->getSpecialFields();
    if (product->getCategory() == "Food") {
        const std::string expirationDate =
            InputHelper::readOptionalLine("保质期(" + special[0] + ")：");
        const std::string origin = InputHelper::readOptionalLine("产地(" + special[1] + ")：");
        product->updateSpecialFields({expirationDate, origin});
    } else if (product->getCategory() == "Daily") {
        const std::string material = InputHelper::readOptionalLine("材质(" + special[0] + ")：");
        const std::string usage = InputHelper::readOptionalLine("用途(" + special[1] + ")：");
        product->updateSpecialFields({material, usage});
    } else {
        const int warrantyMonths = readOptionalInt("保修月数：", std::stoi(special[0]));
        const int power = readOptionalInt("功率：", std::stoi(special[1]));
        product->updateSpecialFields({std::to_string(warrantyMonths), std::to_string(power)});
    }

    std::cout << "修改成功。\n";
}

void Menu::searchProductFlow() const {
    std::cout << "\n查询方式：\n"
              << "1. 按编号查询\n"
              << "2. 按名称查询\n"
              << "3. 按类别查询\n";
    const int choice = InputHelper::readInt("请选择：", 1, 3);
    if (choice == 1) {
        const std::string id = InputHelper::readNonEmptyLine("商品编号：");
        std::shared_ptr<Product> product = manager.findById(id);
        if (!product) {
            std::cout << "未找到匹配商品。\n";
            return;
        }
        printProducts({product});
    } else if (choice == 2) {
        const std::string keyword = InputHelper::readNonEmptyLine("名称关键词：");
        printProducts(manager.findByName(keyword));
    } else {
        std::cout << "1. 食品\n2. 日用品\n3. 电子产品\n";
        const int categoryChoice = InputHelper::readInt("类别：", 1, 3);
        printProducts(manager.findByCategory(categoryFromChoice(categoryChoice)));
    }
}

void Menu::showAllProducts() const {
    printProducts(manager.getAll());
}

void Menu::showStats() const {
    const InventoryStats stats = manager.calculateStats();
    std::cout << "\n========== 库存统计 ==========\n"
              << "商品种类数：" << stats.productCount << '\n'
              << "库存总件数：" << stats.totalStock << '\n'
              << "库存总成本：" << std::fixed << std::setprecision(2) << stats.totalCost << '\n'
              << "预计销售额：" << stats.totalSalesValue << '\n'
              << "预计利润：" << stats.expectedProfit << '\n'
              << "分类数量：\n";
    for (const auto& item : manager.countByCategory()) {
        std::cout << "  " << categoryName(item.first) << "：" << item.second << '\n';
    }
}

void Menu::showLowStock() const {
    const int threshold = InputHelper::readInt("请输入库存预警阈值：", 0, 100000000);
    printProducts(manager.getLowStockProducts(threshold));
}

void Menu::showSortedProducts() const {
    std::cout << "\n排序方式：\n"
              << "1. 按售价升序\n"
              << "2. 按售价降序\n"
              << "3. 按库存升序\n"
              << "4. 按库存降序\n";
    const int choice = InputHelper::readInt("请选择：", 1, 4);
    if (choice == 1) {
        printProducts(manager.sortedByPrice(true));
    } else if (choice == 2) {
        printProducts(manager.sortedByPrice(false));
    } else if (choice == 3) {
        printProducts(manager.sortedByStock(true));
    } else {
        printProducts(manager.sortedByStock(false));
    }
}

void Menu::printProducts(const std::vector<std::shared_ptr<Product>>& products) const {
    if (products.empty()) {
        std::cout << "没有商品记录。\n";
        return;
    }

    std::cout << std::left
              << std::setw(12) << "类别"
              << std::setw(12) << "编号"
              << std::setw(18) << "名称"
              << std::setw(16) << "品牌"
              << std::setw(12) << "进价"
              << std::setw(12) << "售价"
              << std::setw(10) << "库存"
              << std::setw(18) << "字段1"
              << std::setw(18) << "字段2"
              << '\n';
    std::cout << std::string(128, '-') << '\n';
    for (const auto& product : products) {
        if (product) {
            product->printRow(std::cout);
        }
    }
}

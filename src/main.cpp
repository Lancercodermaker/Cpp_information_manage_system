#include "InventoryManager.h"
#include "Menu.h"

#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    const std::string dataPath = "data/products.csv";
    InventoryManager manager;
    if (!manager.loadFromFile(dataPath)) {
        std::cout << "提示：数据文件读取失败，将以空库存启动。\n";
    }

    Menu menu(manager, dataPath);
    menu.run();

    if (!manager.saveToFile(dataPath)) {
        std::cout << "提示：数据保存失败，请检查文件权限。\n";
    }
    return 0;
}

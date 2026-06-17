#pragma once

#include "InventoryManager.h"

#include <memory>
#include <string>
#include <vector>

class Menu {
private:
    InventoryManager& manager;
    std::string dataPath;

    void showMainMenu() const;
    void addProductFlow();
    void deleteProductFlow();
    void updateProductFlow();
    void searchProductFlow() const;
    void showAllProducts() const;
    void showStats() const;
    void showLowStock() const;
    void showSortedProducts() const;
    void printProducts(const std::vector<std::shared_ptr<Product>>& products) const;

public:
    Menu(InventoryManager& manager, std::string dataPath);
    void run();
};

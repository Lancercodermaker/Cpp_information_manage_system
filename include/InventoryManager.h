#pragma once

#include "Product.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

struct InventoryStats {
    int productCount = 0;
    int totalStock = 0;
    double totalCost = 0.0;
    double totalSalesValue = 0.0;
    double expectedProfit = 0.0;
};

class InventoryManager {
private:
    std::vector<std::shared_ptr<Product>> products;

public:
    bool addProduct(std::shared_ptr<Product> product);
    bool removeById(const std::string& id);
    std::shared_ptr<Product> findById(const std::string& id) const;
    std::vector<std::shared_ptr<Product>> findByName(const std::string& keyword) const;
    std::vector<std::shared_ptr<Product>> findByCategory(const std::string& category) const;
    const std::vector<std::shared_ptr<Product>>& getAll() const;
    std::vector<std::shared_ptr<Product>> getLowStockProducts(int threshold) const;
    std::vector<std::shared_ptr<Product>> sortedByPrice(bool ascending) const;
    std::vector<std::shared_ptr<Product>> sortedByStock(bool ascending) const;
    InventoryStats calculateStats() const;
    std::vector<std::pair<std::string, int>> countByCategory() const;
    bool loadFromFile(const std::string& path);
    bool saveToFile(const std::string& path) const;
};

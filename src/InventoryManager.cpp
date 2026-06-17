#include "InventoryManager.h"

#include "CsvUtil.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>

namespace {
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

std::shared_ptr<Product> createProductFromFields(const std::vector<std::string>& fields) {
    if (fields.size() < 9) {
        return nullptr;
    }

    const std::string& category = fields[0];
    const std::string& id = fields[1];
    const std::string& name = fields[2];
    const std::string& brand = fields[3];
    double costPrice = 0.0;
    double sellPrice = 0.0;
    int stock = 0;

    if (id.empty() || name.empty() || !parseDoubleStrict(fields[4], costPrice) ||
        !parseDoubleStrict(fields[5], sellPrice) || !parseIntStrict(fields[6], stock) ||
        costPrice < 0.0 || sellPrice < 0.0 || stock < 0) {
        return nullptr;
    }

    if (category == "Food") {
        return std::make_shared<FoodProduct>(id, name, brand, costPrice, sellPrice, stock,
                                             fields[7], fields[8]);
    }
    if (category == "Daily") {
        return std::make_shared<DailyProduct>(id, name, brand, costPrice, sellPrice, stock,
                                              fields[7], fields[8]);
    }
    if (category == "Electronic") {
        int warrantyMonths = 0;
        int power = 0;
        if (!parseIntStrict(fields[7], warrantyMonths) || !parseIntStrict(fields[8], power) ||
            warrantyMonths < 0 || power < 0) {
            return nullptr;
        }
        return std::make_shared<ElectronicProduct>(id, name, brand, costPrice, sellPrice, stock,
                                                   warrantyMonths, power);
    }

    return nullptr;
}

void stripUtf8Bom(std::string& line) {
    if (line.size() >= 3 &&
        static_cast<unsigned char>(line[0]) == 0xEF &&
        static_cast<unsigned char>(line[1]) == 0xBB &&
        static_cast<unsigned char>(line[2]) == 0xBF) {
        line.erase(0, 3);
    }
}

bool isBlankLine(const std::string& line) {
    return std::all_of(line.begin(), line.end(), [](unsigned char ch) {
        return std::isspace(ch) != 0;
    });
}
}

bool InventoryManager::addProduct(std::shared_ptr<Product> product) {
    if (!product || product->getId().empty() || findById(product->getId()) != nullptr) {
        return false;
    }
    products.push_back(std::move(product));
    return true;
}

bool InventoryManager::removeById(const std::string& id) {
    const auto oldSize = products.size();
    products.erase(std::remove_if(products.begin(), products.end(),
                                  [&id](const std::shared_ptr<Product>& product) {
                                      return product && product->getId() == id;
                                  }),
                   products.end());
    return products.size() != oldSize;
}

std::shared_ptr<Product> InventoryManager::findById(const std::string& id) const {
    const auto it = std::find_if(products.begin(), products.end(),
                                 [&id](const std::shared_ptr<Product>& product) {
                                     return product && product->getId() == id;
                                 });
    return it == products.end() ? nullptr : *it;
}

std::vector<std::shared_ptr<Product>> InventoryManager::findByName(const std::string& keyword) const {
    std::vector<std::shared_ptr<Product>> result;
    for (const auto& product : products) {
        if (product && product->getName().find(keyword) != std::string::npos) {
            result.push_back(product);
        }
    }
    return result;
}

std::vector<std::shared_ptr<Product>> InventoryManager::findByCategory(const std::string& category) const {
    std::vector<std::shared_ptr<Product>> result;
    for (const auto& product : products) {
        if (product && product->getCategory() == category) {
            result.push_back(product);
        }
    }
    return result;
}

const std::vector<std::shared_ptr<Product>>& InventoryManager::getAll() const {
    return products;
}

std::vector<std::shared_ptr<Product>> InventoryManager::getLowStockProducts(int threshold) const {
    std::vector<std::shared_ptr<Product>> result;
    for (const auto& product : products) {
        if (product && product->getStock() <= threshold) {
            result.push_back(product);
        }
    }
    return result;
}

std::vector<std::shared_ptr<Product>> InventoryManager::sortedByPrice(bool ascending) const {
    std::vector<std::shared_ptr<Product>> result = products;
    std::sort(result.begin(), result.end(),
              [ascending](const std::shared_ptr<Product>& left, const std::shared_ptr<Product>& right) {
                  if (!left || !right) {
                      return static_cast<bool>(right);
                  }
                  return ascending ? left->getSellPrice() < right->getSellPrice()
                                   : left->getSellPrice() > right->getSellPrice();
              });
    return result;
}

std::vector<std::shared_ptr<Product>> InventoryManager::sortedByStock(bool ascending) const {
    std::vector<std::shared_ptr<Product>> result = products;
    std::sort(result.begin(), result.end(),
              [ascending](const std::shared_ptr<Product>& left, const std::shared_ptr<Product>& right) {
                  if (!left || !right) {
                      return static_cast<bool>(right);
                  }
                  return ascending ? left->getStock() < right->getStock()
                                   : left->getStock() > right->getStock();
              });
    return result;
}

InventoryStats InventoryManager::calculateStats() const {
    InventoryStats stats;
    stats.productCount = static_cast<int>(products.size());
    for (const auto& product : products) {
        if (!product) {
            continue;
        }
        stats.totalStock += product->getStock();
        stats.totalCost += product->getCostPrice() * product->getStock();
        stats.totalSalesValue += product->getSellPrice() * product->getStock();
    }
    stats.expectedProfit = stats.totalSalesValue - stats.totalCost;
    return stats;
}

std::vector<std::pair<std::string, int>> InventoryManager::countByCategory() const {
    std::map<std::string, int> counts;
    for (const auto& product : products) {
        if (product) {
            ++counts[product->getCategory()];
        }
    }
    return std::vector<std::pair<std::string, int>>(counts.begin(), counts.end());
}

bool InventoryManager::loadFromFile(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        return true;
    }

    std::vector<std::shared_ptr<Product>> loadedProducts;
    std::string line;
    bool firstLine = true;
    int skippedRows = 0;

    while (std::getline(in, line)) {
        stripUtf8Bom(line);
        if (isBlankLine(line)) {
            continue;
        }
        if (firstLine) {
            firstLine = false;
            if (line.rfind("category,", 0) == 0) {
                continue;
            }
        }

        std::shared_ptr<Product> product = createProductFromFields(CsvUtil::parseLine(line));
        if (!product) {
            ++skippedRows;
            continue;
        }
        if (std::find_if(loadedProducts.begin(), loadedProducts.end(),
                         [&product](const std::shared_ptr<Product>& existing) {
                             return existing && existing->getId() == product->getId();
                         }) == loadedProducts.end()) {
            loadedProducts.push_back(product);
        } else {
            ++skippedRows;
        }
    }

    products = std::move(loadedProducts);
    if (skippedRows > 0) {
        std::cerr << "提示：读取文件时跳过 " << skippedRows << " 行无效数据。\n";
    }
    return true;
}

bool InventoryManager::saveToFile(const std::string& path) const {
    std::ofstream out(path);
    if (!out.is_open()) {
        return false;
    }

    out << "category,id,name,brand,costPrice,sellPrice,stock,field1,field2\n";
    for (const auto& product : products) {
        if (product) {
            out << CsvUtil::joinLine(product->toCsvFields()) << '\n';
        }
    }
    return true;
}

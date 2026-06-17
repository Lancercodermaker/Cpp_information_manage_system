#include "Product.h"

#include <iomanip>
#include <sstream>
#include <utility>

namespace {
std::string toText(double value) {
    return std::to_string(value);
}

std::string toText(int value) {
    return std::to_string(value);
}

std::string fitWidth(const std::string& value, std::size_t maxLength) {
    if (value.size() <= maxLength) {
        return value;
    }
    if (maxLength <= 3) {
        return value.substr(0, maxLength);
    }
    return value.substr(0, maxLength - 3) + "...";
}

bool tryParseInt(const std::string& text, int& value) {
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
}

Product::Product(std::string id, std::string name, std::string brand,
                 double costPrice, double sellPrice, int stock)
    : id(std::move(id)),
      name(std::move(name)),
      brand(std::move(brand)),
      costPrice(costPrice),
      sellPrice(sellPrice),
      stock(stock) {}

const std::string& Product::getId() const {
    return id;
}

const std::string& Product::getName() const {
    return name;
}

const std::string& Product::getBrand() const {
    return brand;
}

double Product::getCostPrice() const {
    return costPrice;
}

double Product::getSellPrice() const {
    return sellPrice;
}

int Product::getStock() const {
    return stock;
}

void Product::setName(const std::string& value) {
    name = value;
}

void Product::setBrand(const std::string& value) {
    brand = value;
}

void Product::setCostPrice(double value) {
    costPrice = value;
}

void Product::setSellPrice(double value) {
    sellPrice = value;
}

void Product::setStock(int value) {
    stock = value;
}

std::vector<std::string> Product::toCsvFields() const {
    std::vector<std::string> fields = {
        getCategory(),
        id,
        name,
        brand,
        toText(costPrice),
        toText(sellPrice),
        toText(stock)
    };
    std::vector<std::string> specialFields = getSpecialFields();
    fields.insert(fields.end(), specialFields.begin(), specialFields.end());
    return fields;
}

void Product::printRow(std::ostream& os) const {
    const std::vector<std::string> specialFields = getSpecialFields();
    os << std::left
       << std::setw(12) << fitWidth(getCategory(), 11)
       << std::setw(12) << fitWidth(id, 11)
       << std::setw(18) << fitWidth(name, 17)
       << std::setw(16) << fitWidth(brand, 15)
       << std::setw(12) << costPrice
       << std::setw(12) << sellPrice
       << std::setw(10) << stock
       << std::setw(18) << fitWidth(specialFields.size() > 0 ? specialFields[0] : "", 17)
       << std::setw(18) << fitWidth(specialFields.size() > 1 ? specialFields[1] : "", 17)
       << '\n';
}

FoodProduct::FoodProduct(std::string id, std::string name, std::string brand,
                         double costPrice, double sellPrice, int stock,
                         std::string expirationDate, std::string origin)
    : Product(std::move(id), std::move(name), std::move(brand), costPrice, sellPrice, stock),
      expirationDate(std::move(expirationDate)),
      origin(std::move(origin)) {}

std::string FoodProduct::getCategory() const {
    return "Food";
}

std::vector<std::string> FoodProduct::getSpecialFields() const {
    return {expirationDate, origin};
}

void FoodProduct::updateSpecialFields(const std::vector<std::string>& fields) {
    if (fields.size() >= 1 && !fields[0].empty()) {
        expirationDate = fields[0];
    }
    if (fields.size() >= 2 && !fields[1].empty()) {
        origin = fields[1];
    }
}

std::shared_ptr<Product> FoodProduct::clone() const {
    return std::make_shared<FoodProduct>(*this);
}

DailyProduct::DailyProduct(std::string id, std::string name, std::string brand,
                           double costPrice, double sellPrice, int stock,
                           std::string material, std::string usage)
    : Product(std::move(id), std::move(name), std::move(brand), costPrice, sellPrice, stock),
      material(std::move(material)),
      usage(std::move(usage)) {}

std::string DailyProduct::getCategory() const {
    return "Daily";
}

std::vector<std::string> DailyProduct::getSpecialFields() const {
    return {material, usage};
}

void DailyProduct::updateSpecialFields(const std::vector<std::string>& fields) {
    if (fields.size() >= 1 && !fields[0].empty()) {
        material = fields[0];
    }
    if (fields.size() >= 2 && !fields[1].empty()) {
        usage = fields[1];
    }
}

std::shared_ptr<Product> DailyProduct::clone() const {
    return std::make_shared<DailyProduct>(*this);
}

ElectronicProduct::ElectronicProduct(std::string id, std::string name, std::string brand,
                                     double costPrice, double sellPrice, int stock,
                                     int warrantyMonths, int power)
    : Product(std::move(id), std::move(name), std::move(brand), costPrice, sellPrice, stock),
      warrantyMonths(warrantyMonths),
      power(power) {}

std::string ElectronicProduct::getCategory() const {
    return "Electronic";
}

std::vector<std::string> ElectronicProduct::getSpecialFields() const {
    return {toText(warrantyMonths), toText(power)};
}

void ElectronicProduct::updateSpecialFields(const std::vector<std::string>& fields) {
    int parsed = 0;
    if (fields.size() >= 1 && !fields[0].empty() && tryParseInt(fields[0], parsed) && parsed >= 0) {
        warrantyMonths = parsed;
    }
    if (fields.size() >= 2 && !fields[1].empty() && tryParseInt(fields[1], parsed) && parsed >= 0) {
        power = parsed;
    }
}

std::shared_ptr<Product> ElectronicProduct::clone() const {
    return std::make_shared<ElectronicProduct>(*this);
}

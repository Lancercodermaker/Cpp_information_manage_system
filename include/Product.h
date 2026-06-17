#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Product {
protected:
    std::string id;
    std::string name;
    std::string brand;
    double costPrice;
    double sellPrice;
    int stock;

public:
    Product(std::string id, std::string name, std::string brand,
            double costPrice, double sellPrice, int stock);
    virtual ~Product() = default;

    const std::string& getId() const;
    const std::string& getName() const;
    const std::string& getBrand() const;
    double getCostPrice() const;
    double getSellPrice() const;
    int getStock() const;

    void setName(const std::string& value);
    void setBrand(const std::string& value);
    void setCostPrice(double value);
    void setSellPrice(double value);
    void setStock(int value);

    virtual std::string getCategory() const = 0;
    virtual std::vector<std::string> getSpecialFields() const = 0;
    virtual void updateSpecialFields(const std::vector<std::string>& fields) = 0;
    virtual std::shared_ptr<Product> clone() const = 0;

    std::vector<std::string> toCsvFields() const;
    virtual void printRow(std::ostream& os) const;
};

class FoodProduct : public Product {
private:
    std::string expirationDate;
    std::string origin;

public:
    FoodProduct(std::string id, std::string name, std::string brand,
                double costPrice, double sellPrice, int stock,
                std::string expirationDate, std::string origin);
    std::string getCategory() const override;
    std::vector<std::string> getSpecialFields() const override;
    void updateSpecialFields(const std::vector<std::string>& fields) override;
    std::shared_ptr<Product> clone() const override;
};

class DailyProduct : public Product {
private:
    std::string material;
    std::string usage;

public:
    DailyProduct(std::string id, std::string name, std::string brand,
                 double costPrice, double sellPrice, int stock,
                 std::string material, std::string usage);
    std::string getCategory() const override;
    std::vector<std::string> getSpecialFields() const override;
    void updateSpecialFields(const std::vector<std::string>& fields) override;
    std::shared_ptr<Product> clone() const override;
};

class ElectronicProduct : public Product {
private:
    int warrantyMonths;
    int power;

public:
    ElectronicProduct(std::string id, std::string name, std::string brand,
                      double costPrice, double sellPrice, int stock,
                      int warrantyMonths, int power);
    std::string getCategory() const override;
    std::vector<std::string> getSpecialFields() const override;
    void updateSpecialFields(const std::vector<std::string>& fields) override;
    std::shared_ptr<Product> clone() const override;
};

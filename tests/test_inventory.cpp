#include "CsvUtil.h"
#include "InventoryManager.h"
#include "Product.h"

#include <cassert>
#include <cmath>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace {
bool nearlyEqual(double a, double b) {
    return std::fabs(a - b) < 0.0001;
}

void testCsvEscapingAndParsing() {
    std::vector<std::string> fields = {
        "Food",
        "P001",
        "Milk, low fat",
        "Brand \"A\"",
        "3.500000",
        "5.000000",
        "100",
        "2026-12-01",
        "Inner Mongolia"
    };

    const std::string line = CsvUtil::joinLine(fields);
    const std::vector<std::string> parsed = CsvUtil::parseLine(line);
    assert(parsed == fields);
}

void testProductPolymorphism() {
    FoodProduct food("P001", "Milk", "Mengniu", 3.5, 5.0, 100,
                     "2026-12-01", "Inner Mongolia");
    assert(food.getCategory() == "Food");
    assert(food.getId() == "P001");
    assert(food.getName() == "Milk");
    assert(food.getSpecialFields()[0] == "2026-12-01");

    food.updateSpecialFields({"2027-01-01", "Beijing"});
    assert(food.getSpecialFields()[0] == "2027-01-01");
    assert(food.getSpecialFields()[1] == "Beijing");

    ElectronicProduct electronic("E001", "Kettle", "Midea", 65.0, 99.0, 20, 12, 1500);
    electronic.updateSpecialFields({"24", "1800"});
    assert(electronic.getSpecialFields()[0] == "24");
    assert(electronic.getSpecialFields()[1] == "1800");

    std::shared_ptr<Product> copy = electronic.clone();
    assert(copy->getCategory() == "Electronic");
    assert(copy->getSpecialFields()[0] == "24");
}

void testInventoryOperations() {
    InventoryManager manager;
    auto food = std::make_shared<FoodProduct>("P001", "Milk", "Mengniu", 3.5, 5.0, 100,
                                              "2026-12-01", "Inner Mongolia");
    auto daily = std::make_shared<DailyProduct>("D001", "Detergent", "BlueMoon", 18.0, 29.9, 50,
                                                "Liquid cleaner", "Clothes");
    auto electronic = std::make_shared<ElectronicProduct>("E001", "Kettle", "Midea", 65.0, 99.0, 20,
                                                          12, 1500);

    assert(manager.addProduct(food));
    assert(!manager.addProduct(food));
    assert(manager.addProduct(daily));
    assert(manager.addProduct(electronic));

    assert(manager.findById("P001") != nullptr);
    assert(manager.findById("UNKNOWN") == nullptr);
    assert(manager.findByName("Milk").size() == 1);
    assert(manager.findByCategory("Electronic").size() == 1);

    const InventoryStats stats = manager.calculateStats();
    assert(stats.productCount == 3);
    assert(stats.totalStock == 170);
    assert(nearlyEqual(stats.totalCost, 3.5 * 100 + 18.0 * 50 + 65.0 * 20));
    assert(nearlyEqual(stats.totalSalesValue, 5.0 * 100 + 29.9 * 50 + 99.0 * 20));
    assert(nearlyEqual(stats.expectedProfit, stats.totalSalesValue - stats.totalCost));

    assert(manager.getLowStockProducts(30).size() == 1);
    assert(manager.sortedByPrice(true).front()->getId() == "P001");
    assert(manager.getAll().front()->getId() == "P001");

    assert(manager.removeById("D001"));
    assert(!manager.removeById("D001"));
}

void testPersistenceSkipsBrokenRows() {
    const std::string path = "build/test_products.csv";
    {
        std::ofstream out(path);
        out << "category,id,name,brand,costPrice,sellPrice,stock,field1,field2\n";
        out << "Food,P001,Milk,Mengniu,3.500000,5.000000,100,2026-12-01,Inner Mongolia\n";
        out << "Broken,row,missing,fields\n";
        out << "Electronic,E001,Kettle,Midea,65.000000,99.000000,20,12,1500\n";
    }

    InventoryManager manager;
    assert(manager.loadFromFile(path));
    assert(manager.getAll().size() == 2);
    assert(manager.findById("P001") != nullptr);
    assert(manager.findById("E001") != nullptr);

    assert(manager.saveToFile(path));

    InventoryManager reloaded;
    assert(reloaded.loadFromFile(path));
    assert(reloaded.getAll().size() == 2);
}

void testUtf8BomHeaderDoesNotLogBrokenRow() {
    const std::string path = "build/test_bom_products.csv";
    {
        std::ofstream out(path, std::ios::binary);
        out << "\xEF\xBB\xBF";
        out << "category,id,name,brand,costPrice,sellPrice,stock,field1,field2\n";
        out << "Food,P001,Milk,Mengniu,3.500000,5.000000,100,2026-12-01,Inner Mongolia\n";
    }

    std::ostringstream capturedError;
    std::streambuf* oldBuffer = std::cerr.rdbuf(capturedError.rdbuf());

    InventoryManager manager;
    assert(manager.loadFromFile(path));

    std::cerr.rdbuf(oldBuffer);

    assert(manager.getAll().size() == 1);
    assert(manager.findById("P001") != nullptr);
    assert(capturedError.str().empty());
}

void testBlankLinesAroundCsvAreIgnoredWithoutBrokenRowLog() {
    const std::string path = "build/test_blank_lines_products.csv";
    {
        std::ofstream out(path, std::ios::binary);
        out << "\xEF\xBB\xBF\r\n";
        out << "category,id,name,brand,costPrice,sellPrice,stock,field1,field2\r\n";
        out << "\r\n";
        out << "   \r\n";
        out << "\t\r\n";
        out << "Food,P001,Milk,Mengniu,3.500000,5.000000,100,2026-12-01,Inner Mongolia\r\n";
        out << "\r\n";
    }

    std::ostringstream capturedError;
    std::streambuf* oldBuffer = std::cerr.rdbuf(capturedError.rdbuf());

    InventoryManager manager;
    assert(manager.loadFromFile(path));

    std::cerr.rdbuf(oldBuffer);

    assert(manager.getAll().size() == 1);
    assert(manager.findById("P001") != nullptr);
    assert(capturedError.str().empty());
}
}

int main() {
    testCsvEscapingAndParsing();
    testProductPolymorphism();
    testInventoryOperations();
    testPersistenceSkipsBrokenRows();
    testUtf8BomHeaderDoesNotLogBrokenRow();
    testBlankLinesAroundCsvAreIgnoredWithoutBrokenRowLog();
    return 0;
}

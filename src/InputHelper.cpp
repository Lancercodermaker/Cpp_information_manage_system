#include "InputHelper.h"

#include <iostream>
#include <string>

namespace {
std::string trim(const std::string& text) {
    const std::size_t first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    const std::size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

bool parseIntStrict(const std::string& text, int& value) {
    try {
        const std::string normalized = trim(text);
        std::size_t pos = 0;
        int parsed = std::stoi(normalized, &pos);
        if (pos != normalized.size()) {
            return false;
        }
        value = parsed;
        return true;
    } catch (...) {
        return false;
    }
}

bool parseDoubleStrict(const std::string& text, double& value) {
    try {
        const std::string normalized = trim(text);
        std::size_t pos = 0;
        double parsed = std::stod(normalized, &pos);
        if (pos != normalized.size()) {
            return false;
        }
        value = parsed;
        return true;
    } catch (...) {
        return false;
    }
}

void printInvalidInput() {
    std::cout << "输入无效，请重新输入。\n";
}
}

namespace InputHelper {
int readInt(const std::string& prompt, int minValue, int maxValue) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cin.clear();
            printInvalidInput();
            continue;
        }

        int value = 0;
        if (parseIntStrict(line, value) && value >= minValue && value <= maxValue) {
            return value;
        }
        printInvalidInput();
    }
}

double readDouble(const std::string& prompt, double minValue) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cin.clear();
            printInvalidInput();
            continue;
        }

        double value = 0.0;
        if (parseDoubleStrict(line, value) && value >= minValue) {
            return value;
        }
        printInvalidInput();
    }
}

std::string readNonEmptyLine(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cin.clear();
            printInvalidInput();
            continue;
        }
        line = trim(line);
        if (!line.empty()) {
            return line;
        }
        printInvalidInput();
    }
}

std::string readOptionalLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) {
        std::cin.clear();
        return "";
    }
    return trim(line);
}
}

#pragma once

#include <string>

namespace InputHelper {
    int readInt(const std::string& prompt, int minValue, int maxValue);
    double readDouble(const std::string& prompt, double minValue);
    std::string readNonEmptyLine(const std::string& prompt);
    std::string readOptionalLine(const std::string& prompt);
}

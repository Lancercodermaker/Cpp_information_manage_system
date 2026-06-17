#pragma once

#include <string>
#include <vector>

namespace CsvUtil {
    std::string escapeField(const std::string& field);
    std::vector<std::string> parseLine(const std::string& line);
    std::string joinLine(const std::vector<std::string>& fields);
}

#include "CsvUtil.h"

#include <sstream>

namespace CsvUtil {
std::string escapeField(const std::string& field) {
    bool needsQuotes = false;
    for (char ch : field) {
        if (ch == ',' || ch == '"' || ch == '\n' || ch == '\r') {
            needsQuotes = true;
            break;
        }
    }

    if (!needsQuotes) {
        return field;
    }

    std::string escaped = "\"";
    for (char ch : field) {
        if (ch == '"') {
            escaped += "\"\"";
        } else {
            escaped += ch;
        }
    }
    escaped += '"';
    return escaped;
}

std::vector<std::string> parseLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string current;
    bool inQuotes = false;

    for (std::size_t i = 0; i < line.size(); ++i) {
        const char ch = line[i];
        if (ch == '"') {
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                current += '"';
                ++i;
            } else {
                inQuotes = !inQuotes;
            }
        } else if (ch == ',' && !inQuotes) {
            fields.push_back(current);
            current.clear();
        } else {
            current += ch;
        }
    }

    fields.push_back(current);
    return fields;
}

std::string joinLine(const std::vector<std::string>& fields) {
    std::ostringstream out;
    for (std::size_t i = 0; i < fields.size(); ++i) {
        if (i > 0) {
            out << ',';
        }
        out << escapeField(fields[i]);
    }
    return out.str();
}
}

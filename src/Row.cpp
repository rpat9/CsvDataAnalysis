#include "Row.h"
#include <stdexcept>

const std::string& Row::get(const std::string& col) const {
    auto it = cells.find(col);
    if (it == cells.end()) {
        throw std::out_of_range("Column not found: " + col);
    }
    return it->second;
}

int Row::getAsInt(const std::string& col) const {
    const std::string& val = get(col);
    try {
        return std::stoi(val);
    } catch (...) {
        throw std::invalid_argument(
            "Cannot convert \"" + val + "\" to INT in column: " + col
        );
    }
}

double Row::getAsDouble(const std::string& col) const {
    const std::string& val = get(col);
    try {
        return std::stod(val);
    } catch (...) {
        throw std::invalid_argument(
            "Cannot convert \"" + val + "\" to DOUBLE in column: " + col
        );
    }
}

bool Row::has(const std::string& col) const {
    return cells.count(col) > 0;
}
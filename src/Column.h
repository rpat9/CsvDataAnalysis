#pragma once
#include <string>

struct Column {
    enum class Type { INT, DOUBLE, STRING };

    std::string name;
    int index;      // position in the original CSV (0-based)
    Type type;

    // Convenience: returns a human-readable type label for display
    std::string typeName() const {
        switch (type) {
            case Type::INT:    return "INT";
            case Type::DOUBLE: return "DOUBLE";
            case Type::STRING: return "STRING";
        }
        return "UNKNOWN";
    }
};
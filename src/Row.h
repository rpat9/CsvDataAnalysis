#pragma once
#include <string>
#include <map>

class Row {
public:
    // Raw string storage: column name -> cell value
    // Using std::map keeps columns accessible by name in O(log n)
    std::map<std::string, std::string> cells;

    // Returns the raw string value for a column.
    // Throws std::out_of_range if the column doesn't exist.
    const std::string& get(const std::string& col) const;

    // Typed accessors — convert the stored string on the fly.
    // Throws std::invalid_argument if the value isn't convertible.
    int getAsInt(const std::string& col) const;
    double getAsDouble(const std::string& col) const;

    // Returns true if the column exists in this row
    bool has(const std::string& col) const;
};
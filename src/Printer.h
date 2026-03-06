#pragma once
#include "DataFrame.h"
#include <string>

class Printer {
public:
    // Prints the full DataFrame as an aligned table with a header and
    // separator line. Column widths are calculated dynamically.
    void print(const DataFrame& df) const;

    // Prints a single aggregate result with a label.
    // e.g.  "Average salary: 112450.30"
    void printAggregate(const std::string& func,
                        const std::string& col,
                        double value) const;

    // Prints a row count summary.
    // e.g.  "Showing 23 rows."
    void printCount(int count) const;

    // Prints the column schema: name and inferred type for each column.
    // e.g.  "id (INT)  name (STRING)  salary (DOUBLE)"
    void printSchema(const DataFrame& df) const;

private:
    // Calculates the display width needed for each column by checking
    // both the header name and every cell value in that column.
    std::vector<size_t> computeColumnWidths(const DataFrame& df) const;

    // Prints a horizontal separator line that matches the column widths.
    // e.g.  "+------+---------------+--------+"
    void printSeparator(const std::vector<size_t>& widths) const;

    // Left-pads or right-pads a string to exactly `width` characters.
    std::string padRight(const std::string& s, size_t width) const;
};
#include "Printer.h"
#include <iostream>
#include <iomanip>
#include <sstream>

// ─────────────────────────────────────────────
//  Public Interface
// ─────────────────────────────────────────────

void Printer::print(const DataFrame& df) const {
    const auto& columns = df.getColumns();
    const auto& rows    = df.getRows();

    if (columns.empty()) {
        std::cout << "(no columns)\n";
        return;
    }

    std::vector<size_t> widths = computeColumnWidths(df);

    // Header separator
    printSeparator(widths);

    // Column names
    std::cout << "| ";
    for (size_t i = 0; i < columns.size(); ++i) {
        std::cout << padRight(columns[i].name, widths[i]) << " | ";
    }
    std::cout << "\n";

    // Separator between header and data
    printSeparator(widths);

    // Data rows
    if (rows.empty()) {
        std::cout << "| " << padRight("(no rows)", widths[0]) << " |\n";
    } else {
        for (const Row& row : rows) {
            std::cout << "| ";
            for (size_t i = 0; i < columns.size(); ++i) {
                std::string val = row.has(columns[i].name)
                                  ? row.get(columns[i].name)
                                  : "";
                std::cout << padRight(val, widths[i]) << " | ";
            }
            std::cout << "\n";
        }
    }

    // Bottom border
    printSeparator(widths);
}

void Printer::printAggregate(const std::string& func,
                              const std::string& col,
                              double value) const {
    // Capitalize the function name for display
    std::string label;
    if      (func == "avg") label = "Average";
    else if (func == "sum") label = "Sum";
    else if (func == "min") label = "Minimum";
    else if (func == "max") label = "Maximum";
    else                    label = func;

    // Print with 2 decimal places
    std::cout << std::fixed << std::setprecision(2);
    std::cout << label << " " << col << ": " << value << "\n";
}

void Printer::printCount(int count) const {
    std::cout << count << (count == 1 ? " row" : " rows") << "\n";
}

void Printer::printSchema(const DataFrame& df) const {
    const auto& columns = df.getColumns();
    std::cout << "Columns (" << columns.size() << "): ";
    for (size_t i = 0; i < columns.size(); ++i) {
        std::cout << columns[i].name << " (" << columns[i].typeName() << ")";
        if (i + 1 < columns.size()) std::cout << ",  ";
    }
    std::cout << "\n";
}

// ─────────────────────────────────────────────
//  Private Helpers
// ─────────────────────────────────────────────

std::vector<size_t> Printer::computeColumnWidths(const DataFrame& df) const {
    const auto& columns = df.getColumns();
    const auto& rows    = df.getRows();

    std::vector<size_t> widths(columns.size());

    // Start each column width at the length of its header name
    for (size_t i = 0; i < columns.size(); ++i) {
        widths[i] = columns[i].name.size();
    }

    // Expand width if any cell value in that column is wider
    for (const Row& row : rows) {
        for (size_t i = 0; i < columns.size(); ++i) {
            if (row.has(columns[i].name)) {
                size_t len = row.get(columns[i].name).size();
                if (len > widths[i]) widths[i] = len;
            }
        }
    }

    return widths;
}

void Printer::printSeparator(const std::vector<size_t>& widths) const {
    std::cout << "+";
    for (size_t w : widths) {
        // Each cell is: " " + content + " " + "|"
        // So the dash count is width + 2
        std::cout << std::string(w + 2, '-') << "+";
    }
    std::cout << "\n";
}

std::string Printer::padRight(const std::string& s, size_t width) const {
    if (s.size() >= width) return s;
    return s + std::string(width - s.size(), ' ');
}
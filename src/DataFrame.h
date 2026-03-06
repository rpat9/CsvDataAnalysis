#pragma once
#include "Column.h"
#include "Row.h"
#include <string>
#include <vector>

class DataFrame {
public:
    // Consumes the raw 2D string grid from CSVParser.
    // Row 0 is treated as the header.
    void load(const std::vector<std::vector<std::string>>& raw);

    // Scans every column's values and assigns INT, DOUBLE, or STRING.
    // Must be called after load().
    void inferTypes();

    // --- Query API ---
    // All query methods return a NEW DataFrame — the original is never mutated.

    // Returns rows where column `col` satisfies `op` against `val`.
    // Supported ops: ==  !=  <  >  <=  >=
    DataFrame filter(const std::string& col,
                     const std::string& op,
                     const std::string& val) const;

    // Returns rows sorted by `col`. Respects inferred type for numeric sort.
    DataFrame sort(const std::string& col, bool ascending = true) const;

    // Computes an aggregate over a numeric column.
    // Supported funcs: avg  sum  min  max
    double aggregate(const std::string& func,
                     const std::string& col) const;

    // Number of rows in this DataFrame
    int count() const;

    // --- Accessors (used by Printer and QueryEngine) ---
    const std::vector<Column>& getColumns() const;
    const std::vector<Row>& getRows() const;

    // Returns the Column metadata for a given name.
    // Throws std::invalid_argument if not found.
    const Column& getColumn(const std::string& name) const;

    // Returns true if a column with this name exists
    bool hasColumn(const std::string& name) const;

private:
    std::vector<Column> columns;
    std::vector<Row> rows;

    // Tries to parse every value in a column as INT.
    bool columnIsInt(const std::string& colName) const;

    // Tries to parse every value in a column as DOUBLE.
    bool columnIsDouble(const std::string& colName) const;

    // Compares two cell values using the column's inferred type.
    // Returns negative / zero / positive like strcmp.
    int compareValues(const std::string& a,
                      const std::string& b,
                      Column::Type type) const;
    
};
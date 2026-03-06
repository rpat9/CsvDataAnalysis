#include "DataFrame.h"
#include <stdexcept>
#include <algorithm>
#include <numeric>

// Loading
void DataFrame::load(const std::vector<std::vector<std::string>>& raw){
    if (raw.empty()) {
        throw std::runtime_error("No data to load.");
    }

    columns.clear();
    rows.clear();

    // First row is the header
    const std::vector<std::string>& header = raw[0];
    for (int i = 0; i < static_cast<int>(header.size()); i++) {
        Column col;
        col.name = header[i];
        col.index = i;
        col.type = Column::Type::STRING; // default; overridden by inferTypes()
        columns.push_back(col);
    }

    // Remaining rows are data
    for (size_t r = 1; r < raw.size(); r++) {
        Row row;
        for (size_t c = 0; c < columns.size(); c++) {
            std::string val = (c < raw[r].size()) ? raw[r][c] : "";
            row.cells[columns[c].name] = val;
        }
        rows.push_back(row);
    }
}

bool DataFrame::columnIsInt(const std::string & colName) const {
    for (const Row& row : rows) {
        const std::string& val = row.get(colName);
        if (val.empty()){
            continue;
        }

        try {
            std::stoi(val);
        } catch (...) {
            return false;
        }
    }

    return true;
}

bool DataFrame::columnIsDouble(const std::string& colName) const {
    for (const Row& row : rows) {
        const std::string& val = row.get(colName);
        if (val.empty()){
            continue;
        }

        try {
            std::stod(val);
        } catch (...) {
            return false;
        }
    }

    return true;
}

void DataFrame::inferTypes() {
    for (Column& col : columns) {
        if (columnIsInt(col.name)) {
            col.type = Column::Type::INT;
        } else if (columnIsDouble(col.name)) {
            col.type = Column::Type::DOUBLE;
        } else {
            col.type = Column::Type::STRING;
        }
    }
}

int DataFrame::compareValues(const std::string& a, const std::string& b, Column::Type type) const {
    if (type == Column::Type::INT) {
        int ia = std::stoi(a);
        int ib = std::stoi(b);
        return (ia < ib) ? -1 : (ia > ib) ? 1 : 0;
    }
    if (type == Column::Type::DOUBLE) {
        double da = std::stod(a);
        double db = std::stod(b);
        return (da < db) ? -1 : (da > db) ? 1 : 0;
    }

    return a.compare(b);
}

DataFrame DataFrame::filter(const std::string& col,
                             const std::string& op,
                             const std::string& val) const {
    if (!hasColumn(col)) {
        throw std::invalid_argument("Unknown column: " + col);
    }

    const Column& colMeta = getColumn(col);
    DataFrame result;
    result.columns = columns; // same schema

    for (const Row& row : rows) {
        int cmp = compareValues(row.get(col), val, colMeta.type);

        bool match = false;
        if      (op == "==") match = (cmp == 0);
        else if (op == "!=") match = (cmp != 0);
        else if (op == "<")  match = (cmp <  0);
        else if (op == ">")  match = (cmp >  0);
        else if (op == "<=") match = (cmp <= 0);
        else if (op == ">=") match = (cmp >= 0);
        else throw std::invalid_argument("Unknown operator: " + op);

        if (match) result.rows.push_back(row);
    }

    return result;
}

// ─────────────────────────────────────────────
//  Sort
// ─────────────────────────────────────────────

DataFrame DataFrame::sort(const std::string& col, bool ascending) const {
    if (!hasColumn(col)) {
        throw std::invalid_argument("Unknown column: " + col);
    }

    const Column& colMeta = getColumn(col);
    DataFrame result;
    result.columns = columns;
    result.rows    = rows; // copy all rows, then sort the copy

    std::sort(result.rows.begin(), result.rows.end(),
        [&](const Row& a, const Row& b) {
            int cmp = compareValues(a.get(col), b.get(col), colMeta.type);
            return ascending ? (cmp < 0) : (cmp > 0);
        }
    );

    return result;
}

// ─────────────────────────────────────────────
//  Aggregates
// ─────────────────────────────────────────────

double DataFrame::aggregate(const std::string& func,
                             const std::string& col) const {
    if (!hasColumn(col)) {
        throw std::invalid_argument("Unknown column: " + col);
    }

    const Column& colMeta = getColumn(col);
    if (colMeta.type == Column::Type::STRING) {
        throw std::invalid_argument("Cannot aggregate a STRING column: " + col);
    }

    if (rows.empty()) {
        throw std::runtime_error("No rows to aggregate.");
    }

    // Collect numeric values
    std::vector<double> values;
    values.reserve(rows.size());
    for (const Row& row : rows) {
        values.push_back(row.getAsDouble(col));
    }

    if (func == "sum") {
        return std::accumulate(values.begin(), values.end(), 0.0);
    }
    if (func == "avg") {
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        return sum / static_cast<double>(values.size());
    }
    if (func == "min") {
        return *std::min_element(values.begin(), values.end());
    }
    if (func == "max") {
        return *std::max_element(values.begin(), values.end());
    }

    throw std::invalid_argument("Unknown aggregate function: " + func);
}

// ─────────────────────────────────────────────
//  Accessors
// ─────────────────────────────────────────────

int DataFrame::count() const {
    return static_cast<int>(rows.size());
}

const std::vector<Column>& DataFrame::getColumns() const { return columns; }
const std::vector<Row>&    DataFrame::getRows()    const { return rows; }

const Column& DataFrame::getColumn(const std::string& name) const {
    for (const Column& col : columns) {
        if (col.name == name) return col;
    }
    throw std::invalid_argument("Column not found: " + name);
}

bool DataFrame::hasColumn(const std::string& name) const {
    for (const Column& col : columns) {
        if (col.name == name) return true;
    }
    return false;
}
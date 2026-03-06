#include "CSVParser.h"
#include "DataFrame.h"
#include "QueryEngine.h"
#include "Printer.h"
#include <iostream>

int main(int argc, char* argv[]) {
    // ── 1. Validate arguments ─────────────────────────────────────────
    if (argc < 2) {
        std::cerr << "Usage: csvtool <path/to/file.csv>\n";
        std::cerr << "Example: csvtool data/employees.csv\n";
        return 1;
    }

    const std::string filepath = argv[1];

    // ── 2. Parse the CSV file ─────────────────────────────────────────
    CSVParser parser;
    std::vector<std::vector<std::string>> raw;

    try {
        raw = parser.parse(filepath);
    } catch (const std::exception& e) {
        std::cerr << "[Fatal] " << e.what() << "\n";
        return 1;
    }

    // ── 3. Load into DataFrame and infer column types ─────────────────
    DataFrame df;

    try {
        df.load(raw);
        df.inferTypes();
    } catch (const std::exception& e) {
        std::cerr << "[Fatal] " << e.what() << "\n";
        return 1;
    }

    // ── 4. Print a welcome summary ────────────────────────────────────
    Printer printer;
    std::cout << "\nLoaded: " << filepath << "\n";
    std::cout << df.count() << " rows | ";
    printer.printSchema(df);

    // ── 5. Hand off to the interactive REPL ───────────────────────────
    QueryEngine engine(df);
    engine.run();

    std::cout << "Goodbye.\n";
    return 0;
}
#include "QueryEngine.h"
#include <iostream>
#include <sstream>
#include <algorithm>

// ─────────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────────

QueryEngine::QueryEngine(const DataFrame& base)
    : base(base), view(base) {}

// ─────────────────────────────────────────────
//  REPL Loop
// ─────────────────────────────────────────────

void QueryEngine::run() {
    std::cout << "\nType 'help' for available commands, 'exit' to quit.\n\n";

    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break; // handle EOF (Ctrl+D / Ctrl+Z)

        // Strip leading/trailing whitespace
        size_t start = line.find_first_not_of(" \t");
        size_t end   = line.find_last_not_of(" \t");
        if (start == std::string::npos) continue; // blank line
        line = line.substr(start, end - start + 1);

        std::vector<std::string> tokens = tokenize(line);
        if (tokens.empty()) continue;

        bool keepRunning = dispatch(tokens);
        if (!keepRunning) break;

        std::cout << "\n";
    }
}

// ─────────────────────────────────────────────
//  Tokenizer
// ─────────────────────────────────────────────

std::vector<std::string> QueryEngine::tokenize(const std::string& line) const {
    std::vector<std::string> tokens;
    std::istringstream stream(line);
    std::string token;
    while (stream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// ─────────────────────────────────────────────
//  Dispatcher
// ─────────────────────────────────────────────

bool QueryEngine::dispatch(const std::vector<std::string>& tokens) {
    const std::string& cmd = tokens[0];

    // Normalize command to lowercase so "FILTER" == "filter"
    std::string cmdLower = cmd;
    std::transform(cmdLower.begin(), cmdLower.end(), cmdLower.begin(), ::tolower);

    try {
        if      (cmdLower == "exit")                        return false;
        else if (cmdLower == "help")                        handleHelp();
        else if (cmdLower == "show")                        handleShow();
        else if (cmdLower == "schema")                      handleSchema();
        else if (cmdLower == "count")                       handleCount();
        else if (cmdLower == "reset")                       handleReset();
        else if (cmdLower == "filter")                      handleFilter(tokens);
        else if (cmdLower == "sort")                        handleSort(tokens);
        else if (cmdLower == "avg" || cmdLower == "sum"  ||
                 cmdLower == "min" || cmdLower == "max")    handleAggregate(cmdLower, tokens);
        else {
            printError("Unknown command: '" + cmd + "'. Type 'help' to see available commands.");
        }
    } catch (const std::exception& e) {
        printError(e.what());
    }

    return true;
}

// ─────────────────────────────────────────────
//  Command Handlers
// ─────────────────────────────────────────────

void QueryEngine::handleFilter(const std::vector<std::string>& tokens) {
    // Expected format: filter <col> <op> <val>
    // Note: <val> may contain spaces if the user quotes it — we join
    // everything after token[2] to support values like "New York"
    if (tokens.size() < 4) {
        printError("Usage: filter <column> <op> <value>   e.g. filter age > 25");
        return;
    }

    const std::string& col = tokens[1];
    const std::string& op  = tokens[2];

    // Join remaining tokens as the value (handles multi-word strings)
    std::string val;
    for (size_t i = 3; i < tokens.size(); ++i) {
        if (i > 3) val += " ";
        val += tokens[i];
    }

    view = view.filter(col, op, val);
    std::cout << "Filtered to " << view.count() << " row(s). Use 'show' to display.\n";
}

void QueryEngine::handleSort(const std::vector<std::string>& tokens) {
    // Expected format: sort <col> [asc|desc]
    if (tokens.size() < 2) {
        printError("Usage: sort <column> [asc|desc]   e.g. sort salary desc");
        return;
    }

    const std::string& col = tokens[1];
    bool ascending = true;

    if (tokens.size() >= 3) {
        std::string dir = tokens[2];
        std::transform(dir.begin(), dir.end(), dir.begin(), ::tolower);
        if (dir == "desc") ascending = false;
        else if (dir != "asc") {
            printError("Sort direction must be 'asc' or 'desc'.");
            return;
        }
    }

    view = view.sort(col, ascending);
    std::cout << "Sorted by " << col
              << " (" << (ascending ? "ascending" : "descending") << ")."
              << " Use 'show' to display.\n";
}

void QueryEngine::handleAggregate(const std::string& func,
                                   const std::vector<std::string>& tokens) {
    // Expected format: avg|sum|min|max <col>
    if (tokens.size() < 2) {
        printError("Usage: " + func + " <column>   e.g. avg salary");
        return;
    }

    const std::string& col = tokens[1];
    double result = view.aggregate(func, col);
    printer.printAggregate(func, col, result);
}

void QueryEngine::handleCount() {
    printer.printCount(view.count());
}

void QueryEngine::handleShow() {
    printer.print(view);
    std::cout << view.count() << " row(s).\n";
}

void QueryEngine::handleSchema() {
    printer.printSchema(view);
}

void QueryEngine::handleReset() {
    view = base;
    std::cout << "View reset to full dataset (" << view.count() << " rows).\n";
}

// Menu with Claude
void QueryEngine::handleHelp() const {
    std::cout
        << "┌─────────────────────────────────────────────────────────┐\n"
        << "│                   Available Commands                    │\n"
        << "├─────────────────────────────────────────────────────────┤\n"
        << "│  show                    Print the current view         │\n"
        << "│  schema                  Show column names and types    │\n"
        << "│  count                   Print the row count            │\n"
        << "│  filter <col> <op> <val> Filter rows  (==,!=,<,>,<=,>=) │\n"
        << "│  sort <col> [asc|desc]   Sort rows by a column          │\n"
        << "│  avg <col>               Average of a numeric column    │\n"
        << "│  sum <col>               Sum of a numeric column        │\n"
        << "│  min <col>               Minimum of a numeric column    │\n"
        << "│  max <col>               Maximum of a numeric column    │\n"
        << "│  reset                   Restore the full dataset       │\n"
        << "│  help                    Show this message              │\n"
        << "│  exit                    Quit the program               │\n"
        << "└─────────────────────────────────────────────────────────┘\n";
}

// ─────────────────────────────────────────────
//  Error Display
// ─────────────────────────────────────────────

void QueryEngine::printError(const std::string& message) const {
    std::cerr << "[Error] " << message << "\n";
}
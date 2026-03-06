#pragma once
#include "DataFrame.h"
#include "Printer.h"
#include <string>
#include <vector>

class QueryEngine {
public:
    // Takes ownership of the fully loaded and type-inferred DataFrame.
    // This becomes the immutable "base" — the original data, never modified.
    explicit QueryEngine(const DataFrame& base);

    // Starts the interactive REPL loop.
    // Reads from stdin until the user types "exit".
    void run();

private:
    const DataFrame& base;     // original, untouched data
    DataFrame        view;     // current working view (filtered/sorted)
    Printer          printer;

    // Splits a raw input line into whitespace-delimited tokens.
    std::vector<std::string> tokenize(const std::string& line) const;

    // Dispatches a tokenized command to the correct handler.
    // Returns false if the command is "exit", true otherwise.
    bool dispatch(const std::vector<std::string>& tokens);

    // ── Command Handlers ──────────────────────────────────────────────

    // filter <col> <op> <val>
    // e.g.  filter age > 25
    //       filter department == Engineering
    void handleFilter(const std::vector<std::string>& tokens);

    // sort <col> [asc|desc]       (default: asc)
    // e.g.  sort salary desc
    void handleSort(const std::vector<std::string>& tokens);

    // avg|sum|min|max <col>
    // e.g.  avg salary
    void handleAggregate(const std::string& func,
                         const std::vector<std::string>& tokens);

    // count  — prints number of rows in current view
    void handleCount();

    // show   — prints the current view as a table
    void handleShow();

    // schema — prints column names and inferred types
    void handleSchema();

    // reset  — restores the view to the full base dataset
    void handleReset();

    // help   — prints available commands
    void handleHelp() const;

    // Prints a formatted error message without crashing the REPL
    void printError(const std::string& message) const;
};
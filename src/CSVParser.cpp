#include "CSVParser.h"
#include <fstream>
#include <stdexcept>
#include <sstream>

std::vector<std::string> CSVParser::parseLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string current;
    bool insideQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];

        if (c == '"') {
            // Two consecutive double-quotes inside a quoted field
            // represent a literal quote character: "" -> "
            if (insideQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                current += '"';
                ++i; // skip the second quote
            } else {
                insideQuotes = !insideQuotes;
            }
        } else if (c == ',' && !insideQuotes) {
            fields.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }

    // Push the last field (there is no trailing comma to trigger it)
    fields.push_back(current);
    return fields;
}

void CSVParser::stripCarriageReturn(std::string& s) {
    if (!s.empty() && s.back() == '\r') {
        s.pop_back();
    }
}

std::vector<std::vector<std::string>> CSVParser::parse(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filepath);
    }

    std::vector<std::vector<std::string>> result;
    std::string line;

    while (std::getline(file, line)) {
        stripCarriageReturn(line);

        // Skip completely empty lines (e.g. a trailing newline at end of file)
        if (line.empty()) continue;

        result.push_back(parseLine(line));
    }

    if (result.empty()) {
        throw std::runtime_error("File is empty: " + filepath);
    }

    return result;
    
}
#pragma once
#include <string>
#include <vector>

class CSVParser {

    /**
     * Parses the file at the given path
     * Returns a 2D grid: result[0] is the header row
     * result [1...n] are the data rows
     * Throws std::runtime_error if the file cannot be opened
     */
    public:
        std::vector<std::vector<std::string>> parse(const std::string& filepath);
    
    /**
     * Splits a single CSV line into fields
     * Correctly handles quoted fields that contain commas
     * Example: "New York, NY" is one field not two
     */
    std::vector<std::string> parseLine(const std::string& line);

    // Strips a leading/trailing \r to handle Windows line endings (CRLF).
    void stripCarriageReturn(std::string& s);
        
};
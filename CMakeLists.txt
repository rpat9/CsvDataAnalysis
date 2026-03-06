cmake_minimum_required(VERSION 3.15)
project(CsvDataAnalysis)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(csvtool
    src/main.cpp
    src/CSVParser.cpp
    src/Row.cpp
    src/DataFrame.cpp
    src/QueryEngine.cpp
    src/Printer.cpp
)

target_include_directories(csvtool PRIVATE src)
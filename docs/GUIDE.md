# libcsvkit User Guide

Comprehensive guide to using libcsvkit for CSV parsing and writing.

## Table of Contents

- [Introduction](#introduction)
- [Getting Started](#getting-started)
- [Basic Concepts](#basic-concepts)
- [Reading CSV Files](#reading-csv-files)
- [Writing CSV Files](#writing-csv-files)
- [Configuration](#configuration)
- [Error Handling](#error-handling)
- [Advanced Usage](#advanced-usage)
- [Best Practices](#best-practices)
- [Common Patterns](#common-patterns)

## Introduction

libcsvkit is a lightweight, fast CSV parsing and writing library for C with modern C++ bindings. It provides:

- **RFC 4180 compliant** CSV parsing
- **Flexible configuration** for various CSV dialects
- **Streaming API** for memory-efficient processing
- **Modern C++ bindings** with RAII and exceptions
- **Cross-platform** support

### When to Use libcsvkit

Use libcsvkit when you need to:

- Parse or generate CSV files in C/C++ programs
- Handle large CSV files efficiently (streaming)
- Work with different CSV dialects (semicolon-separated, tab-separated, etc.)
- Process CSV data with strict RFC 4180 compliance
- Write portable, cross-platform CSV handling code

## Getting Started

### Installation

See [BUILDING.md](BUILDING.md) for installation instructions.

### Your First Program (C)

```c
#include <csvkit.h>
#include <stdio.h>

int main() {
    csvkit_parser_t *parser = csvkit_parser_new();
    csvkit_open_file(parser, "data.csv");

    csvkit_row_t *row;
    while (csvkit_read_row(parser, &row) == CSVKIT_OK) {
        for (size_t i = 0; i < row->field_count; i++) {
            printf("%s ", row->fields[i]);
        }
        printf("\n");
        csvkit_row_free(row);
    }

    csvkit_parser_free(parser);
    return 0;
}
```

Compile:

```bash
gcc example.c -o example -lcsvkit
./example
```

### Your First Program (C++)

```cpp
#include <csvkit.hpp>
#include <iostream>

using namespace csvkit;

int main() {
    try {
        Parser parser;
        parser.open("data.csv");

        for (auto& row : parser) {
            for (const auto& field : row) {
                std::cout << field << " ";
            }
            std::cout << "\n";
        }
    } catch (const Exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
```

Compile:

```bash
g++ -std=c++11 example.cpp -o example -lcsvkit++
./example
```

## Basic Concepts

### CSV Format

CSV (Comma-Separated Values) is a text format for tabular data:

```csv
Name,Age,City
"Doe, John",30,"New York"
Jane Smith,25,LA
```

#### Rules

1. Fields separated by delimiter (usually comma)
2. Records (rows) separated by newlines
3. Fields containing delimiter, quotes, or newlines must be quoted
4. Quotes inside quoted fields are escaped by doubling them

### libcsvkit Components

#### Parser

Reads CSV data from:
- Files (by path)
- Streams (`FILE*`)
- Strings (in-memory)

#### Writer

Writes CSV data to:
- Files (by path)
- Streams (`FILE*`)

#### Configuration

Customizes parsing/writing behavior:
- Delimiter character
- Quote character
- Escape character
- Whitespace trimming
- Empty row handling
- Strict mode compliance

## Reading CSV Files

### From File (C)

```c
csvkit_parser_t *parser = csvkit_parser_new();

if (csvkit_open_file(parser, "data.csv") != CSVKIT_OK) {
    fprintf(stderr, "Error: %s\n", csvkit_get_error_msg(parser));
    csvkit_parser_free(parser);
    return 1;
}

csvkit_row_t *row;
csvkit_error_t err;

while ((err = csvkit_read_row(parser, &row)) == CSVKIT_OK) {
    printf("Row %zu has %zu fields\n", row->row_number, row->field_count);

    // Access fields
    for (size_t i = 0; i < row->field_count; i++) {
        printf("  Field %zu: %s\n", i, row->fields[i]);
    }

    csvkit_row_free(row);
}

if (err != CSVKIT_ERROR_EOF) {
    fprintf(stderr, "Parse error: %s\n", csvkit_get_error_msg(parser));
}

csvkit_close(parser);
csvkit_parser_free(parser);
```

### From File (C++)

```cpp
Parser parser;
parser.open("data.csv");

while (auto row = parser.read_row()) {
    std::cout << "Row " << row->row_number() << "\n";

    // Access fields by index
    if (row->size() >= 3) {
        std::cout << "  Name: " << (*row)[0] << "\n";
        std::cout << "  Age: " << (*row)[1] << "\n";
        std::cout << "  City: " << (*row)[2] << "\n";
    }
}
```

### From String (C)

```c
const char *csv_data = "A,B,C\n1,2,3\n4,5,6";

csvkit_parser_t *parser = csvkit_parser_new();
csvkit_open_string(parser, csv_data, strlen(csv_data));

csvkit_row_t *row;
while (csvkit_read_row(parser, &row) == CSVKIT_OK) {
    // Process row
    csvkit_row_free(row);
}

csvkit_parser_free(parser);
```

### From String (C++)

```cpp
std::string csv = "A,B,C\n1,2,3\n4,5,6";

Parser parser;
parser.open_string(csv);

auto rows = parser.read_all();  // Read all at once
std::cout << "Read " << rows.size() << " rows\n";
```

### Range-Based Iteration (C++)

```cpp
Parser parser;
parser.open("data.csv");

// Most idiomatic way
for (auto& row : parser) {
    for (const auto& field : row) {
        std::cout << field << " ";
    }
    std::cout << "\n";
}
```

## Writing CSV Files

### To File (C)

```c
csvkit_writer_t *writer = csvkit_writer_new();

if (csvkit_writer_open_file(writer, "output.csv") != CSVKIT_OK) {
    fprintf(stderr, "Error: %s\n", csvkit_writer_get_error_msg(writer));
    csvkit_writer_free(writer);
    return 1;
}

// Write header
const char *header[] = {"Name", "Age", "City"};
csvkit_writer_write_row(writer, header, 3);

// Write data rows
const char *row1[] = {"John Doe", "30", "New York"};
csvkit_writer_write_row(writer, row1, 3);

const char *row2[] = {"Jane Smith", "25", "Los Angeles"};
csvkit_writer_write_row(writer, row2, 3);

csvkit_writer_close(writer);
csvkit_writer_free(writer);
```

### To File (C++)

```cpp
Writer writer;
writer.open("output.csv");

// Write rows
writer.write_row({"Name", "Age", "City"});
writer.write_row({"John Doe", "30", "New York"});
writer.write_row({"Jane Smith", "25", "Los Angeles"});

writer.close();  // Optional, destructor closes automatically
```

### To Stream (C)

```c
csvkit_writer_t *writer = csvkit_writer_new();
csvkit_writer_open_stream(writer, stdout);

const char *row[] = {"A", "B", "C"};
csvkit_writer_write_row(writer, row, 3);

csvkit_writer_free(writer);
```

### To Stream (C++)

```cpp
Writer writer;
writer.open(stdout);

writer.write_row({"A", "B", "C"});
```

## Configuration

### Default Configuration

Default settings:
- Delimiter: `,`
- Quote character: `"`
- Escape character: `"`
- Trim whitespace: `false`
- Skip empty rows: `false`
- Strict mode: `false`

### Custom Configuration (C)

```c
csvkit_config_t config = csvkit_config_default();

// Semicolon-separated
config.delimiter = ';';

// Trim whitespace
config.trim_whitespace = true;

// Skip empty rows
config.skip_empty_rows = true;

// Enable strict RFC 4180 mode
config.strict_mode = true;

csvkit_parser_t *parser = csvkit_parser_new_with_config(&config);
```

### Custom Configuration (C++)

```cpp
Config config;
config.delimiter(';')
      .quote_char('\'')
      .trim_whitespace(true)
      .skip_empty_rows(true)
      .strict_mode(true);

Parser parser(config);
```

### Common Configurations

#### Tab-Separated Values (TSV)

```cpp
Config config;
config.delimiter('\t');

Parser parser(config);
```

#### Semicolon-Separated (European CSV)

```cpp
Config config;
config.delimiter(';');

Parser parser(config);
```

#### Pipe-Separated

```cpp
Config config;
config.delimiter('|');

Parser parser(config);
```

#### Excel-Style (with trimming)

```cpp
Config config;
config.trim_whitespace(true)
      .skip_empty_rows(true);

Parser parser(config);
```

## Error Handling

### C Error Handling

Always check return values:

```c
csvkit_parser_t *parser = csvkit_parser_new();
if (!parser) {
    fprintf(stderr, "Failed to create parser\n");
    return 1;
}

csvkit_error_t err = csvkit_open_file(parser, "data.csv");
if (err != CSVKIT_OK) {
    const char *msg = csvkit_get_error_msg(parser);
    fprintf(stderr, "Error opening file: %s\n",
            msg ? msg : "Unknown error");
    csvkit_parser_free(parser);
    return 1;
}

csvkit_row_t *row;
while ((err = csvkit_read_row(parser, &row)) == CSVKIT_OK) {
    // Process row
    csvkit_row_free(row);
}

// Check if error or EOF
if (err != CSVKIT_ERROR_EOF) {
    fprintf(stderr, "Parse error: %s\n", csvkit_get_error_msg(parser));
}

csvkit_parser_free(parser);
```

### C++ Exception Handling

Use try-catch blocks:

```cpp
try {
    Parser parser;
    parser.open("data.csv");

    while (auto row = parser.read_row()) {
        try {
            // Safe access with bounds checking
            std::string name = row->at(0);
            std::string age = row->at(1);
        } catch (const std::out_of_range& e) {
            std::cerr << "Missing field in row "
                      << row->row_number() << "\n";
            continue;
        }
    }
} catch (const csvkit::Exception& e) {
    std::cerr << "CSV error: " << e.what() << "\n";
    return 1;
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
}
```

## Advanced Usage

### Memory-Efficient Processing (Streaming)

Process large files without loading everything into memory:

```cpp
Parser parser;
parser.open("huge_file.csv");

size_t total = 0;
while (auto row = parser.read_row()) {
    total += row->size();
    // Row is freed when unique_ptr goes out of scope
}

std::cout << "Total fields: " << total << "\n";
```

### Filtering Rows

```cpp
Parser parser;
parser.open("data.csv");

Writer writer;
writer.open("filtered.csv");

bool is_header = true;
for (auto& row : parser) {
    if (is_header) {
        writer.write_row(row.fields());
        is_header = false;
        continue;
    }

    // Filter: only rows where age > 25
    if (row.size() >= 2 && std::stoi(row[1]) > 25) {
        writer.write_row(row.fields());
    }
}
```

### Transforming Data

```cpp
Parser parser;
parser.open("input.csv");

Writer writer;
writer.open("output.csv");

for (auto& row : parser) {
    std::vector<std::string> new_row;

    for (const auto& field : row) {
        // Transform: convert to uppercase
        std::string upper = field;
        std::transform(upper.begin(), upper.end(),
                      upper.begin(), ::toupper);
        new_row.push_back(upper);
    }

    writer.write_row(new_row);
}
```

### Aggregation

```cpp
Parser parser;
parser.open("sales.csv");

double total = 0.0;
size_t count = 0;

bool skip_header = true;
for (auto& row : parser) {
    if (skip_header) {
        skip_header = false;
        continue;
    }

    if (row.size() >= 3) {
        total += std::stod(row[2]);  // Sum third column
        count++;
    }
}

std::cout << "Average: " << (total / count) << "\n";
```

### Joining CSV Files

```cpp
// Read first file into map
Parser parser1;
parser1.open("users.csv");

std::map<std::string, std::vector<std::string>> users;
bool skip_header = true;
for (auto& row : parser1) {
    if (skip_header) {
        skip_header = false;
        continue;
    }
    if (row.size() > 0) {
        users[row[0]] = row.fields();  // Key by ID
    }
}

// Read second file and join
Parser parser2;
parser2.open("orders.csv");

Writer writer;
writer.open("joined.csv");

skip_header = true;
for (auto& row : parser2) {
    if (skip_header) {
        skip_header = false;
        writer.write_row(row.fields());
        continue;
    }

    if (row.size() > 0) {
        std::string user_id = row[0];
        if (users.count(user_id)) {
            auto combined = row.fields();
            auto& user = users[user_id];
            combined.insert(combined.end(), user.begin(), user.end());
            writer.write_row(combined);
        }
    }
}
```

## Best Practices

### Always Free Resources (C)

```c
csvkit_parser_t *parser = csvkit_parser_new();
if (!parser) return 1;

// ... use parser ...

csvkit_parser_free(parser);  // Always free
```

### Use RAII (C++)

```cpp
{
    Parser parser;  // Automatically freed when scope ends
    parser.open("data.csv");
    // ... use parser ...
}  // Parser freed here
```

### Check All Return Values (C)

```c
csvkit_error_t err = csvkit_open_file(parser, "data.csv");
if (err != CSVKIT_OK) {
    // Handle error
}
```

### Prefer Range-Based Loops (C++)

```cpp
// Good
for (auto& row : parser) {
    // Process row
}

// Less idiomatic
while (auto row = parser.read_row()) {
    // Process row
}
```

### Use `at()` for Safe Access (C++)

```cpp
// Safe - throws on invalid index
std::string name = row.at(0);

// Fast but unsafe - undefined behavior if invalid
std::string name = row[0];
```

### Close Writers Explicitly

```cpp
Writer writer;
writer.open("output.csv");
writer.write_row({"A", "B"});
writer.close();  // Flush data before process ends
```

### Handle Malformed CSV Gracefully

```cpp
try {
    Parser parser;
    parser.open("data.csv");

    for (auto& row : parser) {
        if (row.empty()) continue;  // Skip empty rows

        if (row.size() < 3) {
            std::cerr << "Warning: Row " << row.row_number()
                      << " has only " << row.size() << " fields\n";
            continue;
        }

        // Process valid row
    }
} catch (const csvkit::Exception& e) {
    std::cerr << "Parse error: " << e.what() << "\n";
}
```

## Common Patterns

### Read Header Separately

```cpp
Parser parser;
parser.open("data.csv");

// Read header
auto header_row = parser.read_row();
if (!header_row) {
    std::cerr << "File is empty\n";
    return 1;
}

std::vector<std::string> headers = header_row->fields();

// Process data rows
while (auto row = parser.read_row()) {
    // Use headers[i] and row[i]
}
```

### Convert CSV to JSON

```cpp
Parser parser;
parser.open("data.csv");

auto header = parser.read_row();
if (!header) return 1;

std::cout << "[\n";
bool first = true;

while (auto row = parser.read_row()) {
    if (!first) std::cout << ",\n";
    first = false;

    std::cout << "  {";
    for (size_t i = 0; i < row->size(); i++) {
        if (i > 0) std::cout << ", ";
        std::cout << "\"" << (*header)[i] << "\": "
                  << "\"" << (*row)[i] << "\"";
    }
    std::cout << "}";
}

std::cout << "\n]\n";
```

### Validate CSV Schema

```cpp
Parser parser;
parser.open("data.csv");

const std::vector<std::string> expected_headers = {"Name", "Age", "Email"};

auto header = parser.read_row();
if (!header || header->fields() != expected_headers) {
    std::cerr << "Invalid CSV schema\n";
    return 1;
}

size_t errors = 0;
while (auto row = parser.read_row()) {
    if (row->size() != expected_headers.size()) {
        std::cerr << "Row " << row->row_number()
                  << ": expected " << expected_headers.size()
                  << " fields, got " << row->size() << "\n";
        errors++;
    }
}

std::cout << "Validation complete. Errors: " << errors << "\n";
```

### Progress Reporting

```cpp
Parser parser;
parser.open("large_file.csv");

size_t rows_processed = 0;
const size_t report_interval = 10000;

while (auto row = parser.read_row()) {
    // Process row
    rows_processed++;

    if (rows_processed % report_interval == 0) {
        std::cout << "Processed " << rows_processed << " rows...\n";
    }
}

std::cout << "Total rows: " << rows_processed << "\n";
```

## See Also

- [C API Reference](API.md)
- [C++ API Reference](CPP_API.md)
- [Building Guide](BUILDING.md)
- [Examples Directory](../examples/)

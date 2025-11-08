# libcsvkit C++ API Reference

Complete C++ API documentation for libcsvkit++ bindings.

## Table of Contents

- [Overview](#overview)
- [Classes](#classes)
  - [Config](#config)
  - [Row](#row)
  - [Parser](#parser)
  - [Writer](#writer)
  - [Exception](#exception)
- [Helper Functions](#helper-functions)
- [Examples](#examples)

## Overview

The C++ bindings provide a modern, type-safe interface to libcsvkit with:

- **RAII**: Automatic resource management
- **Exceptions**: Error handling through C++ exceptions
- **STL Integration**: Works seamlessly with `std::vector`, `std::string`
- **Move Semantics**: Efficient ownership transfer (C++11)
- **Range-based loops**: Iterator support for `for (auto& row : parser)`
- **Fluent API**: Method chaining for configuration

**Namespace:** `csvkit`

**Header:** `<csvkit.hpp>`

**Link:** `-lcsvkit++` (also requires `-lcsvkit`)

## Classes

### Config

Fluent configuration builder for CSV parsing and writing.

```cpp
class Config {
public:
    Config();

    Config& delimiter(char delim);
    Config& quote_char(char quote);
    Config& escape_char(char escape);
    Config& trim_whitespace(bool trim);
    Config& skip_empty_rows(bool skip);
    Config& strict_mode(bool strict);

    const csvkit_config_t& get() const;
};
```

#### Methods

##### `Config()`

Creates a configuration with default settings:
- Delimiter: `,`
- Quote character: `"`
- Escape character: `"`
- Trim whitespace: `false`
- Skip empty rows: `false`
- Strict mode: `false`

##### `delimiter(char delim)`

Sets the field delimiter character.

**Parameters:**
- `delim`: Delimiter character (e.g., `,`, `;`, `\t`)

**Returns:** Reference to `this` for chaining.

##### `quote_char(char quote)`

Sets the quote character.

**Parameters:**
- `quote`: Quote character (e.g., `"`, `'`)

**Returns:** Reference to `this` for chaining.

##### `escape_char(char escape)`

Sets the escape character.

**Parameters:**
- `escape`: Escape character

**Returns:** Reference to `this` for chaining.

##### `trim_whitespace(bool trim)`

Enables/disables whitespace trimming.

**Parameters:**
- `trim`: `true` to trim whitespace, `false` otherwise

**Returns:** Reference to `this` for chaining.

##### `skip_empty_rows(bool skip)`

Enables/disables skipping empty rows.

**Parameters:**
- `skip`: `true` to skip empty rows, `false` otherwise

**Returns:** Reference to `this` for chaining.

##### `strict_mode(bool strict)`

Enables/disables strict RFC 4180 compliance.

**Parameters:**
- `strict`: `true` for strict mode, `false` otherwise

**Returns:** Reference to `this` for chaining.

#### Example

```cpp
Config config;
config.delimiter(';')
      .quote_char('\'')
      .trim_whitespace(true)
      .skip_empty_rows(true);
```

---

### Row

Represents a single CSV row with read-only access to fields.

```cpp
class Row {
public:
    // Access fields
    const std::string& operator[](size_t index) const;
    const std::string& at(size_t index) const;

    // Metadata
    size_t size() const;
    size_t field_count() const;
    size_t row_number() const;
    bool empty() const;

    // Iterators
    std::vector<std::string>::const_iterator begin() const;
    std::vector<std::string>::const_iterator end() const;

    // Get all fields
    const std::vector<std::string>& fields() const;

    // Move-only
    Row(Row&& other) noexcept;
    Row& operator=(Row&& other) noexcept;
};
```

#### Methods

##### `operator[](size_t index)`

Accesses field by index without bounds checking (fast).

**Parameters:**
- `index`: Field index (0-based)

**Returns:** Field value as `const std::string&`.

**Note:** No bounds checking. Undefined behavior if `index >= size()`. Use `at()` for safe access.

**Example:**

```cpp
std::string name = row[0];  // Fast, no bounds check
```

##### `at(size_t index)`

Accesses field by index with bounds checking (safe).

**Parameters:**
- `index`: Field index (0-based)

**Returns:** Field value as `const std::string&`.

**Throws:** `std::out_of_range` if index is out of bounds.

**Example:**

```cpp
try {
    std::string name = row.at(0);  // Safe, throws on invalid index
} catch (const std::out_of_range& e) {
    std::cerr << "Invalid index\n";
}
```

##### `size()` / `field_count()`

Returns the number of fields in the row.

**Returns:** Number of fields.

##### `row_number()`

Returns the row number in the source file (1-based).

**Returns:** Row number.

##### `empty()`

Checks if the row is empty (has no fields).

**Returns:** `true` if empty, `false` otherwise.

##### `begin()` / `end()`

Returns iterators for range-based loops.

**Returns:** Const iterator to fields.

**Example:**

```cpp
for (const auto& field : row) {
    std::cout << field << " ";
}
```

##### `fields()`

Returns all fields as a vector.

**Returns:** `const std::vector<std::string>&` containing all fields.

---

### Parser

CSV parser with RAII and iterator support.

```cpp
class Parser {
public:
    Parser();
    explicit Parser(const Config& config);
    ~Parser();

    // Open sources
    void open(const std::string& filename);
    void open(FILE* stream);
    void open_string(const std::string& data);

    // Read rows
    std::unique_ptr<Row> read_row();
    std::vector<Row> read_all();

    // Close
    void close();

    // Error info
    std::string get_error_message() const;

    // Iterator support
    Iterator begin();
    Iterator end();

    // Move-only
    Parser(Parser&& other) noexcept;
    Parser& operator=(Parser&& other) noexcept;
};
```

#### Constructors

##### `Parser()`

Creates a parser with default configuration.

**Throws:** `Exception` if creation fails.

##### `Parser(const Config& config)`

Creates a parser with custom configuration.

**Parameters:**
- `config`: Configuration object

**Throws:** `Exception` if creation fails.

#### Methods

##### `open(const std::string& filename)`

Opens a CSV file for parsing.

**Parameters:**
- `filename`: Path to CSV file

**Throws:** `Exception` on error (file not found, permission denied, etc.)

**Example:**

```cpp
Parser parser;
parser.open("data.csv");
```

##### `open(FILE* stream)`

Opens a FILE* stream for parsing.

**Parameters:**
- `stream`: Open FILE* stream

**Throws:** `Exception` on error.

##### `open_string(const std::string& data)`

Parses CSV data from a string.

**Parameters:**
- `data`: CSV data as string

**Throws:** `Exception` on error.

**Example:**

```cpp
Parser parser;
parser.open_string("A,B,C\n1,2,3");
```

##### `read_row()`

Reads the next row from the CSV.

**Returns:** `std::unique_ptr<Row>` containing the row, or `nullptr` at end of file.

**Throws:** `Exception` on parse error.

**Example:**

```cpp
while (auto row = parser.read_row()) {
    std::cout << "Row " << row->row_number() << "\n";
}
```

##### `read_all()`

Reads all remaining rows into a vector.

**Returns:** `std::vector<Row>` containing all rows.

**Throws:** `Exception` on parse error.

**Example:**

```cpp
std::vector<Row> rows = parser.read_all();
for (const auto& row : rows) {
    // Process row
}
```

##### `close()`

Closes the current CSV source. Called automatically by destructor.

##### `get_error_message()`

Gets detailed error message for the last error.

**Returns:** Error message as `std::string`.

##### `begin()` / `end()`

Returns iterators for range-based loops.

**Returns:** Input iterator.

**Note:** Iterator uses lazy evaluation - no data is read until iteration begins.

**Example:**

```cpp
Parser parser;
parser.open("data.csv");

for (auto& row : parser) {
    for (const auto& field : row) {
        std::cout << field << " ";
    }
    std::cout << "\n";
}
```

---

### Writer

CSV writer with RAII.

```cpp
class Writer {
public:
    Writer();
    explicit Writer(const Config& config);
    ~Writer();

    // Open destinations
    void open(const std::string& filename);
    void open(FILE* stream);

    // Write rows
    void write_row(const std::vector<std::string>& fields);
    void write_row(std::initializer_list<std::string> fields);
    void write_row(const char** fields, size_t count);

    // Close
    void close();

    // Error info
    std::string get_error_message() const;

    // Move-only
    Writer(Writer&& other) noexcept;
    Writer& operator=(Writer&& other) noexcept;
};
```

#### Constructors

##### `Writer()`

Creates a writer with default configuration.

**Throws:** `Exception` if creation fails.

##### `Writer(const Config& config)`

Creates a writer with custom configuration.

**Parameters:**
- `config`: Configuration object

**Throws:** `Exception` if creation fails.

#### Methods

##### `open(const std::string& filename)`

Opens a file for writing.

**Parameters:**
- `filename`: Path to output file

**Throws:** `Exception` on error (permission denied, disk full, etc.)

**Example:**

```cpp
Writer writer;
writer.open("output.csv");
```

##### `open(FILE* stream)`

Opens a FILE* stream for writing.

**Parameters:**
- `stream`: Open FILE* stream

**Throws:** `Exception` on error.

##### `write_row(const std::vector<std::string>& fields)`

Writes a row from a vector of strings.

**Parameters:**
- `fields`: Vector of field values

**Throws:** `Exception` on error.

**Example:**

```cpp
std::vector<std::string> row = {"John", "30", "NYC"};
writer.write_row(row);
```

##### `write_row(std::initializer_list<std::string> fields)`

Writes a row from an initializer list.

**Parameters:**
- `fields`: Initializer list of field values

**Throws:** `Exception` on error.

**Example:**

```cpp
writer.write_row({"John", "30", "NYC"});
```

##### `write_row(const char** fields, size_t count)`

Writes a row from a C-style array.

**Parameters:**
- `fields`: Array of C-strings
- `count`: Number of fields

**Throws:** `Exception` on error.

##### `close()`

Closes the writer and flushes data. Called automatically by destructor.

##### `get_error_message()`

Gets detailed error message for the last error.

**Returns:** Error message as `std::string`.

---

### Exception

Exception class for CSV errors.

```cpp
class Exception : public std::runtime_error {
public:
    explicit Exception(const std::string& msg);
    explicit Exception(csvkit_error_t err);
};
```

Inherits from `std::runtime_error`. Use `what()` to get error message.

**Example:**

```cpp
try {
    Parser parser;
    parser.open("nonexistent.csv");
} catch (const csvkit::Exception& e) {
    std::cerr << "CSV Error: " << e.what() << "\n";
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
}
```

---

## Helper Functions

### `read_file()`

```cpp
std::vector<Row> read_file(const std::string& filename,
                           const Config& config = Config());
```

Reads an entire CSV file into a vector of rows.

**Parameters:**
- `filename`: Path to CSV file
- `config`: Configuration (optional, defaults to default config)

**Returns:** `std::vector<Row>` containing all rows.

**Throws:** `Exception` on error.

**Example:**

```cpp
auto rows = csvkit::read_file("data.csv");
for (const auto& row : rows) {
    std::cout << "Row has " << row.size() << " fields\n";
}
```

### `read_string()`

```cpp
std::vector<Row> read_string(const std::string& data,
                             const Config& config = Config());
```

Parses CSV data from a string into a vector of rows.

**Parameters:**
- `data`: CSV data as string
- `config`: Configuration (optional)

**Returns:** `std::vector<Row>` containing all rows.

**Throws:** `Exception` on error.

**Example:**

```cpp
std::string csv = "A,B,C\n1,2,3\n4,5,6";
auto rows = csvkit::read_string(csv);
```

---

## Examples

### Basic Reading

```cpp
#include <csvkit.hpp>
#include <iostream>

using namespace csvkit;

int main() {
    try {
        Parser parser;
        parser.open("data.csv");

        while (auto row = parser.read_row()) {
            for (const auto& field : *row) {
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

### Range-Based Loop

```cpp
Parser parser;
parser.open("data.csv");

for (auto& row : parser) {
    std::cout << "Row " << row.row_number() << ": ";
    for (const auto& field : row) {
        std::cout << field << " ";
    }
    std::cout << "\n";
}
```

### Custom Configuration

```cpp
Config config;
config.delimiter('\t')
      .quote_char('\'')
      .trim_whitespace(true)
      .skip_empty_rows(true);

Parser parser(config);
parser.open("data.tsv");
```

### Writing CSV

```cpp
Writer writer;
writer.open("output.csv");

// Write header
writer.write_row({"Name", "Age", "City"});

// Write data rows
writer.write_row({"John Doe", "30", "New York"});
writer.write_row({"Jane Smith", "25", "Los Angeles"});

writer.close();
```

### Parsing from String

```cpp
std::string csv_data = R"(
Name,Age,City
John,30,NYC
Jane,25,LA
)";

Parser parser;
parser.open_string(csv_data);

auto rows = parser.read_all();
std::cout << "Read " << rows.size() << " rows\n";
```

### Helper Functions

```cpp
// Read entire file
auto rows = csvkit::read_file("data.csv");

// With custom config
Config config;
config.delimiter(';');
auto rows2 = csvkit::read_file("data.csv", config);

// Parse from string
auto rows3 = csvkit::read_string("A,B\n1,2");
```

### Error Handling

```cpp
try {
    Parser parser;
    parser.open("data.csv");

    while (auto row = parser.read_row()) {
        // Safe access with bounds checking
        try {
            std::string name = row->at(0);
            std::string age = row->at(1);
        } catch (const std::out_of_range& e) {
            std::cerr << "Missing field in row "
                      << row->row_number() << "\n";
        }
    }
} catch (const csvkit::Exception& e) {
    std::cerr << "CSV error: " << e.what() << "\n";
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
}
```

### Move Semantics

```cpp
// Move construction
Parser parser1;
parser1.open("data.csv");
Parser parser2 = std::move(parser1);  // Transfer ownership

// Move assignment
Writer writer1;
writer1.open("output.csv");
Writer writer2;
writer2 = std::move(writer1);  // Transfer ownership
```

### STL Integration

```cpp
Parser parser;
parser.open("data.csv");

std::vector<Row> rows = parser.read_all();

// Use STL algorithms
std::cout << "Total rows: " << rows.size() << "\n";

// Filter rows
std::vector<Row> filtered;
std::copy_if(rows.begin(), rows.end(),
             std::back_inserter(filtered),
             [](const Row& row) { return row.size() > 3; });

// Transform
std::vector<std::string> first_fields;
std::transform(rows.begin(), rows.end(),
               std::back_inserter(first_fields),
               [](const Row& row) { return row[0]; });
```

## Thread Safety

Each `Parser` and `Writer` instance is **not thread-safe**. Use separate instances per thread:

```cpp
// Thread 1
void thread1() {
    Parser parser;
    parser.open("data1.csv");
    // Use parser...
}

// Thread 2
void thread2() {
    Parser parser;
    parser.open("data2.csv");
    // Use parser...
}
```

## Performance Notes

- **Move semantics** avoid unnecessary copies
- **Iterators** enable lazy evaluation - rows are read on-demand
- **`operator[]`** has no overhead (no bounds checking)
- **RAII** ensures resources are freed even on exceptions
- Minimal overhead over C API (thin wrapper)

## Compiling

```bash
# Link against C++ bindings
g++ -std=c++11 myapp.cpp -o myapp -lcsvkit++

# Both libcsvkit++ and libcsvkit are required
g++ -std=c++11 myapp.cpp -o myapp -lcsvkit++ -lcsvkit
```

Ensure libraries are in your library path:

```bash
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```

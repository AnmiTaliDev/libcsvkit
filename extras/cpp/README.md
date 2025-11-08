# libcsvkit C++ Bindings

Modern C++ wrapper for libcsvkit with RAII, exceptions, and STL integration.

## Features

- **RAII-based resource management** - No manual memory management
- **Exception-based error handling** - C++ exceptions instead of error codes
- **STL integration** - Works with `std::vector`, `std::string`, range-based loops
- **Move semantics** - Efficient ownership transfer
- **Fluent configuration API** - Chain configuration methods
- **Type-safe** - Leverages C++ type system
- **Compiled library** - Header + shared library (libcsvkit++.so)

## Requirements

- C++11 or later
- libcsvkit installed (C library)

## Installation

1. Configure with C++ support enabled:
```bash
./configure --enable-cpp
```

2. Build C library and C++ bindings:
```bash
make
make cpp
```

3. Install both libraries:
```bash
sudo make install
sudo make install-cpp
```

This installs:
- `/usr/local/include/csvkit.hpp` - C++ header
- `/usr/local/lib/libcsvkit++.so` - C++ shared library
- `/usr/local/lib/libcsvkit.so` - C library (dependency)

## Usage

### Basic Reading

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
            std::cout << std::endl;
        }
    } catch (const Exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
```

### Custom Configuration

```cpp
Config config;
config.delimiter(';')
      .quote_char('\'')
      .trim_whitespace(true)
      .skip_empty_rows(true);

Parser parser(config);
parser.open("data.csv");
```

### Writing CSV

```cpp
Writer writer;
writer.open("output.csv");

writer.write_row({"Name", "Age", "City"});
writer.write_row({"John", "30", "NYC"});
writer.write_row({"Jane", "25", "LA"});

writer.close();
```

### Reading from String

```cpp
std::string csv_data = "A,B,C\n1,2,3\n4,5,6";

Parser parser;
parser.open_string(csv_data);

while (auto row = parser.read_row()) {
    for (const auto& field : *row) {
        std::cout << field << " ";
    }
    std::cout << std::endl;
}
```

### Helper Functions

```cpp
// Read entire file into vector
auto rows = csvkit::read_file("data.csv");

// Read from string
auto rows = csvkit::read_string("A,B\n1,2");
```

### Accessing Fields

```cpp
auto row = parser.read_row();

// By index
std::string first = (*row)[0];

// Safe access with bounds checking
std::string second = row->at(1);

// Field count
size_t count = row->field_count();

// Row number (1-based)
size_t num = row->row_number();
```

## Building Your Programs

```bash
# Compile with C++ bindings
g++ -std=c++11 myapp.cpp -o myapp -lcsvkit++

# Or explicitly link both libraries
g++ -std=c++11 myapp.cpp -o myapp -lcsvkit++ -lcsvkit
```

**Note:** Both `libcsvkit++.so` and `libcsvkit.so` must be in your library path:

```bash
# Add to library path if needed
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# Or update cache (requires sudo)
sudo ldconfig
```

## API Reference

### Classes

#### `Config`
Fluent configuration builder for CSV parsing/writing.

Methods:
- `delimiter(char)` - Set field delimiter
- `quote_char(char)` - Set quote character
- `escape_char(char)` - Set escape character
- `trim_whitespace(bool)` - Enable/disable whitespace trimming
- `skip_empty_rows(bool)` - Enable/disable empty row skipping
- `strict_mode(bool)` - Enable/disable RFC 4180 strict mode

#### `Parser`
CSV reader with RAII and iterator support.

Methods:
- `open(const std::string& filename)` - Open CSV file
- `open(FILE* stream)` - Open from FILE* stream
- `open_string(const std::string& data)` - Parse from string
- `read_row()` - Read next row (returns `unique_ptr<Row>`)
- `read_all()` - Read all rows into vector
- `close()` - Close current source
- `begin()`, `end()` - Iterator support for range-based loops

#### `Row`
Represents a single CSV row.

Methods:
- `operator[](size_t index)` - Access field by index
- `at(size_t index)` - Safe field access with bounds checking
- `size()`, `field_count()` - Number of fields
- `row_number()` - Row number in source (1-based)
- `empty()` - Check if row is empty
- `begin()`, `end()` - Iterator support
- `fields()` - Get all fields as `vector<string>`

#### `Writer`
CSV writer with RAII.

Methods:
- `open(const std::string& filename)` - Open file for writing
- `open(FILE* stream)` - Open stream for writing
- `write_row(const vector<string>&)` - Write row from vector
- `write_row(initializer_list<string>)` - Write row from initializer list
- `close()` - Close writer
- `get_error_message()` - Get detailed error message

#### `Exception`
Exception class for CSV errors.

### Helper Functions

- `read_file(filename, config)` - Read entire CSV file
- `read_string(data, config)` - Read CSV from string

## Examples

See [example.cpp](example.cpp) for comprehensive usage examples covering:
- Basic reading and writing
- Custom configurations
- Range-based loops
- String parsing
- Error handling
- Move semantics

## Thread Safety

Each `Parser` and `Writer` instance is thread-safe for use in a single thread. Use separate instances per thread.

## Performance

The C++ wrapper has minimal overhead over the C API:
- RAII-based memory management
- Move semantics avoid unnecessary copies
- Iterators allow lazy evaluation

## License

GNU LGPL 3.0 - Same as libcsvkit

## Author

AnmiTaliDev <anmitali198@gmail.com>

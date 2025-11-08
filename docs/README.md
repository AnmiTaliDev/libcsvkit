# libcsvkit Documentation

Complete documentation for libcsvkit CSV parsing library.

## Documentation Index

### Getting Started

- **[Building Guide](BUILDING.md)** - How to build and install libcsvkit
  - System requirements
  - Configuration options
  - Installation instructions
  - Platform-specific notes
  - Troubleshooting

- **[User Guide](GUIDE.md)** - Comprehensive usage guide
  - Basic concepts
  - Reading and writing CSV files
  - Configuration options
  - Error handling
  - Advanced usage patterns
  - Best practices

### API Reference

- **[C API Reference](API.md)** - Complete C API documentation
  - Types and structures
  - Parser API
  - Writer API
  - Configuration
  - Error handling
  - Code examples

- **[C++ API Reference](CPP_API.md)** - Complete C++ API documentation
  - Classes (Config, Row, Parser, Writer, Exception)
  - Helper functions
  - STL integration
  - Move semantics
  - Code examples

## Quick Links

### Installation

```bash
# Build and install
./configure --enable-cpp
make
sudo make install
sudo make install-cpp
```

See [BUILDING.md](BUILDING.md) for details.

### Basic Usage

#### C Example

```c
#include <csvkit.h>

csvkit_parser_t *parser = csvkit_parser_new();
csvkit_open_file(parser, "data.csv");

csvkit_row_t *row;
while (csvkit_read_row(parser, &row) == CSVKIT_OK) {
    // Process row
    csvkit_row_free(row);
}

csvkit_parser_free(parser);
```

#### C++ Example

```cpp
#include <csvkit.hpp>
using namespace csvkit;

Parser parser;
parser.open("data.csv");

for (auto& row : parser) {
    for (const auto& field : row) {
        std::cout << field << " ";
    }
}
```

See [GUIDE.md](GUIDE.md) for comprehensive examples.

## Features

### Core Features

- **RFC 4180 Compliant** - Follows CSV standard specification
- **Flexible Configuration** - Support for various CSV dialects
- **Streaming API** - Memory-efficient processing of large files
- **Error Reporting** - Detailed error messages with line numbers
- **Cross-Platform** - Linux, macOS, BSD, Windows

### C API Features

- Pure C99 implementation
- Minimal dependencies (standard C library only)
- Opaque types with clean API
- Static and shared library builds
- Thread-safe per-instance

### C++ API Features

- Modern C++11 design
- RAII resource management
- Exception-based error handling
- STL integration (std::vector, std::string)
- Move semantics
- Range-based loop support
- Fluent configuration API
- Header + compiled library (not header-only)

## Documentation Structure

```
docs/
├── README.md        # This file - documentation index
├── BUILDING.md      # Build and installation guide
├── GUIDE.md         # Comprehensive user guide
├── API.md           # C API reference
└── CPP_API.md       # C++ API reference
```

## API Overview

### C API

#### Types

- `csvkit_parser_t` - CSV parser handle
- `csvkit_writer_t` - CSV writer handle
- `csvkit_row_t` - Row structure
- `csvkit_config_t` - Configuration structure
- `csvkit_error_t` - Error codes

#### Parser Functions

- `csvkit_parser_new()` - Create parser
- `csvkit_open_file()` - Open file
- `csvkit_read_row()` - Read next row
- `csvkit_close()` - Close source
- `csvkit_parser_free()` - Free parser

#### Writer Functions

- `csvkit_writer_new()` - Create writer
- `csvkit_writer_open_file()` - Open file
- `csvkit_writer_write_row()` - Write row
- `csvkit_writer_close()` - Close writer
- `csvkit_writer_free()` - Free writer

See [API.md](API.md) for complete reference.

### C++ API

#### Classes

- `Config` - Configuration builder
- `Row` - CSV row (read-only)
- `Parser` - CSV parser
- `Writer` - CSV writer
- `Exception` - Error exception

#### Helper Functions

- `read_file()` - Read entire CSV file
- `read_string()` - Parse CSV from string

See [CPP_API.md](CPP_API.md) for complete reference.

## Common Tasks

### Reading a CSV File

**C:**

```c
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
```

**C++:**

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

### Writing a CSV File

**C:**

```c
csvkit_writer_t *writer = csvkit_writer_new();
csvkit_writer_open_file(writer, "output.csv");

const char *row1[] = {"A", "B", "C"};
csvkit_writer_write_row(writer, row1, 3);

csvkit_writer_close(writer);
csvkit_writer_free(writer);
```

**C++:**

```cpp
Writer writer;
writer.open("output.csv");

writer.write_row({"A", "B", "C"});
writer.write_row({"1", "2", "3"});

writer.close();
```

### Custom Configuration

**C:**

```c
csvkit_config_t config = csvkit_config_default();
config.delimiter = '\t';
config.trim_whitespace = true;

csvkit_parser_t *parser = csvkit_parser_new_with_config(&config);
```

**C++:**

```cpp
Config config;
config.delimiter('\t')
      .trim_whitespace(true);

Parser parser(config);
```

## Performance

### Memory Usage

- Streaming API uses minimal memory
- Only current row kept in memory during parsing
- No full-file buffering required

### Speed

- Optimized C implementation
- Zero-copy where possible
- Minimal allocations

### Scalability

Tested with:
- Files up to several GB
- Millions of rows
- Thousands of columns per row

## Thread Safety

Each parser/writer instance is **not thread-safe** internally. Use separate instances per thread:

```cpp
// Thread-safe usage
void process_file(const std::string& filename) {
    Parser parser;  // Separate instance per thread
    parser.open(filename);
    // Process...
}

std::thread t1(process_file, "file1.csv");
std::thread t2(process_file, "file2.csv");
```

## Error Handling

### C Error Codes

```c
csvkit_error_t err = csvkit_open_file(parser, "data.csv");
if (err != CSVKIT_OK) {
    fprintf(stderr, "Error: %s\n", csvkit_get_error_msg(parser));
    return 1;
}
```

### C++ Exceptions

```cpp
try {
    Parser parser;
    parser.open("data.csv");
} catch (const csvkit::Exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
}
```

## Examples

See the [examples/](../examples/) directory for complete examples:

- Basic reading and writing
- Custom configurations
- Error handling
- Streaming large files
- Data transformation
- Format conversion

## Platform Support

### Tested Platforms

- **Linux**: Ubuntu, Debian, Fedora, Arch Linux
- **macOS**: 10.14+
- **BSD**: FreeBSD, OpenBSD
- **Windows**: MinGW, Cygwin, WSL

### Compilers

- **GCC**: 4.9+ (C99, C++11)
- **Clang**: 3.4+ (C99, C++11)
- **MSVC**: 2015+ (via C++ bindings)

## License

GNU Lesser General Public License v3.0 (LGPL-3.0)

See [LICENSE](../LICENSE) for details.

## Contributing

Contributions welcome! Please:

1. Follow existing code style
2. Add tests for new features
3. Update documentation
4. Submit pull request

## Support

- **Issues**: [GitHub Issues](https://github.com/yourusername/libcsvkit/issues)
- **Email**: anmitali198@gmail.com

## Version

Current version: **0.1.0**

## Changelog

See [CHANGELOG.md](../CHANGELOG.md) for version history.

## See Also

- [RFC 4180](https://tools.ietf.org/html/rfc4180) - CSV format specification
- [CSV on Wikipedia](https://en.wikipedia.org/wiki/Comma-separated_values)

## Documentation Conventions

### Code Examples

Examples are provided in both C and C++ where applicable.

### Notation

- `function()` - Function name
- `Type` - Type name
- `CONSTANT` - Constant/macro name
- `parameter` - Parameter name

### Version Information

Documentation version: 0.1.0
Last updated: 2025-01-08

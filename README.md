# libcsvkit

A lightweight, efficient, and robust CSV parsing and writing library for C.

**Author:** AnmiTaliDev <anmitali198@gmail.com>
**License:** GNU LGPL 3.0
**Version:** 0.1.0

## Philosophy

libcsvkit follows the classic Unix philosophy with modern convenience:

- **Do one thing well** - Focus on CSV parsing and writing
- **Simple, clean API** - Easy to use, hard to misuse
- **No dependencies** - Only standard C library
- **Memory efficient** - Stream-based parsing with optimized algorithms
- **RFC 4180 compliant** - Standard CSV format support with strict mode
- **Flexible** - Configurable delimiters, quotes, and behavior
- **Robust** - Comprehensive validation and error handling

## Features

- ✅ Read CSV from files, streams, or strings
- ✅ Write CSV to files or streams with automatic quoting
- ✅ Handle quoted fields with embedded delimiters, newlines, and CRLF
- ✅ Support for custom delimiters, quotes, and escape characters
- ✅ Flexible escape modes: RFC 4180 (`""`) or custom escape character (`\`)
- ✅ Trim whitespace option (excluding quoted fields)
- ✅ Skip empty rows option
- ✅ **Strict mode** for RFC 4180 compliance validation
- ✅ **Configuration validation** - rejects invalid delimiter/quote combinations
- ✅ Memory-efficient streaming parser
- ✅ Optimized: no `strlen()` or `memmove()` in hot paths
- ✅ No external dependencies
- ✅ Simple, intuitive API

## Building

### Requirements

- C99-compliant compiler (gcc, clang, etc.)
- make
- Standard C library (POSIX)
- **Optional:** C++11-compliant compiler for C++ bindings (g++, clang++)

### Quick Start

```bash
# Configure with defaults
./configure

# Or customize installation
./configure --prefix=/usr --build-type=Release

# Enable C++ bindings
./configure --enable-cpp

# Build the library
make

# Build C++ bindings (if enabled)
make cpp

# Build examples
make examples

# Install (may require sudo)
make install

# Install C++ bindings (if enabled)
make install-cpp
```

### Configuration Options

```bash
./configure --help
```

Available options:
- `--prefix=PREFIX` - Installation prefix (default: /usr/local)
- `--libdir=DIR` - Library directory (default: PREFIX/lib)
- `--includedir=DIR` - Header directory (default: PREFIX/include)
- `--cc=PATH` - C compiler path
- `--cxx=PATH` - C++ compiler path
- `--build-type=TYPE` - Release or Debug
- `--enable-debug` - Enable debug symbols
- `--enable-cpp` - Enable C++ bindings
- `--disable-static` - Don't build static library
- `--disable-shared` - Don't build shared library

## C++ Bindings

libcsvkit provides modern C++ bindings with RAII, exceptions, and STL integration.

### Features

- RAII-based resource management
- Exception-based error handling
- STL integration (std::vector, std::string)
- Range-based loop support
- Move semantics
- Fluent configuration API

### Building C++ Bindings

```bash
./configure --enable-cpp
make
make cpp
sudo make install
sudo make install-cpp
```

### C++ Usage Example

```cpp
#include <csvkit.hpp>
#include <iostream>

using namespace csvkit;

int main() {
    try {
        // Read CSV file
        Parser parser;
        parser.open("data.csv");

        // Range-based loop
        for (auto& row : parser) {
            for (const auto& field : row) {
                std::cout << field << " ";
            }
            std::cout << std::endl;
        }

        // Write CSV file
        Writer writer;
        writer.open("output.csv");
        writer.write_row({"Name", "Age", "City"});
        writer.write_row({"John", "30", "NYC"});
        writer.close();

    } catch (const Exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
```

### Compiling with C++ Bindings

```bash
g++ -std=c++11 myapp.cpp -o myapp -lcsvkit++
```

For detailed C++ API documentation, see [extras/cpp/README.md](extras/cpp/README.md).

## License

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

See [LICENSE](LICENSE) for the full license text.

## Author

**AnmiTaliDev**
Email: anmitali198@gmail.com

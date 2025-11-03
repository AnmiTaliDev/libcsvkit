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

### Quick Start

```bash
# Configure with defaults
./configure

# Or customize installation
./configure --prefix=/usr --build-type=Release

# Build the library
make

# Build examples
make examples

# Install (may require sudo)
make install
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
- `--build-type=TYPE` - Release or Debug
- `--enable-debug` - Enable debug symbols
- `--disable-static` - Don't build static library
- `--disable-shared` - Don't build shared library

## License

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

See [LICENSE](LICENSE) for the full license text.

## Author

**AnmiTaliDev**
Email: anmitali198@gmail.com

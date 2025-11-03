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

## Usage

### Reading CSV

```c
#include <csvkit.h>

/* Create parser */
csvkit_parser_t *parser = csvkit_parser_new();

/* Open file */
csvkit_open_file(parser, "data.csv");

/* Read rows */
csvkit_row_t *row;
while (csvkit_read_row(parser, &row) == CSVKIT_OK) {
    for (size_t i = 0; i < row->field_count; i++) {
        printf("Field %zu: %s\n", i, row->fields[i]);
    }
    csvkit_row_free(row);
}

/* Cleanup */
csvkit_parser_free(parser);
```

### Writing CSV

```c
#include <csvkit.h>

/* Create writer */
csvkit_writer_t *writer = csvkit_writer_new();

/* Open file */
csvkit_writer_open_file(writer, "output.csv");

/* Write rows */
const char *row1[] = {"Name", "Age", "City"};
csvkit_writer_write_row(writer, row1, 3);

const char *row2[] = {"Alice", "30", "New York"};
csvkit_writer_write_row(writer, row2, 3);

/* Cleanup */
csvkit_writer_free(writer);
```

### Custom Configuration

```c
/* Create custom config */
csvkit_config_t config = csvkit_config_default();
config.delimiter = ';';
config.trim_whitespace = true;
config.skip_empty_rows = true;
config.strict_mode = true;  /* Enable strict RFC 4180 validation */

/* Create parser with config (returns NULL if config is invalid) */
csvkit_parser_t *parser = csvkit_parser_new_with_config(&config);
if (!parser) {
    fprintf(stderr, "Invalid configuration\n");
    return -1;
}
```

### Strict Mode

Strict mode enables RFC 4180 compliance validation:

```c
csvkit_config_t config = csvkit_config_default();
config.strict_mode = true;

csvkit_parser_t *parser = csvkit_parser_new_with_config(&config);

/* In strict mode:
 * - All rows must have the same number of fields (based on first row)
 * - No characters allowed after closing quote (except delimiter/newline)
 * - Returns CSVKIT_ERROR_PARSE on violations
 */
```

### Custom Escape Character

```c
/* Use backslash as escape character instead of double-quote */
csvkit_config_t config = csvkit_config_default();
config.escape_char = '\\';

csvkit_parser_t *parser = csvkit_parser_new_with_config(&config);

/* Now handles: "field with \" quote" instead of "field with "" quote" */
```

### TSV (Tab-Separated Values)

```c
csvkit_config_t config = csvkit_config_default();
config.delimiter = '\t';

csvkit_parser_t *parser = csvkit_parser_new_with_config(&config);
```

### Parse from String

```c
const char *csv_data = "Name,Age\nAlice,30\nBob,25\n";
csvkit_open_string(parser, csv_data, strlen(csv_data));
```

### Parse from Stream

```c
FILE *fp = fopen("data.csv", "r");
csvkit_open_stream(parser, fp);
```

## API Reference

### Parser Functions

- `csvkit_parser_t *csvkit_parser_new(void)` - Create parser with defaults
- `csvkit_parser_t *csvkit_parser_new_with_config(const csvkit_config_t *config)` - Create parser with custom config (returns NULL if invalid)
- `void csvkit_parser_free(csvkit_parser_t *parser)` - Free parser
- `csvkit_error_t csvkit_open_file(csvkit_parser_t *parser, const char *filename)` - Open CSV file
- `csvkit_error_t csvkit_open_stream(csvkit_parser_t *parser, FILE *stream)` - Open CSV stream
- `csvkit_error_t csvkit_open_string(csvkit_parser_t *parser, const char *data, size_t len)` - Parse CSV string
- `csvkit_error_t csvkit_read_row(csvkit_parser_t *parser, csvkit_row_t **row)` - Read next row
- `void csvkit_row_free(csvkit_row_t *row)` - Free row
- `void csvkit_close(csvkit_parser_t *parser)` - Close current source
- `const char *csvkit_get_error_msg(csvkit_parser_t *parser)` - Get error message

### Writer Functions

- `csvkit_writer_t *csvkit_writer_new(void)` - Create writer
- `csvkit_writer_t *csvkit_writer_new_with_config(const csvkit_config_t *config)` - Create writer with config (returns NULL if invalid)
- `void csvkit_writer_free(csvkit_writer_t *writer)` - Free writer
- `csvkit_error_t csvkit_writer_open_file(csvkit_writer_t *writer, const char *filename)` - Open file for writing
- `csvkit_error_t csvkit_writer_open_stream(csvkit_writer_t *writer, FILE *stream)` - Open stream for writing
- `csvkit_error_t csvkit_writer_write_row(csvkit_writer_t *writer, const char **fields, size_t field_count)` - Write row
- `void csvkit_writer_close(csvkit_writer_t *writer)` - Close writer

### Helper Functions

- `csvkit_config_t csvkit_config_default(void)` - Get default config
- `const char *csvkit_row_get_field(const csvkit_row_t *row, size_t index)` - Get field by index
- `size_t csvkit_row_field_count(const csvkit_row_t *row)` - Get field count
- `bool csvkit_row_is_empty(const csvkit_row_t *row)` - Check if row is empty

### Configuration

```c
typedef struct {
    char delimiter;          /* Field delimiter (default: ',') */
    char quote_char;         /* Quote character (default: '"') */
    char escape_char;        /* Escape character (default: '"') */
    bool trim_whitespace;    /* Trim leading/trailing whitespace (default: false) */
    bool skip_empty_rows;    /* Skip rows with no data (default: false) */
    bool strict_mode;        /* Strict RFC 4180 compliance (default: false) */
} csvkit_config_t;
```

**Configuration Validation:**

The following configurations are **rejected** (return NULL):
- `delimiter == '\n'` or `'\r'` - Can't use line endings as delimiter
- `delimiter == quote_char` - Can't distinguish fields from quotes
- `quote_char == '\n'` or `'\r'` - Invalid quote character
- `escape_char == '\n'` or `'\r'` - Invalid escape character
- `config == NULL` - Null pointer

**Valid configurations:**
- Standard CSV: `,` delimiter, `"` quote/escape
- TSV: `\t` delimiter, `"` quote/escape
- Semicolon CSV: `;` delimiter, `"` quote/escape
- Backslash escape: `,` delimiter, `"` quote, `\` escape

### Error Codes

- `CSVKIT_OK` - Success
- `CSVKIT_ERROR_MEMORY` - Out of memory
- `CSVKIT_ERROR_IO` - I/O error
- `CSVKIT_ERROR_PARSE` - Parse error (unclosed quote, field count mismatch in strict mode, etc.)
- `CSVKIT_ERROR_INVALID_ARG` - Invalid argument
- `CSVKIT_ERROR_EOF` - End of file

## Advanced Features

### CRLF Handling

The library correctly handles different line endings:
- **Outside quoted fields:** `\r`, `\n`, `\r\n` all treated as row terminators
- **Inside quoted fields:** `\r`, `\n`, `\r\n` preserved as-is (part of field data)

```c
/* This CSV has CRLF inside a quoted field: */
const char *csv = "\"field with\r\nline break\",\"another\"\n";

/* The \r\n is preserved inside the quoted field */
```

### Escape Character Modes

**RFC 4180 mode (default):** `escape_char == quote_char`
```c
config.escape_char = '"';  /* "" becomes " */
/* Example: "field with ""quote""" → field with "quote" */
```

**Backslash mode:** `escape_char != quote_char`
```c
config.escape_char = '\\';  /* \" becomes " */
/* Example: "field with \"quote\"" → field with "quote" */
/* Non-special chars: "field with \x" → field with \x */
```

### Whitespace Trimming

When `trim_whitespace = true`:
- Trims leading/trailing whitespace from **unquoted** fields
- **Preserves** whitespace in quoted fields

```c
config.trim_whitespace = true;

/* Input:  " abc ",  "  def  " */
/* Result:   abc  ,    def    (spaces inside quotes preserved) */
```

## Examples

See the [examples](examples/) directory for complete examples:

- `read_csv.c` - Read and display CSV file
- `write_csv.c` - Write CSV file with automatic quoting
- `custom_config.c` - Use custom delimiters and options
- `test_comprehensive.c` - Comprehensive test suite
- `stress_test.c` - Performance benchmarks

Build examples:
```bash
make examples
./examples/read_csv data.csv
./examples/write_csv output.csv
```

## Performance

Optimized for speed and memory efficiency:

- **No `strlen()` in hot paths** - Single-pass string operations
- **No `memmove()`** - Direct character copying
- **Stream-based parsing** - Constant memory usage
- **Dynamic buffer resizing** - Handles large fields efficiently

Benchmark results (examples/stress_test):
- **Write performance:** ~9M rows/second
- **Read performance:** ~7M rows/second
- **Large fields:** 10KB fields handled efficiently
- **Many fields:** 1000 fields per row supported
- **Empty rows:** 100K+ empty rows with skip_empty_rows

## Linking

### Static Linking

```bash
gcc myapp.c -o myapp -L/path/to/lib -lcsvkit
```

### Dynamic Linking

```bash
gcc myapp.c -o myapp -L/path/to/lib -lcsvkit
export LD_LIBRARY_PATH=/path/to/lib:$LD_LIBRARY_PATH
./myapp
```

### pkg-config (after installation)

```bash
gcc myapp.c -o myapp $(pkg-config --cflags --libs csvkit)
```

## Testing

Create a test CSV file:

```bash
cat > test.csv << EOF
Name,Age,City
"Alice Smith",30,"New York"
Bob Johnson,25,Los Angeles
"Charlie Brown",35,Chicago
EOF

# Run the example
./examples/read_csv test.csv
```

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## License

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

See [LICENSE](LICENSE) for the full license text.

## Author

**AnmiTaliDev**
Email: anmitali198@gmail.com

## Changelog

### Version 0.1.0 (2025-01-03)

**Initial release with comprehensive improvements:**

- ✅ Basic CSV parsing and writing
- ✅ Support for files, streams, and strings
- ✅ Configurable delimiters, quotes, and escape characters
- ✅ RFC 4180 compliance with strict mode
- ✅ Optimized parsing (no strlen/memmove in hot paths)
- ✅ Configuration validation (rejects invalid configs)
- ✅ Proper CRLF handling (inside/outside quotes)
- ✅ Flexible escape modes (RFC 4180 or backslash)
- ✅ Unclosed quote detection
- ✅ Field count validation in strict mode
- ✅ Whitespace trimming (excluding quoted fields)
- ✅ Empty row skipping
- ✅ Comprehensive error handling

**Bug fixes:**
- Fixed code duplication in parser (310 lines → 67 lines)
- Fixed EOF inside quoted fields not detected
- Fixed escape_char not being used
- Fixed trim_whitespace inefficiency
- Fixed escape logic for non-special characters
- Fixed CRLF conversion inside quoted fields

## Roadmap

- [ ] Add comprehensive test suite with CI/CD
- [ ] Add pkg-config support
- [ ] Add UTF-8 BOM handling
- [ ] Add column name lookup / header row parsing
- [ ] Add CSV validation utility
- [ ] Add more examples (data transformation, filtering)
- [ ] Performance profiling and further optimizations
- [ ] Windows compatibility testing

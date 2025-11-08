# libcsvkit C API Reference

Complete C API documentation for libcsvkit.

## Table of Contents

- [Types](#types)
- [Configuration](#configuration)
- [Parser API](#parser-api)
- [Writer API](#writer-api)
- [Row API](#row-api)
- [Error Handling](#error-handling)
- [Examples](#examples)

## Types

### `csvkit_config_t`

Configuration structure for CSV parsing and writing.

```c
typedef struct {
    char delimiter;          /* Field delimiter (default: ',') */
    char quote_char;        /* Quote character (default: '"') */
    char escape_char;       /* Escape character (default: '"') */
    bool trim_whitespace;   /* Trim leading/trailing whitespace */
    bool skip_empty_rows;   /* Skip rows with no data */
    bool strict_mode;       /* Strict RFC 4180 compliance */
} csvkit_config_t;
```

### `csvkit_row_t`

Represents a single CSV row.

```c
typedef struct {
    char **fields;          /* Array of field values */
    size_t field_count;     /* Number of fields */
    size_t row_number;      /* Row number in the file (1-based) */
} csvkit_row_t;
```

### `csvkit_parser_t`

Opaque parser handle. Created with `csvkit_parser_new()`.

### `csvkit_writer_t`

Opaque writer handle. Created with `csvkit_writer_new()`.

### `csvkit_error_t`

Error codes returned by library functions.

```c
typedef enum {
    CSVKIT_OK = 0,           /* Success */
    CSVKIT_ERROR_MEMORY,     /* Memory allocation error */
    CSVKIT_ERROR_IO,         /* I/O error */
    CSVKIT_ERROR_PARSE,      /* Parse error */
    CSVKIT_ERROR_INVALID_ARG,/* Invalid argument */
    CSVKIT_ERROR_EOF         /* End of file */
} csvkit_error_t;
```

## Configuration

### `csvkit_config_default()`

```c
csvkit_config_t csvkit_config_default(void);
```

Returns the default configuration:
- `delimiter`: `,`
- `quote_char`: `"`
- `escape_char`: `"`
- `trim_whitespace`: `false`
- `skip_empty_rows`: `false`
- `strict_mode`: `false`

**Returns:** Default configuration structure.

**Example:**

```c
csvkit_config_t config = csvkit_config_default();
config.delimiter = ';';
config.trim_whitespace = true;
```

## Parser API

### `csvkit_parser_new()`

```c
csvkit_parser_t *csvkit_parser_new(void);
```

Creates a new CSV parser with default configuration.

**Returns:** Parser handle, or `NULL` on error.

**Example:**

```c
csvkit_parser_t *parser = csvkit_parser_new();
if (!parser) {
    fprintf(stderr, "Failed to create parser\n");
    return 1;
}
```

### `csvkit_parser_new_with_config()`

```c
csvkit_parser_t *csvkit_parser_new_with_config(const csvkit_config_t *config);
```

Creates a new CSV parser with custom configuration.

**Parameters:**
- `config`: Configuration structure

**Returns:** Parser handle, or `NULL` on error.

**Example:**

```c
csvkit_config_t config = csvkit_config_default();
config.delimiter = '\t';
config.strict_mode = true;

csvkit_parser_t *parser = csvkit_parser_new_with_config(&config);
```

### `csvkit_open_file()`

```c
csvkit_error_t csvkit_open_file(csvkit_parser_t *parser, const char *filename);
```

Opens a CSV file for parsing.

**Parameters:**
- `parser`: Parser handle
- `filename`: Path to CSV file

**Returns:** `CSVKIT_OK` on success, error code otherwise.

**Example:**

```c
csvkit_error_t err = csvkit_open_file(parser, "data.csv");
if (err != CSVKIT_OK) {
    fprintf(stderr, "Error: %s\n", csvkit_get_error_msg(parser));
    return 1;
}
```

### `csvkit_open_stream()`

```c
csvkit_error_t csvkit_open_stream(csvkit_parser_t *parser, FILE *stream);
```

Opens a FILE* stream for parsing.

**Parameters:**
- `parser`: Parser handle
- `stream`: Open FILE* stream

**Returns:** `CSVKIT_OK` on success, error code otherwise.

**Example:**

```c
FILE *fp = fopen("data.csv", "r");
csvkit_error_t err = csvkit_open_stream(parser, fp);
```

### `csvkit_open_string()`

```c
csvkit_error_t csvkit_open_string(csvkit_parser_t *parser, const char *data, size_t len);
```

Parses CSV data from a string buffer.

**Parameters:**
- `parser`: Parser handle
- `data`: CSV data string
- `len`: Length of data

**Returns:** `CSVKIT_OK` on success, error code otherwise.

**Example:**

```c
const char *csv = "A,B,C\n1,2,3\n4,5,6";
csvkit_error_t err = csvkit_open_string(parser, csv, strlen(csv));
```

### `csvkit_read_row()`

```c
csvkit_error_t csvkit_read_row(csvkit_parser_t *parser, csvkit_row_t **row);
```

Reads the next row from the CSV.

**Parameters:**
- `parser`: Parser handle
- `row`: Pointer to receive the row (output parameter)

**Returns:**
- `CSVKIT_OK` on success
- `CSVKIT_ERROR_EOF` at end of file
- Other error codes on failure

**Note:** Caller must free the row with `csvkit_row_free()`.

**Example:**

```c
csvkit_row_t *row;
csvkit_error_t err;

while ((err = csvkit_read_row(parser, &row)) == CSVKIT_OK) {
    for (size_t i = 0; i < row->field_count; i++) {
        printf("%s ", row->fields[i]);
    }
    printf("\n");
    csvkit_row_free(row);
}

if (err != CSVKIT_ERROR_EOF) {
    fprintf(stderr, "Error: %s\n", csvkit_get_error_msg(parser));
}
```

### `csvkit_close()`

```c
void csvkit_close(csvkit_parser_t *parser);
```

Closes the current CSV source.

**Parameters:**
- `parser`: Parser handle

### `csvkit_parser_free()`

```c
void csvkit_parser_free(csvkit_parser_t *parser);
```

Frees the parser and all associated resources.

**Parameters:**
- `parser`: Parser handle

**Example:**

```c
csvkit_parser_free(parser);
```

### `csvkit_get_error_msg()`

```c
const char *csvkit_get_error_msg(csvkit_parser_t *parser);
```

Gets the last error message from the parser.

**Parameters:**
- `parser`: Parser handle

**Returns:** Error message string, or `NULL` if no error.

## Writer API

### `csvkit_writer_new()`

```c
csvkit_writer_t *csvkit_writer_new(void);
```

Creates a new CSV writer with default configuration.

**Returns:** Writer handle, or `NULL` on error.

### `csvkit_writer_new_with_config()`

```c
csvkit_writer_t *csvkit_writer_new_with_config(const csvkit_config_t *config);
```

Creates a new CSV writer with custom configuration.

**Parameters:**
- `config`: Configuration structure

**Returns:** Writer handle, or `NULL` on error.

### `csvkit_writer_open_file()`

```c
csvkit_error_t csvkit_writer_open_file(csvkit_writer_t *writer, const char *filename);
```

Opens a file for writing CSV data.

**Parameters:**
- `writer`: Writer handle
- `filename`: Path to output file

**Returns:** `CSVKIT_OK` on success, error code otherwise.

**Example:**

```c
csvkit_writer_t *writer = csvkit_writer_new();
csvkit_error_t err = csvkit_writer_open_file(writer, "output.csv");
```

### `csvkit_writer_open_stream()`

```c
csvkit_error_t csvkit_writer_open_stream(csvkit_writer_t *writer, FILE *stream);
```

Opens a FILE* stream for writing CSV data.

**Parameters:**
- `writer`: Writer handle
- `stream`: Open FILE* stream

**Returns:** `CSVKIT_OK` on success, error code otherwise.

### `csvkit_writer_write_row()`

```c
csvkit_error_t csvkit_writer_write_row(csvkit_writer_t *writer, const char **fields, size_t field_count);
```

Writes a row to the CSV file.

**Parameters:**
- `writer`: Writer handle
- `fields`: Array of field values
- `field_count`: Number of fields

**Returns:** `CSVKIT_OK` on success, error code otherwise.

**Example:**

```c
const char *row1[] = {"Name", "Age", "City"};
csvkit_writer_write_row(writer, row1, 3);

const char *row2[] = {"John", "30", "NYC"};
csvkit_writer_write_row(writer, row2, 3);
```

### `csvkit_writer_close()`

```c
void csvkit_writer_close(csvkit_writer_t *writer);
```

Closes the writer and flushes any buffered data.

**Parameters:**
- `writer`: Writer handle

### `csvkit_writer_free()`

```c
void csvkit_writer_free(csvkit_writer_t *writer);
```

Frees the writer and all associated resources.

**Parameters:**
- `writer`: Writer handle

### `csvkit_writer_get_error_msg()`

```c
const char *csvkit_writer_get_error_msg(csvkit_writer_t *writer);
```

Gets the last error message from the writer.

**Parameters:**
- `writer`: Writer handle

**Returns:** Error message string, or `NULL` if no error.

## Row API

### `csvkit_row_free()`

```c
void csvkit_row_free(csvkit_row_t *row);
```

Frees a row structure and all its fields.

**Parameters:**
- `row`: Row to free

**Example:**

```c
csvkit_row_t *row;
csvkit_read_row(parser, &row);
// Use row...
csvkit_row_free(row);
```

### `csvkit_row_get_field()`

```c
const char *csvkit_row_get_field(const csvkit_row_t *row, size_t index);
```

Gets a field value by index.

**Parameters:**
- `row`: Row structure
- `index`: Field index (0-based)

**Returns:** Field value, or `NULL` if index out of bounds.

**Example:**

```c
const char *name = csvkit_row_get_field(row, 0);
const char *age = csvkit_row_get_field(row, 1);
```

### `csvkit_row_field_count()`

```c
size_t csvkit_row_field_count(const csvkit_row_t *row);
```

Gets the number of fields in a row.

**Parameters:**
- `row`: Row structure

**Returns:** Number of fields.

### `csvkit_row_is_empty()`

```c
bool csvkit_row_is_empty(const csvkit_row_t *row);
```

Checks if a row is empty (has no fields).

**Parameters:**
- `row`: Row structure

**Returns:** `true` if row is empty, `false` otherwise.

## Error Handling

All functions that can fail return a `csvkit_error_t` code. Always check return values:

```c
csvkit_error_t err = csvkit_open_file(parser, "data.csv");
if (err != CSVKIT_OK) {
    const char *msg = csvkit_get_error_msg(parser);
    fprintf(stderr, "Error: %s\n", msg ? msg : "Unknown error");
    return 1;
}
```

Error codes:
- `CSVKIT_OK` (0): Success
- `CSVKIT_ERROR_MEMORY`: Memory allocation failed
- `CSVKIT_ERROR_IO`: File or I/O error
- `CSVKIT_ERROR_PARSE`: CSV parsing error
- `CSVKIT_ERROR_INVALID_ARG`: Invalid function argument
- `CSVKIT_ERROR_EOF`: End of file reached

## Examples

### Reading a CSV File

```c
#include <csvkit.h>
#include <stdio.h>

int main() {
    csvkit_parser_t *parser = csvkit_parser_new();
    if (!parser) {
        fprintf(stderr, "Failed to create parser\n");
        return 1;
    }

    csvkit_error_t err = csvkit_open_file(parser, "data.csv");
    if (err != CSVKIT_OK) {
        fprintf(stderr, "Error opening file: %s\n", csvkit_get_error_msg(parser));
        csvkit_parser_free(parser);
        return 1;
    }

    csvkit_row_t *row;
    while ((err = csvkit_read_row(parser, &row)) == CSVKIT_OK) {
        for (size_t i = 0; i < row->field_count; i++) {
            printf("%s%s", row->fields[i],
                   i < row->field_count - 1 ? "," : "\n");
        }
        csvkit_row_free(row);
    }

    if (err != CSVKIT_ERROR_EOF) {
        fprintf(stderr, "Error reading: %s\n", csvkit_get_error_msg(parser));
    }

    csvkit_close(parser);
    csvkit_parser_free(parser);
    return 0;
}
```

### Writing a CSV File

```c
#include <csvkit.h>
#include <stdio.h>

int main() {
    csvkit_writer_t *writer = csvkit_writer_new();
    if (!writer) {
        fprintf(stderr, "Failed to create writer\n");
        return 1;
    }

    csvkit_error_t err = csvkit_writer_open_file(writer, "output.csv");
    if (err != CSVKIT_OK) {
        fprintf(stderr, "Error: %s\n", csvkit_writer_get_error_msg(writer));
        csvkit_writer_free(writer);
        return 1;
    }

    const char *header[] = {"Name", "Age", "City"};
    csvkit_writer_write_row(writer, header, 3);

    const char *row1[] = {"John Doe", "30", "New York"};
    csvkit_writer_write_row(writer, row1, 3);

    const char *row2[] = {"Jane Smith", "25", "Los Angeles"};
    csvkit_writer_write_row(writer, row2, 3);

    csvkit_writer_close(writer);
    csvkit_writer_free(writer);
    return 0;
}
```

### Custom Configuration

```c
csvkit_config_t config = csvkit_config_default();
config.delimiter = '\t';           // Use tabs
config.quote_char = '\'';          // Single quotes
config.trim_whitespace = true;     // Trim whitespace
config.skip_empty_rows = true;     // Skip empty rows
config.strict_mode = true;         // RFC 4180 compliance

csvkit_parser_t *parser = csvkit_parser_new_with_config(&config);
```

### Parsing from String

```c
const char *csv_data =
    "Name,Age,City\n"
    "John,30,NYC\n"
    "Jane,25,LA\n";

csvkit_parser_t *parser = csvkit_parser_new();
csvkit_open_string(parser, csv_data, strlen(csv_data));

csvkit_row_t *row;
while (csvkit_read_row(parser, &row) == CSVKIT_OK) {
    printf("Row %zu: %zu fields\n", row->row_number, row->field_count);
    csvkit_row_free(row);
}

csvkit_parser_free(parser);
```

/*
 * libcsvkit - CSV parsing library for C
 * Copyright (C) 2025 AnmiTaliDev <anmitali198@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef CSVKIT_H
#define CSVKIT_H

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Version information */
#define CSVKIT_VERSION_MAJOR 0
#define CSVKIT_VERSION_MINOR 1
#define CSVKIT_VERSION_PATCH 0

/* CSV parser configuration */
typedef struct {
    char delimiter;          /* Field delimiter (default: ',') */
    char quote_char;        /* Quote character (default: '"') */
    char escape_char;       /* Escape character (default: '"') */
    bool trim_whitespace;   /* Trim leading/trailing whitespace */
    bool skip_empty_rows;   /* Skip rows with no data */
    bool strict_mode;       /* Strict RFC 4180 compliance */
} csvkit_config_t;

/* CSV row structure */
typedef struct {
    char **fields;          /* Array of field values */
    size_t field_count;     /* Number of fields */
    size_t row_number;      /* Row number in the file */
} csvkit_row_t;

/* CSV parser handle */
typedef struct csvkit_parser csvkit_parser_t;

/* Error codes */
typedef enum {
    CSVKIT_OK = 0,
    CSVKIT_ERROR_MEMORY,
    CSVKIT_ERROR_IO,
    CSVKIT_ERROR_PARSE,
    CSVKIT_ERROR_INVALID_ARG,
    CSVKIT_ERROR_EOF
} csvkit_error_t;

/*
 * Core API Functions
 */

/* Create a new CSV parser with default configuration */
csvkit_parser_t *csvkit_parser_new(void);

/* Create a new CSV parser with custom configuration */
csvkit_parser_t *csvkit_parser_new_with_config(const csvkit_config_t *config);

/* Get default configuration */
csvkit_config_t csvkit_config_default(void);

/* Free the parser and its resources */
void csvkit_parser_free(csvkit_parser_t *parser);

/* Open a CSV file for parsing */
csvkit_error_t csvkit_open_file(csvkit_parser_t *parser, const char *filename);

/* Open a CSV from FILE* stream */
csvkit_error_t csvkit_open_stream(csvkit_parser_t *parser, FILE *stream);

/* Parse from a string buffer */
csvkit_error_t csvkit_open_string(csvkit_parser_t *parser, const char *data, size_t len);

/* Read the next row from the CSV */
csvkit_error_t csvkit_read_row(csvkit_parser_t *parser, csvkit_row_t **row);

/* Free a row structure */
void csvkit_row_free(csvkit_row_t *row);

/* Close the current CSV source */
void csvkit_close(csvkit_parser_t *parser);

/* Get the last error message */
const char *csvkit_get_error_msg(csvkit_parser_t *parser);

/*
 * Convenience Functions
 */

/* Get field value by index (returns NULL if out of bounds) */
const char *csvkit_row_get_field(const csvkit_row_t *row, size_t index);

/* Get field count */
size_t csvkit_row_field_count(const csvkit_row_t *row);

/* Check if a row is empty */
bool csvkit_row_is_empty(const csvkit_row_t *row);

/*
 * Writer API
 */

typedef struct csvkit_writer csvkit_writer_t;

/* Create a new CSV writer */
csvkit_writer_t *csvkit_writer_new(void);

/* Create a new CSV writer with custom configuration */
csvkit_writer_t *csvkit_writer_new_with_config(const csvkit_config_t *config);

/* Open a file for writing */
csvkit_error_t csvkit_writer_open_file(csvkit_writer_t *writer, const char *filename);

/* Open a FILE* stream for writing */
csvkit_error_t csvkit_writer_open_stream(csvkit_writer_t *writer, FILE *stream);

/* Write a row to CSV */
csvkit_error_t csvkit_writer_write_row(csvkit_writer_t *writer, const char **fields, size_t field_count);

/* Close the writer */
void csvkit_writer_close(csvkit_writer_t *writer);

/* Free the writer */
void csvkit_writer_free(csvkit_writer_t *writer);

#ifdef __cplusplus
}
#endif

#endif /* CSVKIT_H */

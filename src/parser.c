/*
 * libcsvkit - CSV parsing library for C
 * Copyright (C) 2025 AnmiTaliDev <anmitali198@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 */

#define _POSIX_C_SOURCE 200809L

#include "csvkit.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define INITIAL_BUFFER_SIZE 1024
#define INITIAL_FIELD_COUNT 16

typedef enum {
    SOURCE_NONE,
    SOURCE_FILE,
    SOURCE_STREAM,
    SOURCE_STRING
} source_type_t;

struct csvkit_parser {
    csvkit_config_t config;
    source_type_t source_type;
    FILE *file;
    const char *string_data;
    size_t string_pos;
    size_t string_len;
    size_t row_number;
    char *error_msg;
    bool owns_file;
    size_t expected_field_count;  /* For strict mode */
};

/* Internal helper functions */
static char *string_duplicate(const char *str, size_t len) {
    char *result = malloc(len + 1);
    if (result) {
        memcpy(result, str, len);
        result[len] = '\0';
    }
    return result;
}

static void trim_whitespace_inplace(char *str) {
    if (!str || *str == '\0') return;

    /* Trim leading - find first non-space */
    char *start = str;
    while (isspace((unsigned char)*start)) start++;

    /* If all spaces, make empty string */
    if (*start == '\0') {
        *str = '\0';
        return;
    }

    /* Trim trailing - find last non-space (without strlen) */
    char *end = start;
    char *last_non_space = start;

    while (*end != '\0') {
        if (!isspace((unsigned char)*end)) {
            last_non_space = end;
        }
        end++;
    }

    /* Copy in-place if needed (without memmove) */
    if (start != str) {
        char *dest = str;
        char *src = start;
        while (src <= last_non_space) {
            *dest++ = *src++;
        }
        *dest = '\0';
    } else {
        /* Just null-terminate after last non-space */
        last_non_space[1] = '\0';
    }
}

static void set_error(csvkit_parser_t *parser, const char *msg) {
    free(parser->error_msg);
    parser->error_msg = msg ? strdup(msg) : NULL;
}

static int read_char(csvkit_parser_t *parser) {
    switch (parser->source_type) {
        case SOURCE_FILE:
        case SOURCE_STREAM:
            return fgetc(parser->file);
        case SOURCE_STRING:
            if (parser->string_pos >= parser->string_len) {
                return EOF;
            }
            return (unsigned char)parser->string_data[parser->string_pos++];
        default:
            return EOF;
    }
}

static void unread_char(csvkit_parser_t *parser, int c) {
    if (c == EOF) return;

    switch (parser->source_type) {
        case SOURCE_FILE:
        case SOURCE_STREAM:
            ungetc(c, parser->file);
            break;
        case SOURCE_STRING:
            if (parser->string_pos > 0) {
                parser->string_pos--;
            }
            break;
        default:
            break;
    }
}

/* Validate configuration for invalid combinations */
static bool validate_config(const csvkit_config_t *config) {
    if (!config) return false;

    /* Delimiter must not be a special character */
    if (config->delimiter == '\n' || config->delimiter == '\r') {
        return false;  /* Can't use line endings as delimiter */
    }

    /* In RFC 4180 mode, delimiter and quote must be different */
    if (config->escape_char == config->quote_char) {
        if (config->delimiter == config->quote_char) {
            return false;  /* Can't distinguish field separator from quote */
        }
    }

    /* Delimiter and quote should be different for clarity */
    if (config->delimiter == config->quote_char) {
        return false;
    }

    /* Quote char must not be newline */
    if (config->quote_char == '\n' || config->quote_char == '\r') {
        return false;
    }

    /* Escape char must not be newline */
    if (config->escape_char == '\n' || config->escape_char == '\r') {
        return false;
    }

    return true;
}

csvkit_config_t csvkit_config_default(void) {
    csvkit_config_t config = {
        .delimiter = ',',
        .quote_char = '"',
        .escape_char = '"',
        .trim_whitespace = false,
        .skip_empty_rows = false,
        .strict_mode = false
    };
    return config;
}

csvkit_parser_t *csvkit_parser_new(void) {
    csvkit_config_t config = csvkit_config_default();
    return csvkit_parser_new_with_config(&config);
}

csvkit_parser_t *csvkit_parser_new_with_config(const csvkit_config_t *config) {
    if (!config || !validate_config(config)) {
        return NULL;  /* Invalid configuration */
    }

    csvkit_parser_t *parser = calloc(1, sizeof(csvkit_parser_t));
    if (!parser) return NULL;

    parser->config = *config;
    parser->source_type = SOURCE_NONE;
    parser->row_number = 0;
    parser->error_msg = NULL;

    return parser;
}

void csvkit_parser_free(csvkit_parser_t *parser) {
    if (!parser) return;

    csvkit_close(parser);
    free(parser->error_msg);
    free(parser);
}

csvkit_error_t csvkit_open_file(csvkit_parser_t *parser, const char *filename) {
    if (!parser || !filename) return CSVKIT_ERROR_INVALID_ARG;

    csvkit_close(parser);

    FILE *file = fopen(filename, "r");
    if (!file) {
        set_error(parser, strerror(errno));
        return CSVKIT_ERROR_IO;
    }

    parser->file = file;
    parser->source_type = SOURCE_FILE;
    parser->owns_file = true;
    parser->row_number = 0;
    parser->expected_field_count = 0;

    return CSVKIT_OK;
}

csvkit_error_t csvkit_open_stream(csvkit_parser_t *parser, FILE *stream) {
    if (!parser || !stream) return CSVKIT_ERROR_INVALID_ARG;

    csvkit_close(parser);

    parser->file = stream;
    parser->source_type = SOURCE_STREAM;
    parser->owns_file = false;
    parser->row_number = 0;
    parser->expected_field_count = 0;

    return CSVKIT_OK;
}

csvkit_error_t csvkit_open_string(csvkit_parser_t *parser, const char *data, size_t len) {
    if (!parser || !data) return CSVKIT_ERROR_INVALID_ARG;

    csvkit_close(parser);

    parser->string_data = data;
    parser->string_pos = 0;
    parser->string_len = len;
    parser->source_type = SOURCE_STRING;
    parser->row_number = 0;
    parser->expected_field_count = 0;

    return CSVKIT_OK;
}

void csvkit_close(csvkit_parser_t *parser) {
    if (!parser) return;

    if (parser->owns_file && parser->file) {
        fclose(parser->file);
    }

    parser->file = NULL;
    parser->source_type = SOURCE_NONE;
    parser->string_data = NULL;
    parser->string_pos = 0;
    parser->string_len = 0;
}

/* Internal helper to parse a single row */
static csvkit_error_t parse_row_internal(
    csvkit_parser_t *parser,
    csvkit_row_t *row,
    char **buffer,
    size_t *buffer_capacity,
    size_t *fields_capacity
) {
    size_t buffer_len = 0;
    bool in_quotes = false;
    bool field_started = false;
    bool field_was_quoted = false;
    int c;

    while ((c = read_char(parser)) != EOF) {
        /* Handle CRLF outside quoted fields only */
        if (!in_quotes && c == '\r') {
            /* Skip CR, handle CRLF */
            int next = read_char(parser);
            if (next != '\n' && next != EOF) {
                unread_char(parser, next);
            }
            c = '\n';
        }

        if (!in_quotes) {
            if (c == parser->config.quote_char && !field_started) {
                /* Start of quoted field */
                in_quotes = true;
                field_started = true;
                field_was_quoted = true;
                continue;
            } else if (c == parser->config.delimiter) {
                /* End of field */
                char *field_value = string_duplicate(*buffer, buffer_len);
                if (!field_value) {
                    return CSVKIT_ERROR_MEMORY;
                }

                if (parser->config.trim_whitespace && !field_was_quoted) {
                    trim_whitespace_inplace(field_value);
                }

                /* Resize fields array if needed */
                if (row->field_count >= *fields_capacity) {
                    *fields_capacity *= 2;
                    char **new_fields = realloc(row->fields, *fields_capacity * sizeof(char *));
                    if (!new_fields) {
                        free(field_value);
                        return CSVKIT_ERROR_MEMORY;
                    }
                    row->fields = new_fields;
                }

                row->fields[row->field_count++] = field_value;
                buffer_len = 0;
                field_started = false;
                field_was_quoted = false;
                continue;
            } else if (c == '\n') {
                /* End of row */
                break;
            }
        } else {
            /* Inside quotes */
            if (c == parser->config.escape_char) {
                /* Escape character found - read next character */
                int next = read_char(parser);
                if (next == parser->config.quote_char) {
                    /* Escaped quote: escape_char + quote becomes quote */
                    c = parser->config.quote_char;
                } else if (next == parser->config.escape_char) {
                    /* Escaped escape character */
                    c = parser->config.escape_char;
                } else if (next != EOF) {
                    /* If escape_char == quote_char (RFC 4180 mode), treat as end of field */
                    if (parser->config.escape_char == parser->config.quote_char) {
                        /* This is end of quoted field */

                        /* In strict mode, verify no characters after closing quote */
                        if (parser->config.strict_mode) {
                            if (next != parser->config.delimiter && next != '\n' && next != '\r') {
                                /* Invalid character after closing quote */
                                return CSVKIT_ERROR_PARSE;
                            }
                        }

                        unread_char(parser, next);
                        in_quotes = false;
                        continue;
                    } else {
                        /* Other escaped characters - add both escape and next char */
                        /* First add escape char to buffer */
                        if (buffer_len >= *buffer_capacity - 1) {
                            *buffer_capacity *= 2;
                            char *new_buffer = realloc(*buffer, *buffer_capacity);
                            if (!new_buffer) {
                                return CSVKIT_ERROR_MEMORY;
                            }
                            *buffer = new_buffer;
                        }
                        (*buffer)[buffer_len++] = c;

                        /* Now set c to next char so it will be added in the next section */
                        c = next;
                        field_started = true;
                    }
                } else {
                    /* EOF after escape character */
                    break;
                }
            } else if (c == parser->config.quote_char && parser->config.escape_char != parser->config.quote_char) {
                /* Unescaped quote character (only when escape_char != quote_char) */
                /* This is end of quoted field */
                in_quotes = false;

                /* In strict mode, verify no characters after closing quote */
                if (parser->config.strict_mode) {
                    int next = read_char(parser);
                    if (next != EOF && next != parser->config.delimiter && next != '\n' && next != '\r') {
                        /* Invalid character after closing quote */
                        return CSVKIT_ERROR_PARSE;
                    }
                    unread_char(parser, next);
                }
                continue;
            }
        }

        /* Add character to buffer */
        if (buffer_len >= *buffer_capacity - 1) {
            *buffer_capacity *= 2;
            char *new_buffer = realloc(*buffer, *buffer_capacity);
            if (!new_buffer) {
                return CSVKIT_ERROR_MEMORY;
            }
            *buffer = new_buffer;
        }

        (*buffer)[buffer_len++] = c;
        field_started = true;
    }

    /* Handle last field */
    if (c == EOF && buffer_len == 0 && row->field_count == 0 && !field_started) {
        return CSVKIT_ERROR_EOF;
    }

    /* Check for unclosed quoted field */
    if (in_quotes) {
        return CSVKIT_ERROR_PARSE;
    }

    /* Add last field */
    char *field_value = string_duplicate(*buffer, buffer_len);
    if (!field_value) {
        return CSVKIT_ERROR_MEMORY;
    }

    if (parser->config.trim_whitespace && !field_was_quoted) {
        trim_whitespace_inplace(field_value);
    }

    if (row->field_count >= *fields_capacity) {
        (*fields_capacity)++;
        char **new_fields = realloc(row->fields, *fields_capacity * sizeof(char *));
        if (!new_fields) {
            free(field_value);
            return CSVKIT_ERROR_MEMORY;
        }
        row->fields = new_fields;
    }

    row->fields[row->field_count++] = field_value;
    return CSVKIT_OK;
}

csvkit_error_t csvkit_read_row(csvkit_parser_t *parser, csvkit_row_t **out_row) {
    if (!parser || !out_row) return CSVKIT_ERROR_INVALID_ARG;
    if (parser->source_type == SOURCE_NONE) return CSVKIT_ERROR_INVALID_ARG;

    *out_row = NULL;

    csvkit_row_t *row;
    size_t fields_capacity;
    size_t buffer_capacity;
    char *buffer;
    csvkit_error_t result;

read_next_row:
    /* Allocate row structure */
    row = calloc(1, sizeof(csvkit_row_t));
    if (!row) {
        set_error(parser, "Out of memory");
        return CSVKIT_ERROR_MEMORY;
    }

    /* Allocate initial fields array */
    fields_capacity = INITIAL_FIELD_COUNT;
    row->fields = malloc(fields_capacity * sizeof(char *));
    if (!row->fields) {
        free(row);
        set_error(parser, "Out of memory");
        return CSVKIT_ERROR_MEMORY;
    }

    /* Allocate field buffer */
    buffer_capacity = INITIAL_BUFFER_SIZE;
    buffer = malloc(buffer_capacity);
    if (!buffer) {
        free(row->fields);
        free(row);
        set_error(parser, "Out of memory");
        return CSVKIT_ERROR_MEMORY;
    }

    /* Parse the row using the internal helper */
    result = parse_row_internal(parser, row, &buffer, &buffer_capacity, &fields_capacity);

    free(buffer);

    if (result != CSVKIT_OK) {
        /* Clean up on error */
        for (size_t i = 0; i < row->field_count; i++) {
            free(row->fields[i]);
        }
        free(row->fields);
        free(row);

        if (result == CSVKIT_ERROR_PARSE) {
            set_error(parser, "Unclosed quoted field");
        } else if (result == CSVKIT_ERROR_MEMORY) {
            set_error(parser, "Out of memory");
        }

        return result;
    }

    /* Update row number */
    parser->row_number++;
    row->row_number = parser->row_number;

    /* Skip empty rows if configured */
    if (parser->config.skip_empty_rows && csvkit_row_is_empty(row)) {
        csvkit_row_free(row);
        goto read_next_row;
    }

    /* Strict mode: check field count consistency */
    if (parser->config.strict_mode) {
        if (parser->expected_field_count == 0) {
            /* First row - set expected count */
            parser->expected_field_count = row->field_count;
        } else if (row->field_count != parser->expected_field_count) {
            /* Field count mismatch */
            csvkit_row_free(row);
            set_error(parser, "Field count mismatch in strict mode");
            return CSVKIT_ERROR_PARSE;
        }
    }

    *out_row = row;
    return CSVKIT_OK;
}

void csvkit_row_free(csvkit_row_t *row) {
    if (!row) return;

    for (size_t i = 0; i < row->field_count; i++) {
        free(row->fields[i]);
    }
    free(row->fields);
    free(row);
}

const char *csvkit_get_error_msg(csvkit_parser_t *parser) {
    return parser ? parser->error_msg : NULL;
}

const char *csvkit_row_get_field(const csvkit_row_t *row, size_t index) {
    if (!row || index >= row->field_count) return NULL;
    return row->fields[index];
}

size_t csvkit_row_field_count(const csvkit_row_t *row) {
    return row ? row->field_count : 0;
}

bool csvkit_row_is_empty(const csvkit_row_t *row) {
    if (!row || row->field_count == 0) return true;

    for (size_t i = 0; i < row->field_count; i++) {
        if (row->fields[i] && row->fields[i][0] != '\0') {
            return false;
        }
    }

    return true;
}

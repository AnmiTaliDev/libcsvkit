/*
 * libcsvkit - CSV writing library for C
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

struct csvkit_writer {
    csvkit_config_t config;
    FILE *file;
    bool owns_file;
    char *error_msg;
};

static void set_error(csvkit_writer_t *writer, const char *msg) {
    free(writer->error_msg);
    writer->error_msg = msg ? strdup(msg) : NULL;
}

/* Validate configuration for invalid combinations */
static bool validate_config(const csvkit_config_t *config) {
    if (!config) return false;

    /* Delimiter must not be a special character */
    if (config->delimiter == '\n' || config->delimiter == '\r') {
        return false;
    }

    /* Delimiter and quote should be different */
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

static bool needs_quoting(const char *field, char delimiter, char quote_char) {
    if (!field) return false;

    for (const char *p = field; *p; p++) {
        if (*p == delimiter || *p == quote_char || *p == '\n' || *p == '\r') {
            return true;
        }
    }

    return false;
}

csvkit_writer_t *csvkit_writer_new(void) {
    csvkit_config_t config = csvkit_config_default();
    return csvkit_writer_new_with_config(&config);
}

csvkit_writer_t *csvkit_writer_new_with_config(const csvkit_config_t *config) {
    if (!config || !validate_config(config)) {
        return NULL;  /* Invalid configuration */
    }

    csvkit_writer_t *writer = calloc(1, sizeof(csvkit_writer_t));
    if (!writer) return NULL;

    writer->config = *config;
    writer->file = NULL;
    writer->owns_file = false;
    writer->error_msg = NULL;

    return writer;
}

csvkit_error_t csvkit_writer_open_file(csvkit_writer_t *writer, const char *filename) {
    if (!writer || !filename) return CSVKIT_ERROR_INVALID_ARG;

    csvkit_writer_close(writer);

    FILE *file = fopen(filename, "w");
    if (!file) {
        set_error(writer, "Failed to open file for writing");
        return CSVKIT_ERROR_IO;
    }

    writer->file = file;
    writer->owns_file = true;

    return CSVKIT_OK;
}

csvkit_error_t csvkit_writer_open_stream(csvkit_writer_t *writer, FILE *stream) {
    if (!writer || !stream) return CSVKIT_ERROR_INVALID_ARG;

    csvkit_writer_close(writer);

    writer->file = stream;
    writer->owns_file = false;

    return CSVKIT_OK;
}

csvkit_error_t csvkit_writer_write_row(csvkit_writer_t *writer, const char **fields, size_t field_count) {
    if (!writer || !writer->file) return CSVKIT_ERROR_INVALID_ARG;
    if (!fields && field_count > 0) return CSVKIT_ERROR_INVALID_ARG;

    for (size_t i = 0; i < field_count; i++) {
        if (i > 0) {
            if (fputc(writer->config.delimiter, writer->file) == EOF) {
                set_error(writer, "Write error");
                return CSVKIT_ERROR_IO;
            }
        }

        const char *field = fields[i] ? fields[i] : "";

        if (needs_quoting(field, writer->config.delimiter, writer->config.quote_char)) {
            /* Write quoted field */
            if (fputc(writer->config.quote_char, writer->file) == EOF) {
                set_error(writer, "Write error");
                return CSVKIT_ERROR_IO;
            }

            for (const char *p = field; *p; p++) {
                if (*p == writer->config.quote_char) {
                    /* Escape quote character */
                    if (fputc(writer->config.escape_char, writer->file) == EOF ||
                        fputc(writer->config.quote_char, writer->file) == EOF) {
                        set_error(writer, "Write error");
                        return CSVKIT_ERROR_IO;
                    }
                } else {
                    if (fputc(*p, writer->file) == EOF) {
                        set_error(writer, "Write error");
                        return CSVKIT_ERROR_IO;
                    }
                }
            }

            if (fputc(writer->config.quote_char, writer->file) == EOF) {
                set_error(writer, "Write error");
                return CSVKIT_ERROR_IO;
            }
        } else {
            /* Write unquoted field */
            if (fputs(field, writer->file) == EOF) {
                set_error(writer, "Write error");
                return CSVKIT_ERROR_IO;
            }
        }
    }

    /* Write newline */
    if (fputc('\n', writer->file) == EOF) {
        set_error(writer, "Write error");
        return CSVKIT_ERROR_IO;
    }

    return CSVKIT_OK;
}

void csvkit_writer_close(csvkit_writer_t *writer) {
    if (!writer) return;

    if (writer->owns_file && writer->file) {
        fclose(writer->file);
    }

    writer->file = NULL;
    writer->owns_file = false;
}

void csvkit_writer_free(csvkit_writer_t *writer) {
    if (!writer) return;

    csvkit_writer_close(writer);
    free(writer->error_msg);
    free(writer);
}

const char *csvkit_writer_get_error_msg(csvkit_writer_t *writer) {
    if (!writer) return NULL;
    return writer->error_msg;
}

/*
 * libcsvkit - C++ wrapper for CSV parsing library
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

#ifndef CSVKIT_HPP
#define CSVKIT_HPP

#include <csvkit.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <memory>

namespace csvkit {

/**
 * Exception class for CSV parsing errors
 */
class Exception : public std::runtime_error {
public:
    explicit Exception(const std::string& msg);
    explicit Exception(csvkit_error_t err);

private:
    static std::string error_to_string(csvkit_error_t err);
};

/**
 * Configuration for CSV parsing/writing
 */
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

private:
    csvkit_config_t config_;
};

/**
 * CSV Row - represents a single row of CSV data
 */
class Row {
public:
    explicit Row(csvkit_row_t* row);
    ~Row();

    Row(const Row&) = delete;
    Row& operator=(const Row&) = delete;

    Row(Row&& other) noexcept;
    Row& operator=(Row&& other) noexcept;

    // Access fields
    const std::string& operator[](size_t index) const;
    const std::string& at(size_t index) const;

    size_t size() const;
    size_t field_count() const;
    size_t row_number() const;
    bool empty() const;

    // Iterators
    std::vector<std::string>::const_iterator begin() const;
    std::vector<std::string>::const_iterator end() const;

    // Get all fields as vector
    const std::vector<std::string>& fields() const;

private:
    csvkit_row_t* row_;
    std::vector<std::string> fields_;
    size_t row_number_;
};

/**
 * CSV Parser - reads CSV data from files, streams, or strings
 */
class Parser {
public:
    Parser();
    explicit Parser(const Config& config);
    ~Parser();

    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;

    Parser(Parser&& other) noexcept;
    Parser& operator=(Parser&& other) noexcept;

    // Open CSV from file
    void open(const std::string& filename);

    // Open CSV from FILE* stream
    void open(FILE* stream);

    // Open CSV from string
    void open_string(const std::string& data);

    // Read next row
    std::unique_ptr<Row> read_row();

    // Read all rows
    std::vector<Row> read_all();

    // Close current source
    void close();

    // Get error message
    std::string get_error_message() const;

    // Iterator support for range-based for loops
    class Iterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = Row;
        using difference_type = std::ptrdiff_t;
        using pointer = Row*;
        using reference = Row&;

        Iterator(Parser* parser, bool is_end = false);

        Iterator& operator++();
        Row& operator*();
        Row* operator->();
        bool operator!=(const Iterator& other) const;

    private:
        void advance();  // Helper to read next row

        Parser* parser_;
        std::unique_ptr<Row> current_;
        bool is_end_;
        bool initialized_;  // Track if first read happened
    };

    Iterator begin();
    Iterator end();

private:
    csvkit_parser_t* parser_;
};

/**
 * CSV Writer - writes CSV data to files or streams
 */
class Writer {
public:
    Writer();
    explicit Writer(const Config& config);
    ~Writer();

    Writer(const Writer&) = delete;
    Writer& operator=(const Writer&) = delete;

    Writer(Writer&& other) noexcept;
    Writer& operator=(Writer&& other) noexcept;

    // Open file for writing
    void open(const std::string& filename);

    // Open FILE* stream for writing
    void open(FILE* stream);

    // Write a row from vector of strings
    void write_row(const std::vector<std::string>& fields);

    // Write a row from initializer list
    void write_row(std::initializer_list<std::string> fields);

    // Write a row from C-style array
    void write_row(const char** fields, size_t count);

    // Close writer
    void close();

    // Get error message
    std::string get_error_message() const;

private:
    csvkit_writer_t* writer_;
};

// Helper functions
std::vector<Row> read_file(const std::string& filename, const Config& config = Config());
std::vector<Row> read_string(const std::string& data, const Config& config = Config());

} // namespace csvkit

#endif // CSVKIT_HPP

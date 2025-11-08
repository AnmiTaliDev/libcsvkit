/*
 * libcsvkit - C++ wrapper implementation
 * Copyright (C) 2025 AnmiTaliDev <anmitali198@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 */

#include "csvkit.hpp"
#include <cstring>

namespace csvkit {

// ============================================================================
// Exception
// ============================================================================

Exception::Exception(const std::string& msg) : std::runtime_error(msg) {}

Exception::Exception(csvkit_error_t err) : std::runtime_error(error_to_string(err)) {}

std::string Exception::error_to_string(csvkit_error_t err) {
    switch (err) {
        case CSVKIT_OK: return "No error";
        case CSVKIT_ERROR_MEMORY: return "Memory allocation error";
        case CSVKIT_ERROR_IO: return "I/O error";
        case CSVKIT_ERROR_PARSE: return "Parse error";
        case CSVKIT_ERROR_INVALID_ARG: return "Invalid argument";
        case CSVKIT_ERROR_EOF: return "End of file";
        default: return "Unknown error";
    }
}

// ============================================================================
// Config
// ============================================================================

Config::Config() : config_(csvkit_config_default()) {}

Config& Config::delimiter(char delim) {
    config_.delimiter = delim;
    return *this;
}

Config& Config::quote_char(char quote) {
    config_.quote_char = quote;
    return *this;
}

Config& Config::escape_char(char escape) {
    config_.escape_char = escape;
    return *this;
}

Config& Config::trim_whitespace(bool trim) {
    config_.trim_whitespace = trim;
    return *this;
}

Config& Config::skip_empty_rows(bool skip) {
    config_.skip_empty_rows = skip;
    return *this;
}

Config& Config::strict_mode(bool strict) {
    config_.strict_mode = strict;
    return *this;
}

const csvkit_config_t& Config::get() const {
    return config_;
}

// ============================================================================
// Row
// ============================================================================

Row::Row(csvkit_row_t* row) : row_(nullptr), row_number_(0) {
    if (!row) {
        throw Exception("Null row pointer");
    }

    // Copy fields and row number, then free the C structure
    // Use try-catch to ensure row is freed if exception occurs
    try {
        row_number_ = row->row_number;
        fields_.reserve(row->field_count);
        for (size_t i = 0; i < row->field_count; ++i) {
            fields_.emplace_back(row->fields[i] ? row->fields[i] : "");
        }
        // Free immediately after copying - we don't need it anymore
        csvkit_row_free(row);
    } catch (...) {
        // Free the row before re-throwing
        csvkit_row_free(row);
        throw;
    }
}

Row::~Row() {
    if (row_) {
        csvkit_row_free(row_);
    }
}

Row::Row(Row&& other) noexcept
    : row_(other.row_), fields_(std::move(other.fields_)), row_number_(other.row_number_) {
    other.row_ = nullptr;
    other.row_number_ = 0;
}

Row& Row::operator=(Row&& other) noexcept {
    if (this != &other) {
        if (row_) {
            csvkit_row_free(row_);
        }
        row_ = other.row_;
        fields_ = std::move(other.fields_);
        row_number_ = other.row_number_;
        other.row_ = nullptr;
        other.row_number_ = 0;
    }
    return *this;
}

const std::string& Row::operator[](size_t index) const {
    // No bounds checking - follows STL convention for operator[]
    return fields_[index];
}

const std::string& Row::at(size_t index) const {
    if (index >= fields_.size()) {
        throw std::out_of_range("Field index out of range");
    }
    return fields_[index];
}

size_t Row::size() const {
    return fields_.size();
}

size_t Row::field_count() const {
    return fields_.size();
}

size_t Row::row_number() const {
    return row_number_;
}

bool Row::empty() const {
    return fields_.empty();
}

std::vector<std::string>::const_iterator Row::begin() const {
    return fields_.begin();
}

std::vector<std::string>::const_iterator Row::end() const {
    return fields_.end();
}

const std::vector<std::string>& Row::fields() const {
    return fields_;
}

// ============================================================================
// Parser
// ============================================================================

Parser::Parser() : parser_(csvkit_parser_new()) {
    if (!parser_) {
        throw Exception("Failed to create parser");
    }
}

Parser::Parser(const Config& config)
    : parser_(csvkit_parser_new_with_config(&config.get())) {
    if (!parser_) {
        throw Exception("Failed to create parser with config");
    }
}

Parser::~Parser() {
    if (parser_) {
        csvkit_parser_free(parser_);
    }
}

Parser::Parser(Parser&& other) noexcept : parser_(other.parser_) {
    other.parser_ = nullptr;
}

Parser& Parser::operator=(Parser&& other) noexcept {
    if (this != &other) {
        if (parser_) {
            csvkit_parser_free(parser_);
        }
        parser_ = other.parser_;
        other.parser_ = nullptr;
    }
    return *this;
}

void Parser::open(const std::string& filename) {
    csvkit_error_t err = csvkit_open_file(parser_, filename.c_str());
    if (err != CSVKIT_OK) {
        throw Exception(get_error_message());
    }
}

void Parser::open(FILE* stream) {
    csvkit_error_t err = csvkit_open_stream(parser_, stream);
    if (err != CSVKIT_OK) {
        throw Exception(get_error_message());
    }
}

void Parser::open_string(const std::string& data) {
    csvkit_error_t err = csvkit_open_string(parser_, data.c_str(), data.size());
    if (err != CSVKIT_OK) {
        throw Exception(get_error_message());
    }
}

std::unique_ptr<Row> Parser::read_row() {
    csvkit_row_t* row = nullptr;
    csvkit_error_t err = csvkit_read_row(parser_, &row);

    if (err == CSVKIT_ERROR_EOF) {
        return nullptr;
    }

    if (err != CSVKIT_OK) {
        throw Exception(get_error_message());
    }

    return std::unique_ptr<Row>(new Row(row));
}

std::vector<Row> Parser::read_all() {
    std::vector<Row> rows;
    while (auto row = read_row()) {
        rows.push_back(std::move(*row));
    }
    return rows;
}

void Parser::close() {
    csvkit_close(parser_);
}

std::string Parser::get_error_message() const {
    const char* msg = csvkit_get_error_msg(parser_);
    return msg ? std::string(msg) : "Unknown error";
}

Parser::Iterator Parser::begin() {
    return Iterator(this);
}

Parser::Iterator Parser::end() {
    return Iterator(this, true);
}

// ============================================================================
// Parser::Iterator
// ============================================================================

Parser::Iterator::Iterator(Parser* parser, bool is_end)
    : parser_(parser), is_end_(is_end), initialized_(false) {
    // Don't read anything in constructor - lazy evaluation
}

void Parser::Iterator::advance() {
    if (!initialized_) {
        initialized_ = true;
    }
    if (parser_ && !is_end_) {
        current_ = parser_->read_row();
        if (!current_) {
            is_end_ = true;
        }
    }
}

Parser::Iterator& Parser::Iterator::operator++() {
    advance();
    return *this;
}

Row& Parser::Iterator::operator*() {
    if (!initialized_) {
        advance();
    }
    return *current_;
}

Row* Parser::Iterator::operator->() {
    if (!initialized_) {
        advance();
    }
    return current_.get();
}

bool Parser::Iterator::operator!=(const Iterator& other) const {
    // Need to ensure both iterators are initialized for proper comparison
    if (!initialized_ && !is_end_ && parser_) {
        const_cast<Iterator*>(this)->advance();
    }
    return is_end_ != other.is_end_;
}

// ============================================================================
// Writer
// ============================================================================

Writer::Writer() : writer_(csvkit_writer_new()) {
    if (!writer_) {
        throw Exception("Failed to create writer");
    }
}

Writer::Writer(const Config& config)
    : writer_(csvkit_writer_new_with_config(&config.get())) {
    if (!writer_) {
        throw Exception("Failed to create writer with config");
    }
}

Writer::~Writer() {
    if (writer_) {
        csvkit_writer_free(writer_);
    }
}

Writer::Writer(Writer&& other) noexcept : writer_(other.writer_) {
    other.writer_ = nullptr;
}

Writer& Writer::operator=(Writer&& other) noexcept {
    if (this != &other) {
        if (writer_) {
            csvkit_writer_free(writer_);
        }
        writer_ = other.writer_;
        other.writer_ = nullptr;
    }
    return *this;
}

void Writer::open(const std::string& filename) {
    csvkit_error_t err = csvkit_writer_open_file(writer_, filename.c_str());
    if (err != CSVKIT_OK) {
        throw Exception(get_error_message());
    }
}

void Writer::open(FILE* stream) {
    csvkit_error_t err = csvkit_writer_open_stream(writer_, stream);
    if (err != CSVKIT_OK) {
        throw Exception(get_error_message());
    }
}

void Writer::write_row(const std::vector<std::string>& fields) {
    std::vector<const char*> c_fields;
    c_fields.reserve(fields.size());
    for (const auto& field : fields) {
        c_fields.push_back(field.c_str());
    }

    csvkit_error_t err = csvkit_writer_write_row(
        writer_, c_fields.data(), c_fields.size()
    );

    if (err != CSVKIT_OK) {
        throw Exception(get_error_message());
    }
}

void Writer::write_row(std::initializer_list<std::string> fields) {
    write_row(std::vector<std::string>(fields));
}

void Writer::write_row(const char** fields, size_t count) {
    csvkit_error_t err = csvkit_writer_write_row(writer_, fields, count);
    if (err != CSVKIT_OK) {
        throw Exception(get_error_message());
    }
}

void Writer::close() {
    csvkit_writer_close(writer_);
}

std::string Writer::get_error_message() const {
    const char* msg = csvkit_writer_get_error_msg(writer_);
    return msg ? std::string(msg) : "Unknown error";
}

// ============================================================================
// Helper functions
// ============================================================================

std::vector<Row> read_file(const std::string& filename, const Config& config) {
    Parser parser(config);
    parser.open(filename);
    return parser.read_all();
}

std::vector<Row> read_string(const std::string& data, const Config& config) {
    Parser parser(config);
    parser.open_string(data);
    return parser.read_all();
}

} // namespace csvkit

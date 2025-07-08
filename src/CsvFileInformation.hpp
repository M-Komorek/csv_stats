#pragma once

#include <optional>
#include <vector>
#include <string>

struct CsvFileInformation
{
    const std::string filePath_;
    uintmax_t fileSize_ = 0;
    size_t headerEndPosition_ = 0;
    size_t numOfValueColumns_ = 0;
    std::vector<std::string> columnNames_;
};

auto generateCsvFileInformation(std::string filepath) -> std::optional<CsvFileInformation>;
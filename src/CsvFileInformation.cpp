#include <iostream>
#include <filesystem>
#include <fstream>

#include "CsvFileInformation.hpp"
#include "ToolKit.hpp"

auto generateCsvFileInformation(std::string filepath) -> std::optional<CsvFileInformation>
{
    auto file = std::ifstream{filepath};
    if (not file) 
    {
        std::cerr << "Error: Cannot open file '" << filepath << "'" << std::endl;
        return std::nullopt;
    }

    auto headerLine = std::string{};
    if (not std::getline(file, headerLine)) 
    {
        std::cerr << "Error: Cannot read header or file is empty." << std::endl;
        return std::nullopt;
    }
    if (not headerLine.empty() && headerLine.back() == '\r')
    {
        headerLine.pop_back();
    }

    const auto headers = splitLine(headerLine, ',');
    if (headers.size() < 2)
    {
        std::cerr << "Error: CSV must have an ID and at least one value column." << std::endl;
        return std::nullopt;
    }

    auto columnNames = std::vector<std::string>(headers.begin() + 1, headers.end());
    const auto headerEndPosition = static_cast<size_t>(file.tellg());
    auto fileSize = uintmax_t{};

    try 
    {
        fileSize = std::filesystem::file_size(filepath);
    } 
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << "Error getting file size: " << e.what() << std::endl;
        return std::nullopt;
    }
    
    return CsvFileInformation{
        std::move(filepath),
        fileSize,
        headerEndPosition,
        columnNames.size(),
        std::move(columnNames)
    };
}
#include <iostream>
#include <stdexcept>

#include "ToolKit.hpp"

auto splitLine(std::string_view line, char delimiter) -> std::vector<std::string_view> 
{
    auto result = std::vector<std::string_view>{};
    auto start = size_t{0};
    while (start < line.length()) 
    {
        auto end = line.find(delimiter, start);
        if (end == std::string_view::npos) 
        {
            result.push_back(line.substr(start));
            break;
        }
        result.emplace_back(line.substr(start, end - start));
        start = end + 1;
    }

    return result;
}

auto calculateMedian(const Histogram& histogram, size_t datasetSize) -> double
{
    if (datasetSize == 0)
    {
        throw std::runtime_error("calculateMedian: totalCount can not be zero; corrupted data.");
    };

    auto firstMedianPos = (datasetSize - 1) / 2;
    auto medianValue = double{0.0};
    auto itemsSeen = size_t{0};

    auto it = histogram.begin();
    while (it != histogram.end()) 
    {
        itemsSeen += it->second;
        if (itemsSeen > firstMedianPos) 
        {
            medianValue = it->first;
            break;
        }
        ++it;
    }

    if (datasetSize % 2 != 0) 
    {
        return medianValue;
    }

    size_t secondMedianPos = datasetSize / 2;
    if (secondMedianPos < itemsSeen) 
    {
        return medianValue;
    }
    
    ++it;
    if (it != histogram.end()) 
    {
        return (medianValue + it->first) / 2.0;
    }

    throw std::runtime_error("calculateMedian: Histogram does not match dataset size; corrupted data.");
}

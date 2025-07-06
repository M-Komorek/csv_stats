#pragma once

#include <vector>

#include "ToolKit.hpp"
#include "ColumnStats.hpp"

struct ThreadLocalResults 
{
    explicit ThreadLocalResults(size_t numOfColumns);
    
    std::vector<double> mins_;
    std::vector<double> maxs_;
    std::vector<double> sums_;
    std::vector<size_t> counts_;
    std::vector<Histogram> histograms_;

    auto parseAndUpdate(std::string_view line, size_t numOfValueColumns) -> void;
};

auto aggregateResults(
    const std::vector<ThreadLocalResults>& allThreadResults,
    const std::vector<std::string>& columnNames) -> std::vector<ColumnStats>;

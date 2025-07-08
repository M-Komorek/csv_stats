#pragma once

#include <limits>
#include <string>
#include <vector>

struct ColumnStats
{
    std::string name_;
    double min_ = std::numeric_limits<double>::max();
    double max_ = std::numeric_limits<double>::lowest();
    double mean_ = 0.0;
    double median_ = 0.0;
    size_t count_ = 0;
};

auto prettyPrint(const ColumnStats& columnStats) -> void;
auto prettyPrint(const std::vector<ColumnStats>& columnStatsCollection) -> void;

#include <iostream>
#include <iomanip>

#include "ColumnStats.hpp"

auto prettyPrint(const ColumnStats& columnStats) -> void
{
    std::cout << "Column '" << columnStats.name_ << "':\n";
    if (columnStats.count_ > 0) {
        std::cout << "  Count:  " << columnStats.count_ << "\n";
        std::cout << "  Min:    " << columnStats.min_ << "\n";
        std::cout << "  Max:    " << columnStats.max_ << "\n";
        std::cout << "  Mean:   " << columnStats.mean_ << "\n";
        std::cout << "  Median: " << columnStats.median_ << "\n\n";
    } else {
        std::cout << "  (No valid data found)\n\n";
    }
}

auto prettyPrint(const std::vector<ColumnStats>& columnStatsCollection) -> void
{
    std::cout << "\n--- Statistics Summary ---\n";
    std::cout << std::fixed << std::setprecision(4);
    for (const auto& columnStats : columnStatsCollection) 
    {
        prettyPrint(columnStats);
    }
}
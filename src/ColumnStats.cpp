#include <iostream>
#include <iomanip>

#include "ColumnStats.hpp"

auto ColumnStats::prettyPrint() const -> void
{
    std::cout << "Column '" << name_ << "':\n";
    if (count_ > 0) {
        std::cout << "  Count:  " << count_ << "\n";
        std::cout << "  Min:    " << min_ << "\n";
        std::cout << "  Max:    " << max_ << "\n";
        std::cout << "  Mean:   " << mean_ << "\n";
        std::cout << "  Median: " << median_ << "\n\n";
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
        columnStats.prettyPrint();
    }
}
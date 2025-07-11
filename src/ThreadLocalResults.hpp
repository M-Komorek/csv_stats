#pragma once

#include <vector>

#include "ColumnStats.hpp"
#include "ToolKit.hpp"

class ThreadLocalResults 
{
public:
    explicit ThreadLocalResults(size_t numOfValueColumns);

    auto parseAndUpdate(std::string_view line) -> void;

    auto getMin(size_t index) const
    {
        return mins_[index];
    }

    auto getMax(size_t index) const
    {
        return maxs_[index];
    }

    auto getSum(size_t index) const
    {
        return sums_[index];
    }

    auto getCount(size_t index) const
    {
        return counts_[index];
    }

    auto getHistogram(size_t index) const
    {
        return histograms_[index];
    }

private:
    std::vector<double> mins_;
    std::vector<double> maxs_;
    std::vector<double> sums_;
    std::vector<size_t> counts_;
    std::vector<Histogram> histograms_;
};


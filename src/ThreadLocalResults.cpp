#include <limits>
#include <charconv>

#include "ThreadLocalResults.hpp"

ThreadLocalResults::ThreadLocalResults(size_t numOfColumns)
    : mins_(numOfColumns, std::numeric_limits<double>::max())
    , maxs_(numOfColumns, std::numeric_limits<double>::lowest())
    , sums_(numOfColumns, 0.0)
    , counts_(numOfColumns, 0)
    , histograms_(numOfColumns)
{
}

auto ThreadLocalResults::parseAndUpdate(std::string_view line, size_t numOfValueColumns) -> void
{
    if (line.empty())
    {
        return;
    }

    if (line.back() == '\r') 
    {
        line.remove_suffix(1);
    }

    const auto parts = splitLine(line, ',');
    if (parts.size() != numOfValueColumns + 1) 
    {
        return;
    }

    for (size_t i = 0; i < numOfValueColumns; ++i) 
    {
        auto value = double{};
        const auto& part = parts[i + 1];
        if (std::from_chars(part.data(), part.data() + part.size(), value).ec == std::errc()) 
        {
            mins_[i] = std::min(mins_[i], value);
            maxs_[i] = std::max(maxs_[i], value);
            sums_[i] += value;
            ++counts_[i];
            ++histograms_[i][value];
        }
    }
}

auto aggregateResults(
    const std::vector<ThreadLocalResults>& allThreadResults,
    const std::vector<std::string>& columnNames) -> std::vector<ColumnStats>
{
    auto finalColumnStats = std::vector<ColumnStats>{columnNames.size()};

    for (size_t i = 0; i < columnNames.size(); ++i) 
    {
        finalColumnStats[i].name_ = columnNames[i];
        auto finalColumnHistogram = Histogram{};

        for (const auto& threadLocalResult : allThreadResults) 
        {
            if (threadLocalResult.counts_[i] == 0) 
            {
                continue;
            }

            finalColumnStats[i].min_ = std::min(finalColumnStats[i].min_, threadLocalResult.mins_[i]);
            finalColumnStats[i].max_ = std::max(finalColumnStats[i].max_, threadLocalResult.maxs_[i]);
            finalColumnStats[i].mean_ += threadLocalResult.sums_[i];
            finalColumnStats[i].count_ += threadLocalResult.counts_[i];

            for (const auto& [value, freq] : threadLocalResult.histograms_[i]) 
            {
                finalColumnHistogram[value] += freq;
            }
        }

        if (finalColumnStats[i].count_ > 0) 
        {
            finalColumnStats[i].mean_ /= finalColumnStats[i].count_;
            finalColumnStats[i].median_ = calculateMedian(finalColumnHistogram, finalColumnStats[i].count_);
        }
    }

    return finalColumnStats;
}

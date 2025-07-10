#include <charconv>
#include <limits>

#include "ThreadLocalResults.hpp"

ThreadLocalResults::ThreadLocalResults(size_t numOfValueColumns)
    : mins_(numOfValueColumns, std::numeric_limits<double>::max())
    , maxs_(numOfValueColumns, std::numeric_limits<double>::lowest())
    , sums_(numOfValueColumns, 0.0)
    , counts_(numOfValueColumns, 0)
    , histograms_(numOfValueColumns)
{
}

auto ThreadLocalResults::parseAndUpdate(std::string_view line) -> void
{
    if (line.empty())
    {
        return;
    }

    if (line.back() == '\r') 
    {
        line.remove_suffix(1);
    }

    const auto numOfValueColumns = std::size(mins_);
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

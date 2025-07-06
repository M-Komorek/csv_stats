#pragma once

#include <map>
#include <string_view>
#include <vector>

using Histogram = std::map<double, size_t>;

auto splitLine(std::string_view line, char delimiter) -> std::vector<std::string_view>;
auto calculateMedian(const Histogram& histogram, size_t datasetSize) -> double;

#pragma once

#include <vector>

#include "ColumnStats.hpp"
#include "CsvFileInformation.hpp"
#include "ThreadLocalResults.hpp"

namespace internal
{

constexpr uintmax_t MIN_BYTES_PER_THREAD = 16 * 1024 * 1024;

auto calculateOptimalThreads(const CsvFileInformation& csvFileInformation) -> unsigned int;
auto processInParallel(const CsvFileInformation& csvFileInformation, unsigned int numOfThreads) -> std::vector<ColumnStats>;
auto processChunk(const CsvFileInformation& csvFileInformation, size_t chunkStart, size_t chunkEnd, unsigned int threadId) -> ThreadLocalResults;
auto aggregateResults(
    const std::vector<ThreadLocalResults>& allThreadResults,
    const std::vector<std::string>& columnNames) -> std::vector<ColumnStats>;

} // namespace internal

auto calculateCsvStats(const CsvFileInformation& csvFileInformation) -> std::vector<ColumnStats>;
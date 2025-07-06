#pragma once

#include <vector>

#include "ColumnStats.hpp"
#include "CsvFileInformation.hpp"
#include "ThreadLocalResults.hpp"

class CsvFileManager;

class CsvProcessor 
{
public:
    explicit CsvProcessor(CsvFileInformation csvFileInformation);

    auto run() const -> std::vector<ColumnStats>;

private:
    auto calculateOptimalThreads() const -> unsigned int;
    auto processInParallel(unsigned int numOfThreads) const -> std::vector<ColumnStats>;
    auto processChunk(size_t chunkStart, size_t chunkEnd, unsigned int threadId) const -> ThreadLocalResults;

    const CsvFileInformation csvFileInformation_;
    static constexpr uintmax_t MIN_BYTES_PER_THREAD = 16 * 1024 * 1024;
};

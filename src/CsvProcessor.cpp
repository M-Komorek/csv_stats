#include "CsvProcessor.hpp"
#include "ToolKit.hpp"

#include <fstream>
#include <future>
#include <iostream>
#include <thread>

CsvProcessor::CsvProcessor(CsvFileInformation csvFileInformation) : csvFileInformation_(std::move(csvFileInformation)) 
{
}

auto CsvProcessor::run() const -> std::vector<ColumnStats>
{
    if (csvFileInformation_.fileSize_ <= csvFileInformation_.headerEndPosition_) 
    {
        std::cout << "Warning: File contains a header but no data rows." << std::endl;
        auto emptyColumnStats = std::vector<ColumnStats>{};
        for (const auto& name : csvFileInformation_.columnNames_) 
        {
            emptyColumnStats.push_back({name});
        }
        return emptyColumnStats;
    }

    const auto numOfThreads = calculateOptimalThreads();
    std::cout << "Dynamically scaling to " << numOfThreads << " threads for optimal performance." << std::endl;

    return processInParallel(numOfThreads);
}

auto CsvProcessor::calculateOptimalThreads() const -> unsigned int
{
    const auto maxHardwareThreads = std::max(1u, std::thread::hardware_concurrency());
    const auto fileSize = csvFileInformation_.fileSize_;
    const auto headerEndPosition = csvFileInformation_.headerEndPosition_;

    if (fileSize <= headerEndPosition) 
    {
        return 1;
    }

    const auto dataSize = fileSize - headerEndPosition;
    auto idealThreads = static_cast<unsigned int>(dataSize / MIN_BYTES_PER_THREAD);
    auto practicalThreads = std::min(maxHardwareThreads, idealThreads);

    return std::max(1u, practicalThreads);
}

auto CsvProcessor::processInParallel(unsigned int numOfThreads) const -> std::vector<ColumnStats>
{
    const auto fileSize = csvFileInformation_.fileSize_;
    const auto headerEndPosition = csvFileInformation_.headerEndPosition_;
    const auto dataSize = fileSize - headerEndPosition;
    const auto chunkSize = std::max(size_t{1}, dataSize / numOfThreads);

    auto futuresWithThreadLocalResults = std::vector<std::future<ThreadLocalResults>>{};
    for (unsigned int i = 0; i < numOfThreads; ++i) 
    {
        auto chunkStart = headerEndPosition + i * chunkSize;
        if (chunkStart >= fileSize) 
        {
            break;
        };
        auto chunkEnd = (i == numOfThreads - 1) ? fileSize : chunkStart + chunkSize;

        futuresWithThreadLocalResults.push_back(
            std::async(std::launch::async, &CsvProcessor::processChunk, this, chunkStart, chunkEnd, i));
    }

    auto allThreadResults = std::vector<ThreadLocalResults>{};
    allThreadResults.reserve(futuresWithThreadLocalResults.size());
    for (auto& futureWithThreadLocalResults : futuresWithThreadLocalResults) 
    {
        allThreadResults.push_back(futureWithThreadLocalResults.get());
    }

    return aggregateResults(allThreadResults, csvFileInformation_.columnNames_);
}

auto CsvProcessor::processChunk(size_t chunkStart, size_t chunkEnd, unsigned int threadId) const -> ThreadLocalResults 
{
    auto threadLocalResults = ThreadLocalResults{csvFileInformation_.numOfValueColumns_};
    auto file = std::ifstream{csvFileInformation_.filePath_, std::ios::binary};

    file.seekg(chunkStart);
    if (threadId > 0 && chunkStart > csvFileInformation_.headerEndPosition_) 
    {
        auto dummy = std::string{};
        std::getline(file, dummy); // skip partial line
    }

    auto line = std::string{};
    while (static_cast<size_t>(file.tellg()) < chunkEnd && std::getline(file, line)) 
    {
        threadLocalResults.parseAndUpdate(line, csvFileInformation_.numOfValueColumns_);
    }

    return threadLocalResults;
}

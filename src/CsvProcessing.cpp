#include <fstream>
#include <future>
#include <iostream>
#include <thread>

#include "CsvProcessing.hpp"
#include "ToolKit.hpp"

namespace internal
{

auto calculateOptimalThreads(const CsvFileInformation& csvFileInformation) -> unsigned int
{
    const auto maxHardwareThreads = std::max(1u, std::thread::hardware_concurrency());

    if (csvFileInformation.fileSize_ <=  csvFileInformation.headerEndPosition_) 
    {
        return 1;
    }

    const auto dataSize = csvFileInformation.fileSize_ -  csvFileInformation.headerEndPosition_;
    auto idealThreads = static_cast<unsigned int>(dataSize / MIN_BYTES_PER_THREAD);
    auto practicalThreads = std::min(maxHardwareThreads, idealThreads);

    return std::max(1u, practicalThreads);
}

auto processInParallel(const CsvFileInformation& csvFileInformation, unsigned int numOfThreads) -> std::vector<ColumnStats>
{
    const auto dataSize = csvFileInformation.fileSize_ - csvFileInformation.headerEndPosition_;
    const auto chunkSize = std::max(size_t{1}, dataSize / numOfThreads);

    auto futuresWithThreadLocalResults = std::vector<std::future<ThreadLocalResults>>{};
    for (unsigned int i = 0; i < numOfThreads; ++i) 
    {
        auto chunkStart = csvFileInformation.headerEndPosition_ + i * chunkSize;
        if (chunkStart >= csvFileInformation.fileSize_) 
        {
            break;
        };
        auto chunkEnd = (i == numOfThreads - 1) ? csvFileInformation.fileSize_ : chunkStart + chunkSize;

        futuresWithThreadLocalResults.push_back(
            std::async(std::launch::async, &processChunk, csvFileInformation, chunkStart, chunkEnd, i));
    }

    auto allThreadResults = std::vector<ThreadLocalResults>{};
    allThreadResults.reserve(futuresWithThreadLocalResults.size());
    for (auto& futureWithThreadLocalResults : futuresWithThreadLocalResults) 
    {
        allThreadResults.push_back(futureWithThreadLocalResults.get());
    }

    return aggregateResults(allThreadResults, csvFileInformation.columnNames_);
}

auto processChunk(const CsvFileInformation& csvFileInformation, size_t chunkStart, size_t chunkEnd, unsigned int threadId) -> ThreadLocalResults
{
    auto threadLocalResults = ThreadLocalResults{csvFileInformation.numOfValueColumns_};
    auto file = std::ifstream{csvFileInformation.filePath_, std::ios::binary};

    file.seekg(chunkStart);
    if (threadId > 0 && chunkStart > csvFileInformation.headerEndPosition_) 
    {
        auto dummy = std::string{};
        std::getline(file, dummy); // skip partial line
    }

    auto line = std::string{};
    while (static_cast<size_t>(file.tellg()) < chunkEnd && std::getline(file, line)) 
    {
        threadLocalResults.parseAndUpdate(line);
    }

    return threadLocalResults;
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
            if (threadLocalResult.getCount(i) == 0) 
            {
                continue;
            }

            finalColumnStats[i].min_ = std::min(finalColumnStats[i].min_, threadLocalResult.getMin(i));
            finalColumnStats[i].max_ = std::max(finalColumnStats[i].max_, threadLocalResult.getMax(i));
            finalColumnStats[i].mean_ += threadLocalResult.getSum(i);
            finalColumnStats[i].count_ += threadLocalResult.getCount(i);

            for (const auto& [value, freq] : threadLocalResult.getHistogram(i))
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

} // namespace internal

auto calculateCsvStats(const CsvFileInformation& csvFileInformation) -> std::vector<ColumnStats>
{
    if (csvFileInformation.fileSize_ <= csvFileInformation.headerEndPosition_) 
    {
        std::cout << "Warning: File contains a header but no data rows." << std::endl;
        auto emptyColumnStats = std::vector<ColumnStats>{};
        for (const auto& name : csvFileInformation.columnNames_) 
        {
            emptyColumnStats.push_back(ColumnStats{name});
        }
        return emptyColumnStats;
    }

    const auto numOfThreads = internal::calculateOptimalThreads(csvFileInformation);
    std::cout << "Dynamically scaling to " << numOfThreads << " threads for optimal performance." << std::endl;

    return internal::processInParallel(csvFileInformation, numOfThreads);
}
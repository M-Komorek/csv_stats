#include <gtest/gtest.h>

#include "ToolKit.hpp"

TEST(SplitLineTest, HandlesBasicCase)
{
    const auto line = std::string{"one,two,three"};
    const auto expected = std::vector<std::string_view>{"one", "two", "three"};
    ASSERT_EQ(splitLine(line, ','), expected);
}

TEST(SplitLineTest, HandlesEmptyInput)
{
    const auto line = std::string{""};
    const auto expected = std::vector<std::string_view>{};
    ASSERT_EQ(splitLine(line, ','), expected);
}

TEST(SplitLineTest, HandlesNoDelimiter)
{
    const auto line = std::string{"single-token"};
    const auto expected = std::vector<std::string_view>{"single-token"};
    ASSERT_EQ(splitLine(line, ','), expected);
}

TEST(SplitLineTest, HandlesLeadingDelimiter)
{
    const auto line = std::string{",one,two"};
    const auto expected = std::vector<std::string_view>{"", "one", "two"};
    ASSERT_EQ(splitLine(line, ','), expected);
}

TEST(SplitLineTest, HandlesTrailingDelimiter) 
{
    const auto line = std::string{"one,two,"};
    const auto expected = std::vector<std::string_view>{"one", "two"};
    ASSERT_EQ(splitLine(line, ','), expected);
}

TEST(SplitLineTest, HandlesConsecutiveDelimiters)
{
    const auto line = std::string{"one,,three"};
    const auto expected = std::vector<std::string_view>{"one", "", "three"};
    ASSERT_EQ(splitLine(line, ','), expected);
}

TEST(SplitLineTest, HandlesDelimiterOnlyString)
{
    const auto line = std::string{",,"};
    const auto expected = std::vector<std::string_view>{"", ""};
    ASSERT_EQ(splitLine(line, ','), expected);
}

TEST(SplitLineTest, HandlesDifferentDelimiter) 
{
    const auto line = std::string{"hello world from C++"};
    const auto expected = std::vector<std::string_view>{"hello", "world", "from", "C++"};
    ASSERT_EQ(splitLine(line, ' '), expected);
}

TEST(CalculateMedianTest, ThrowsOnZeroDatasetSize) 
{
    const auto hist = Histogram{{1.0, 1}};
    ASSERT_THROW(calculateMedian(hist, 0), std::runtime_error);
}

TEST(CalculateMedianTest, HandlesOddNumberOfElements) 
{
    const auto hist = Histogram{{10.0, 1}, {20.0, 2}, {30.0, 1}, {40.0, 1}};
    const auto datasetSize = size_t{5};
    ASSERT_DOUBLE_EQ(calculateMedian(hist, datasetSize), 20.0);
}

TEST(CalculateMedianTest, HandlesSingleElement) 
{
    const auto hist = Histogram{{42.0, 1}};
    const auto datasetSize = size_t{1};
    ASSERT_DOUBLE_EQ(calculateMedian(hist, datasetSize), 42.0);
}

TEST(CalculateMedianTest, HandlesEvenNumberOfElementsAverage) 
{
    const auto hist = Histogram{{10.0, 1}, {20.0, 1}, {30.0, 1}, {40.0, 1}};
    const auto datasetSize = size_t{4};
    ASSERT_DOUBLE_EQ(calculateMedian(hist, datasetSize), 25.0);
}

TEST(CalculateMedianTest, HandlesAllElementsSame) 
{
    const auto hist = Histogram{{7.0, 5}};
    const auto datasetSize = size_t{5};
    ASSERT_DOUBLE_EQ(calculateMedian(hist, datasetSize), 7.0);
}
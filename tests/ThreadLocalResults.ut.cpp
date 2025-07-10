#include <gtest/gtest.h>

#include "ThreadLocalResults.hpp"

TEST(ThreadLocalResultsTest, ConstructorInitializesCorrectly)
{
    const auto numOfValueColumns = size_t{3};
    auto results = ThreadLocalResults{numOfValueColumns};

    for (size_t i = 0; i < numOfValueColumns; ++i)
    {
        EXPECT_EQ(results.getCount(i), 0);
        EXPECT_DOUBLE_EQ(results.getSum(i), 0.0);
        EXPECT_DOUBLE_EQ(results.getMin(i), std::numeric_limits<double>::max());
        EXPECT_DOUBLE_EQ(results.getMax(i), std::numeric_limits<double>::lowest());
        EXPECT_TRUE(results.getHistogram(i).empty());
    }
}

TEST(ThreadLocalResultsTest, ParseAndUpdateProcessesValidLine)
{
    const auto numOfValueColumns = size_t{2};
    auto results = ThreadLocalResults{numOfValueColumns};

    auto line = std::string_view{"ID1,10.5,-20.0"};
    results.parseAndUpdate(line);

    EXPECT_EQ(results.getCount(0), 1);
    EXPECT_DOUBLE_EQ(results.getSum(0), 10.5);
    EXPECT_DOUBLE_EQ(results.getMin(0), 10.5);
    EXPECT_DOUBLE_EQ(results.getMax(0), 10.5);
    EXPECT_EQ(results.getHistogram(0).at(10.5), 1);
    
    EXPECT_EQ(results.getCount(1), 1);
    EXPECT_DOUBLE_EQ(results.getSum(1), -20.0);
    EXPECT_DOUBLE_EQ(results.getMin(1), -20.0);
    EXPECT_DOUBLE_EQ(results.getMax(1), -20.0);
    EXPECT_EQ(results.getHistogram(1).at(-20.0), 1);
}

TEST(ThreadLocalResultsTest, ParseAndUpdateAccumulatesMultipleLines)
{
    const auto numOfValueColumns = size_t{1};
    auto results = ThreadLocalResults{numOfValueColumns};

    results.parseAndUpdate("ID1,10.0");
    results.parseAndUpdate("ID2,5.0");
    results.parseAndUpdate("ID3,15.0");

    EXPECT_EQ(results.getCount(0), 3);
    EXPECT_DOUBLE_EQ(results.getSum(0), 30.0); // 10 + 5 + 15
    EXPECT_DOUBLE_EQ(results.getMin(0), 5.0);
    EXPECT_DOUBLE_EQ(results.getMax(0), 15.0);
    
    const auto& histogram = results.getHistogram(0);
    EXPECT_EQ(histogram.size(), 3);
    EXPECT_EQ(histogram.at(10.0), 1);
    EXPECT_EQ(histogram.at(5.0), 1);
    EXPECT_EQ(histogram.at(15.0), 1);
}

TEST(ThreadLocalResultsTest, ParseAndUpdateHandlesCarriageReturn)
{
    const auto numOfValueColumns = size_t{1};
    auto results = ThreadLocalResults{numOfValueColumns};

    auto lineWithCR = std::string_view{"ID1,99.9\r"};
    results.parseAndUpdate(lineWithCR);

    EXPECT_EQ(results.getCount(0), 1);
    EXPECT_DOUBLE_EQ(results.getSum(0), 99.9);
    EXPECT_DOUBLE_EQ(results.getMin(0), 99.9);
}

TEST(ThreadLocalResultsTest, ParseAndUpdateIgnoresEmptyLine)
{
    const auto numOfValueColumns = size_t{1};
    auto results = ThreadLocalResults{numOfValueColumns};
    
    results.parseAndUpdate("");

    EXPECT_EQ(results.getCount(0), 0);
    EXPECT_DOUBLE_EQ(results.getSum(0), 0.0);
}

TEST(ThreadLocalResultsTest, ParseAndUpdateIgnoresLineWithWrongColumnCount)
{
    const auto numOfValueColumns = size_t{2};
    auto results = ThreadLocalResults{numOfValueColumns};
    
    results.parseAndUpdate("ID1,123.45");
    
    EXPECT_EQ(results.getCount(0), 0);
    EXPECT_EQ(results.getCount(1), 0);
}

TEST(ThreadLocalResultsTest, ParseAndUpdateSkipsNonNumericValues)
{
    const auto numOfValueColumns = size_t{3};
    auto results = ThreadLocalResults{numOfValueColumns};

    auto line = std::string_view{"ID1,5.5,not-a-number,10.1"};
    results.parseAndUpdate(line);

    EXPECT_EQ(results.getCount(0), 1);
    EXPECT_DOUBLE_EQ(results.getSum(0), 5.5);

    EXPECT_EQ(results.getCount(1), 0);
    EXPECT_DOUBLE_EQ(results.getSum(1), 0.0);

    EXPECT_EQ(results.getCount(2), 1);
    EXPECT_DOUBLE_EQ(results.getSum(2), 10.1);
}
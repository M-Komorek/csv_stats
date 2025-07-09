#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include "CsvFileInformation.hpp"

class CsvFileInformationTest : public ::testing::Test 
{
protected:
    static auto SetUpTestSuite() -> void
    {
        tempDir_ = std::filesystem::temp_directory_path() / "my_app_tests";
        std::filesystem::create_directory(tempDir_);
    }

    static auto TearDownTestSuite() -> void
    {
        std::filesystem::remove_all(tempDir_);
    }

    auto createTestFile(const std::string& filename, const std::string& content) -> void
    {
        auto path = tempDir_ / filename;
        auto ofs = std::ofstream{path};
        ofs << content;
    }

    static std::filesystem::path tempDir_;
};

std::filesystem::path CsvFileInformationTest::tempDir_;

TEST_F(CsvFileInformationTest, SuccessfullyProcessesValidCsv) 
{
    const auto filename = std::string{"valid.csv"};
    const auto content = std::string{"ID,Value1,Value2\n1,10.5,20.0\n"};
    createTestFile(filename, content);

    const auto filePath = (tempDir_ / filename).string();
    const auto result = generateCsvFileInformation(filePath);

    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(result->filePath_, filePath);
    EXPECT_EQ(result->numOfValueColumns_, 2);
    EXPECT_EQ(result->fileSize_, content.length());

    const auto expectedNames = std::vector<std::string>{"Value1", "Value2"};
    EXPECT_EQ(result->columnNames_, expectedNames);
}

TEST_F(CsvFileInformationTest, HandlesCarriageReturnInHeader)
{
    const auto filename = std::string{"crlf.csv"};
    // \r\n at the end of the header line.
    const auto content = std::string{"ID,Pressure\r\n1,101.3\n"};
    createTestFile(filename, content);
    
    const auto filePath = (tempDir_ / filename).string();
    const auto result = generateCsvFileInformation(filePath);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->numOfValueColumns_, 1);
    
    // The key is that "Pressure" should not have a '\r' at the end.
    const auto expectedNames = std::vector<std::string>{"Pressure"};
    EXPECT_EQ(result->columnNames_, expectedNames);
}

TEST_F(CsvFileInformationTest, ReturnsNulloptForNonExistentFile)
{
    const auto filePath = (tempDir_ / "non_existent_file.csv").string();
    const auto result = generateCsvFileInformation(filePath);

    EXPECT_FALSE(result.has_value());
}

TEST_F(CsvFileInformationTest, ReturnsNulloptForEmptyFile)
{
    const auto filename = std::string{"empty.csv"};
    createTestFile(filename, ""); // Create an empty file

    const auto filePath = (tempDir_ / filename).string();
    const auto result = generateCsvFileInformation(filePath);

    EXPECT_FALSE(result.has_value());
}

TEST_F(CsvFileInformationTest, ReturnsNulloptForCsvWithOnlyOneColumn)
{
    const auto filename = std::string{"one_col.csv"};
    const auto content = std::string{"ID\n1\n2\n"};
    createTestFile(filename, content);

    const auto filePath = (tempDir_ / filename).string();
    const auto result = generateCsvFileInformation(filePath);

    EXPECT_FALSE(result.has_value());
}
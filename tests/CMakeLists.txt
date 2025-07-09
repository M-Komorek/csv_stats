add_executable(csv_stats_tests
	CsvFileInformation.ut.cpp
	ToolKit.ut.cpp
)

target_link_libraries(csv_stats_tests 
	PRIVATE 
		csv_stats_lib
		GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(csv_stats_tests)
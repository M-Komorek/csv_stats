find_package(Threads REQUIRED)

add_library(csv_stats_lib
	CsvFileInformation.cpp
	ColumnStats.cpp
    CsvProcessing.cpp
    ToolKit.cpp
	ThreadLocalResults.cpp
)

target_include_directories(csv_stats_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(csv_stats_lib PRIVATE Threads::Threads)

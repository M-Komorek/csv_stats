add_executable(csv_stats main.cpp)
target_link_libraries(csv_stats PRIVATE csv_stats_lib)
target_compile_options(csv_stats PRIVATE -Wall -Wextra -Wpedantic)
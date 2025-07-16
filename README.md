# CSV Stats

A high-performance C++ tool for calculating basic statistics (count, mean, and standard deviation) on a specific column of a large CSV file. It leverages parallel processing and a MapReduce architecture to efficiently handle files that are too large to fit into memory, and it can dynamically determine the optimal number of threads for the fastest execution.

---

Example Input (data/example.csv):
```
id,value1,value2
1,10,20
2,30,40
3,25,35
```

Expected Output (Human-readable):
```
Column: value1
- Min: 10
- Max: 30
- Mean: 21.67
- Median: 25.00

Column: value2
- Min: 20
- Max: 40
- Mean: 31.67
- Median: 35.00
```

## Key Features
- Parallel Processing: Utilizes a thread pool to process multiple chunks of the CSV file concurrently, significantly speeding up computation on multi-core systems.
- MapReduce Architecture: The problem is broken down using a MapReduce pattern.
  - Map: Each thread processes a chunk of the file, calculating partial statistics.
  - Reduce: The partial results from all threads are merged to produce the final statistics.
- Dynamic Thread Optimization: The program can benchmark itself on the target machine to find and use the optimal number of threads for the fastest processing time.
- Memory Efficient: The file is processed in chunks, not loaded into memory all at once, allowing it to handle extremely large CSV files with a small memory footprint.
- Robust CSV Parsing: Correctly handles file chunking by ensuring that chunks do not split lines, preserving data integrity.

## How It Works

The core of the project is a carefully designed parallel processing pipeline.

1. File Chunking: The input CSV file is logically divided into several chunks equal to the number of threads. The program intelligently finds the start and end of each chunk by scanning for newline characters, ensuring that no row is ever split between two chunks.
2. Map Phase: Each thread independently executes a "map" task:
  - It reads its assigned chunk of the file.
  - It parses the rows, extracts the numerical value from the specified column.
  - It calculates a `ThreadLocalResults` object containing the count, sum, and sum_of_squares for its chunk.
3. Reduce Phase: Once all threads have completed their tasks, the main thread collects the `ThreadLocalResults` from each one. It then "reduces" them by iteratively merging them into a single, final `ColumnStats` object.
4. Final Calculation: From the final aggregated stats, the global mean and standard deviation are calculated and displayed to the user.

## Dynamic Thread Optimization

When run without a specified thread count, _csv_stats_ enters an optimization mode. It performs the entire `process_csv` operation for a range of thread counts (from 1 up to the number of hardware cores). It measures the execution time for each run and identifies which thread count yielded the fastest result. It then presents the statistics from that optimal run, along with the number of threads used.

## Build and run
Build project:
``` bash
mkdir build
cmake -B build/
cmake --build build/
```
Run app:
```bash
./build/app/csv_stats data/example.csv
```
Run tests:
```bash
cd build/
ctest
```

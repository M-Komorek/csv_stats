# Overview
Implementation of a multi-threaded tool for aggregating statistics from a csv file.

The current implementation is divided into the following steps:
1. Pre-analysis of the provided csv file.
2. Selection of the optimal number of threads needed to process the file.
3. Parallel file processing (each thread processes a file chunk).
4. Aggregation of results provided by threads into the final result.
5. Presentation of the final result on the standard output.
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

## Current State
Maybe not so good, but at least it was delivered quickly.

## To-do list
- Multiple files support.
- Improving the resource management mechanism.
- Code reorganization, introducing better composition.
- Add unit tests.

# Build and run
Build project:
``` bash
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

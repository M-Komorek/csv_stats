#include <chrono>
#include <iostream>

#include "ColumnStats.hpp"
#include "CsvProcessing.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) 
    {
        std::cerr << "Usage: " << argv[0] << " file.csv" << std::endl;
        return 1;
    }
    
    try 
    {
        const auto startTime = std::chrono::high_resolution_clock::now();
        
        const auto csvFileInformation = generateCsvFileInformation(argv[1]);
        if (not csvFileInformation)
        {
            std::cerr << "Failed to initialize csv file manager." << std::endl;
            return 1;
        }
        const auto stats = calculateCsvStats(csvFileInformation.value());

        const auto endTime = std::chrono::high_resolution_clock::now();
        const auto diff = std::chrono::duration<double>{endTime - startTime};
        
        prettyPrint(stats);
        
        std::cout << "Total processing time: " << diff.count() << " seconds." << std::endl;
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "An unexpected error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

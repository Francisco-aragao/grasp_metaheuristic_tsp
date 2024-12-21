#include <ctime>
#include <string>
#include "../include/utils.hpp"
#include <string>

using namespace std;
namespace fs = std::filesystem;

Utils utils;

void grasp(ifstream& inputFile, bool useCenterCity, double alpha, int iterations) {
    double bestPathCost = std::numeric_limits<double>::infinity();
    vector<int> bestPath;
    vector<City> bestCities;
    std::clock_t start = std::clock();
    double totalElapsedTime = 0;
    double totalPathCost = 0;
    double totalInitialPathCost = 0;

    // Read and process the input file
    vector<string> res = utils.findPathInfo(inputFile);
    int numCities = stoi(res[0]);
    string distance_type = res[1];

    utils.storeCities(inputFile, numCities, distance_type);
    
    for (int iteration = 0; iteration < iterations; iteration++) {
        start = std::clock();  

        cout << "GRASP Iteration " << iteration + 1 << ":\n";

        // construct the initial path
        double pathCost = utils.constructive_heuristic(numCities, useCenterCity, alpha);
        totalInitialPathCost += pathCost;

        cout << "Initial Path Cost: " << pathCost << "\n";

        vector<City> currentCities = utils.getCities();
        vector<int> currentPath = utils.getPath();

        bool improved = true;

        // starting local search
        while (improved) {
            improved = false;

            // Apply local search methods (e.g., two_opt, three_opt)
            double newPathCost = utils.two_opt(currentCities);
            if (newPathCost < pathCost) {
                pathCost = newPathCost;
                improved = true;
                continue;
            }

            newPathCost = utils.three_opt(currentCities);
            if (newPathCost < pathCost) {
                pathCost = newPathCost;
                improved = true;
                continue;
            }
        }

        // Update the best solution if needed
        if (pathCost < bestPathCost) {
            bestPathCost = pathCost;
            bestPath = utils.getPath();
            bestCities = currentCities;
        }

        cout << "Final Path Cost: " << pathCost << "\n";
        double GraspTime = double(std::clock() - start) / CLOCKS_PER_SEC;

        totalElapsedTime += GraspTime;
        totalPathCost += pathCost;

        cout << "Grasp Time: " << GraspTime << " seconds\n";        
    }
    
    double avgPathCost = totalPathCost / iterations;
    double avgElapsedTime = totalElapsedTime / iterations;
    double avgInitialPathCost = totalInitialPathCost / iterations;
    cout << "Average Path Cost: " << avgPathCost << "\n";
    cout << "Average Elapsed Time: " << avgElapsedTime << " seconds\n";
    cout << "Average Initial Path Cost: " << avgInitialPathCost << "\n";

    // Output the best solution found
    cout << "\nBest Solution Found:\n";
    cout << "Best Path Cost: " << bestPathCost << "\n";
    /* cout << "Path: ";
    for (int city : bestPath) {
        cout << city << " ";
    } */
    cout << endl;
}

int main(int argc, char* argv[]) {

    // receive input folder path and type of city to start in the command line
    if (argc != 5) {
        cerr << "Usage: " << argv[0] << " <folder_path>" << " <use_center_city>" << "<alpha>" << "<iterations>" << endl;
        cerr << "Example: " << argv[0] << " ./tsp_files 1 -> using center city" << "0.5" << 5 << endl;
        cerr << "Example: " << argv[0] << " ./tsp_files 0 -> using first city" << "0.2" << 10 << endl;
        return 1;
    }

    string folderPath = argv[1];
    bool useCenterCity = stoi(argv[2]);
    double alpha = stod(argv[3]);
    int iterations = stoi(argv[4]);

    // Process all .tsp files in the folder
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.path().extension() == ".tsp") { // just open .tsp files

            string filename = entry.path().string();
            ifstream inputFile(filename);

            if (!inputFile.is_open()) {
                cerr << "Failed to open file: " << filename << endl;
                return 1;
            }

            cout << "Results for " << filename << ":\n";
            grasp(inputFile, useCenterCity, alpha, iterations);
        }
    }

    return 0;
}
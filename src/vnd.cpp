#include <ctime>
#include <string>
#include "../include/utils.hpp"
#include <string>

using namespace std;
namespace fs = std::filesystem;

const int NUMBER_OF_ITERATIONS = 5;
Utils utils;

void grasp(ifstream& inputFile, bool useCenterCity) {
    const int GRASP_ITERATIONS = 10; // Number of GRASP iterations
    double bestPathCost = std::numeric_limits<double>::infinity();
    vector<int> bestPath;
    vector<City> bestCities;

    // Read and process the input file
    vector<string> res = utils.findPathInfo(inputFile);
    int numCities = stoi(res[0]);
    string distance_type = res[1];

    for (int iteration = 0; iteration < GRASP_ITERATIONS; iteration++) {
        cout << "GRASP Iteration " << iteration + 1 << ":\n";

        // **Construction Phase**
        double pathCost = utils.constructive_heuristic(inputFile, numCities, distance_type, useCenterCity, true); // Add randomization

        cout << "Initial Path Cost: " << pathCost << "\n";

        // **Local Search Phase**
        vector<City> currentCities = utils.getCities();
        vector<int> currentPath = utils.getPath();

        bool improved = true;
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

            newPathCost = utils.double_bridge(currentCities);
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
    }

    // Output the best solution found
    cout << "\nBest Solution Found:\n";
    cout << "Path Cost: " << bestPathCost << "\n";
    cout << "Path: ";
    for (int city : bestPath) {
        cout << city << " ";
    }
    cout << endl;
}
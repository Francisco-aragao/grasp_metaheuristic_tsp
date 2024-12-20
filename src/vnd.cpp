#include <ctime>
#include <string>
#include "../include/utils.hpp"
#include <string>

using namespace std;
namespace fs = std::filesystem;

Utils utils;
const int GRASP_ITERATIONS = 10; // Number of GRASP iterations

void grasp(ifstream& inputFile, bool useCenterCity) {
    double bestPathCost = std::numeric_limits<double>::infinity();
    vector<int> bestPath;
    vector<City> bestCities;

    // Read and process the input file
    vector<string> res = utils.findPathInfo(inputFile);
    int numCities = stoi(res[0]);
    string distance_type = res[1];

    utils.storeCities(inputFile, numCities, distance_type);

    
    for (int iteration = 0; iteration < GRASP_ITERATIONS; iteration++) {
        cout << "GRASP Iteration " << iteration + 1 << ":\n";


        // **Construction Phase**
        double pathCost = utils.constructive_heuristic(numCities, useCenterCity); // Add randomization

        cout << "Initial Path Cost: " << pathCost << "\n";
        //double originalPathCost = pathCost;

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

            /* newPathCost = utils.double_bridge(currentCities);
            if (newPathCost < pathCost) {
                pathCost = newPathCost;
                improved = true;
                continue;
            } */
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
    /* cout << "Path: ";
    for (int city : bestPath) {
        cout << city << " ";
    } */
    cout << endl;
}

int main(int argc, char* argv[]) {

    // receive input folder path and type of city to start in the command line
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <folder_path>" << " <use_center_city>" << endl;
        cerr << "Example: " << argv[0] << " ./tsp_files 1 -> using center city" << endl;
        cerr << "Example: " << argv[0] << " ./tsp_files 0 -> using first city" << endl;
        return 1;
    }

    string folderPath = argv[1];
    bool useCenterCity = stoi(argv[2]);


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
            grasp(inputFile, useCenterCity);
        }
    }

    return 0;
}
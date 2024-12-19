#ifndef UTILS_HPP
#define UTILS_HPP

#include <algorithm>
#include <fstream>
#include <filesystem>
#include "city.hpp"

using namespace std;

// class to handle utility functions for reading and processing input data, and calculating the path

class Utils {
    private:
        vector<int> path;
        vector<City> cities;
        double pathCost;
    public:        
        double getPathCost() {return pathCost;};
        vector<int> getPath() {return path;};
        vector<City> getCities() {return cities;};
        void setPathCost(double pathCost) {this->pathCost = pathCost;};
        void setCities(vector<City> cities) {this->cities = cities;};
        void setPath(vector<int> path) {this->path = path;};
        double findPath(int initialCityId, vector<City> cities, int numCities);
        int findCenterCity(vector<City> cities, int numCities);
        vector<City> receiveCoordinatesParameters(ifstream& inputFile, int numCities, string DISTANCE_TYPE);
        vector<string> findPathInfo(ifstream& inputFile);
        double constructive_heuristic(ifstream& inputFile, int numCities, string distance_type, bool useCenterCity);
        double two_opt(vector<City> cities);
        double three_opt(vector<City> cities);
        double double_bridge(vector<City> cities);
        vector<vector<double>> calculateAllDistances(vector<City> cities);

};

#endif

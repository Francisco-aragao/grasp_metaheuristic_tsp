#ifndef CITY_HPP
#define CITY_HPP

#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

// class to handle city information and perform calculations about distances

class City {
    private:
        int id;
        int x;
        int y;
        std::vector<double> distances;
    public:
        City(int id, int x, int y, int numCities);
        int getX() {return x;};
        int getY() {return y;};
        int getId() {return id;};
        double returnDistanceTo(int id);
        double calculateDistanceTo(City city, std::string distanceType);
        void setDistances(int id, double distance);
        bool operator==(const City& city);
};

#endif

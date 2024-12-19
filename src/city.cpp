#include "../include/city.hpp"

City::City(int id, int x, int y, int numCities) {
    
    this->id = id;
    this->x = x;
    this->y = y;

    // used + 1 to avoid index 0
    std::vector<double> distances(numCities+1, -1);
    this->distances = distances;
}

// define operator== for City
bool City::operator==(const City& city) {
    return this->id == city.id;
}

double City::returnDistanceTo(int id) {
    return this->distances[id];
}

double City::calculateDistanceTo(City city, std::string distanceType) {
    // calculates the euclidian OR pseudo euclidian distance between two cities

    if (distanceType == "EUC_2D") {
        
        double x = city.getX() - this->getX();
        double y = city.getY() - this->getY();

        return sqrt(x*x + y*y);

    } else if (distanceType == "ATT") {

        double x = city.getX() - this->getX();
        double y = city.getY() - this->getY();

        double rij=  sqrt((x*x + y*y) / 10.0);
        double tij= round(rij);

        if (tij < rij) {
            return tij + 1;
        } else {
            return tij;
        }

    } else {
        throw std::invalid_argument("Invalid distance type");
    }
    
}

void City::setDistances(int id, double distance) {

    this->distances[id] = distance;    
}

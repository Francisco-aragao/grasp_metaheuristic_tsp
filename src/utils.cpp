#include "../include/utils.hpp"

// funciton to calculate all the distances between the cities
vector<vector<double>> Utils::calculateAllDistances(vector<City> cities) {
    // for each index representing the city ID i will store the distances to all other cities, sorted by the distance
    vector<vector<double>> distances;
    for (int i = 0; i < (int)cities.size(); i++) {
        vector<double> distancesToCities;
        for (int j = 0; j < (int)cities.size(); j++) {
            if (i == j) {
                distancesToCities.push_back(-1);
                continue;
            }
            distancesToCities.push_back(cities[i].returnDistanceTo(j));
        }
        distances.push_back(distancesToCities);
    }
    
}

double Utils::findPath(int initialCityId, vector<City> cities, int numCities) {
    vector<int> path;
    this->path = path;

    path.push_back(initialCityId);

    int currentCityId = initialCityId;
    
    double pathCost = 0;

    vector<vector<double>> distances = calculateAllDistances(cities);

    // ARRUMAR AQUI

    while ((int)path.size() < numCities) {
        double minDistance = 5000000;
        int nextCityId = 0;

        for (auto city : cities) {
            if (city.getId() == 0)
                continue;

            if (city.getId() == currentCityId)
                continue;

            if (find(path.begin(), path.end(), city.getId()) != path.end())
                continue;

            double distance = city.returnDistanceTo(currentCityId);
            if (distance < minDistance) {
                if (minDistance != 5000000)
                    pathCost -= minDistance;

                pathCost += distance;
                minDistance = distance;
                nextCityId = city.getId();
            }
        }

        path.push_back(nextCityId);
        currentCityId = nextCityId;
    }
    
    path.push_back(initialCityId);
    pathCost += cities[currentCityId].returnDistanceTo(initialCityId);

    this->path = path;

    return pathCost;
}

int Utils::findCenterCity(vector<City> cities, int numCities) {
    double minDistance = 5000000;
    int idCenterCity = 0;
    for (auto city : cities) {
        if (city.getId() == 0)
            continue;

        double sum = 0;
        for (int i = 1; i <= numCities; i++) {
            if (city.getId() == i)
                continue;

            sum += city.returnDistanceTo(i);
        }

        if (sum < minDistance) {
            minDistance = sum;
            idCenterCity = city.getId();
        }
    }

    return idCenterCity;
}

vector<City> Utils::receiveCoordinatesParameters(ifstream& inputFile, int numCities, string DISTANCE_TYPE) {
    
    std::vector<City> cities;

    City initialCity(0, 0, 0, numCities);
    cities.push_back(initialCity);
    
    int id;
    float x, y;
    for (int i = 0; i < numCities; i++) {
        inputFile >> id >> x >> y;
        City newCity(id, x, y, numCities);
        cities.push_back(newCity);

        for (auto city : cities) {
            if (city.getId() == 0)
                continue;

            if (city == newCity)
                continue;

            if (city.returnDistanceTo(id) != -1)
                continue;

            double distance = city.calculateDistanceTo(newCity, DISTANCE_TYPE);
            city.setDistances(id, distance);
            newCity.setDistances(city.getId(), distance);

            cities[city.getId()] = city;
            cities[id] = newCity;
        }
    }

    return cities;
}

vector<string> Utils::findPathInfo(ifstream& inputFile) {

    string info;

    string numCities;
    string distance_type;

    while (info != "NODE_COORD_SECTION") {

        inputFile >> info;
        
        if (info == "DIMENSION:") {
            inputFile >> info;

            numCities =info;
        } else if (info == "DIMENSION") {
            inputFile >> info; // reading ':'
            inputFile >> info;
            numCities = info;
        }

        if (info == "EDGE_WEIGHT_TYPE:") {
            inputFile >> info;
            distance_type = info;
        } else if (info == "EDGE_WEIGHT_TYPE") {
            inputFile >> info; // reading ':'
            inputFile >> info;
            distance_type = info;
        }
    }

    vector<string> results;
    results.push_back(numCities);
    results.push_back(distance_type);

    return results;
}

// constructive heuristic to find the initial path using the nearest neighbor algorithm (greedy)
double Utils::constructive_heuristic(ifstream& inputFile, int numCities, string distance_type, bool useCenterCity) {

    vector<City> cities;
    cities = this->receiveCoordinatesParameters(inputFile, numCities, distance_type);

    this->cities = cities;

    int initialCityId = 1;
    if ( useCenterCity )
        initialCityId = this->findCenterCity(cities, numCities);


    double pathCost = this->findPath(initialCityId, cities, numCities);


    this->pathCost = pathCost;

    return pathCost;
}

// 2-opt heuristic to improve the path. Runs over all possible pairs of edges and tries to swap them to improve the path cost
double Utils::two_opt(vector<City> cities) {

    vector<int> initialPath = this->path;

    double newPathCost = this->pathCost + 1;
    vector<int> newPath;

    int tries = 0;
    newPathCost = this->pathCost + 1;
    newPath = initialPath;

    for (int i = 0; i < (int)initialPath.size()  ; i++) {

        for (int j = 0; j < (int) initialPath.size() ; j++) {

            if (i == j)
                continue;

            tries++;
            newPath = initialPath;

            int temp = newPath[i];
            newPath[i] = newPath[j];
            newPath[j] = temp;

            newPathCost = 0;
            for (int i = 0; i < (int)newPath.size() - 1; i++) {
                newPathCost += cities[newPath[i]].returnDistanceTo(newPath[i + 1]);
            }

            newPathCost += cities[newPath[newPath.size() - 1]].returnDistanceTo(newPath[0]); // add the distance from the last city to the first one

            if (newPathCost < this->pathCost) {
                this->path = newPath;
                this->pathCost = newPathCost;

                return newPathCost;
            }
        }   
    }
    //cout << "Tries: " << tries << endl;

    return this->pathCost;


}

// 3-opt heuristic to improve the path. Runs over all possible triplets of edges and tries to swap them to improve the path cost
double Utils::three_opt(vector<City> cities) {
    
    vector<int> initialPath = this->path;

    double newPathCost = this->pathCost + 1;
    vector<int> newPath;

    int tries = 0;

    newPathCost = this->pathCost + 1;
    newPath = initialPath;

    for (int i = 0; i < (int)initialPath.size()  ; i++) {

        for (int j = 0; j < (int) initialPath.size() ; j++) {
            
            for (int k = 0; k < (int) initialPath.size() ; k++) {

                if (i == j || j == k || i == k)
                    continue;

                tries++;
                newPath = initialPath;

                int temp = newPath[i];
                newPath[i] = newPath[j];
                newPath[j] = newPath[k];
                newPath[k] = temp;

                newPathCost = 0;
                for (int i = 0; i < (int)newPath.size() - 1; i++) {
                    newPathCost += cities[newPath[i]].returnDistanceTo(newPath[i + 1]);
                }

                newPathCost += cities[newPath[newPath.size() - 1]].returnDistanceTo(newPath[0]); // add the distance from the last city to the first one

                if (newPathCost < this->pathCost) {
                    this->path = newPath;
                    this->pathCost = newPathCost;

                    return newPathCost;
                }
            }
        }   
    }
    //cout << "Tries: " << tries << endl;


    return this->pathCost;
}

// double bridge heuristic to improve the path. Runs over all possible 4-tuple of edges and tries to swap them to improve the path cost. The double bridge replace the edges using a cross strategy 
double Utils::double_bridge(vector<City> cities) {

    vector<int> initialPath = this->path;

    double newPathCost = this->pathCost + 1;
    vector<int> newPath = initialPath;

    int tries = 0;

    for (int i = 0; i < (int)initialPath.size()  ; i++) {

        for (int j = i; j < (int) initialPath.size() ; j++) {
            
            for (int k = j; k < (int) initialPath.size() ; k++) {
            
                for (int l = k; l < (int) initialPath.size() ; l++) {
                    
                    if (i == j || j == k || i == k || i == l || j == l || k == l)
                        continue;

                    newPath = initialPath;

                    tries++;
                    int temp = newPath[i];
                    int temp2 = newPath[l];
                    newPath[i] = newPath[k];
                    newPath[l] = newPath[j];
                    newPath[k] = temp;
                    newPath[j] = temp2;

                    newPathCost = 0;
                    for (int i = 0; i < (int)newPath.size() - 1; i++) {
                        newPathCost += cities[newPath[i]].returnDistanceTo(newPath[i + 1]);
                    }

                    newPathCost += cities[newPath[newPath.size() - 1]].returnDistanceTo(newPath[0]); // add the distance from the last city to the first one

                    if (newPathCost < this->pathCost) {
                        this->path = newPath;
                        this->pathCost = newPathCost;

                        return newPathCost;
                    }
                }
            }
        }
    }

    //cout << "Tries: " << tries << endl;

    return this->pathCost;
}
#include "../include/utils.hpp"

// funciton to calculate all the distances between the cities
vector<vector<double>> Utils::calculateAllDistances(vector<City> cities) {
    // print info city 1
    vector<vector<double>> distances;
    // create a vector with 0 to add in distances
    //vector<double> zero(cities.size(), 0);
    //distances.push_back(zero);
    for (int i = 0; i < (int)cities.size(); i++) {
        vector<double> distancesToCities;
        for (int j = 0; j < (int)cities.size(); j++) {
            if (i == 0 || j == 0) {
                distancesToCities.push_back(0);
                continue;
            }
            if (i > j) {
                // if i > j, the distance is already calculated
                distancesToCities.push_back(distances[j][i]);
                continue;
            }
            if (i == j) {
                distancesToCities.push_back(0);
                continue;
            }
            distancesToCities.push_back(cities[i].returnDistanceTo(j));
        }
        // print distancesToCities
        distances.push_back(distancesToCities);
    }

    return distances;
    
}

vector<int> selectCandidates(int numCities, int currentCity, vector<City> cities, vector<int> path) {
    vector<pair<int, double>> candidates;  // {cityId, distance}
    //create candidates list based on the current city

    for (int i = 1; i <= numCities; i++) {
        if (i == currentCity)
            continue;

        // check if the city is already in the path
        if (find(path.begin(), path.end(), i) != path.end())
            continue;

        double distance = cities[currentCity].returnDistanceTo(i);
        candidates.push_back({i, distance});
    }

    //sort candidates list by distance
    sort(candidates.begin(), candidates.end(), [](pair<int, double> a, pair<int, double> b) {
        return a.second < b.second;
    });

    // find the size of the candidates list based on cmin + (cmax - cmin) * alpha
    double cmin = candidates[0].second;
    double cmax = candidates[candidates.size() - 1].second;
    double alpha = 0.7;
    double size = cmin + (cmax - cmin) * alpha;

    vector<int> selectedCandidates;
    // return all candidates with distance <= size
    for (auto candidate : candidates) {
        // check if the candidate is already in the path
        if (candidate.second > size) {
            break;
        }
        selectedCandidates.push_back(candidate.first);
    }

    return selectedCandidates;

}

double Utils::findPath(int initialCityId, vector<City> cities, int numCities) {
    vector<int> path;
    this->path = path;

    path.push_back(initialCityId);

    int currentCityId = initialCityId;
    
    double pathCost = 0;

    // vector<vector<double>> distances = calculateAllDistances(cities);

    while ((int) path.size() < numCities) {

        vector<int> candidatesList = selectCandidates(numCities, currentCityId, cities, path);

        // select random city from candidates list

        int nextCityId = candidatesList[rand() % candidatesList.size()];

        pathCost += cities[currentCityId].returnDistanceTo(nextCityId);

        path.push_back(nextCityId);
        currentCityId = nextCityId;

    }

    this->path = path;

    pathCost += cities[currentCityId].returnDistanceTo(initialCityId);

    /* for (int i = 0; i < (int)path.size(); i++) {
        cout << path[i] << " ";
    } */

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

double Utils::storeCities(ifstream& inputFile, int numCities, string distance_type) {

    vector<City> cities;
    cities = this->receiveCoordinatesParameters(inputFile, numCities, distance_type);

    this->cities = cities;

    return 0;
}

// constructive heuristic to find the initial path using the nearest neighbor algorithm (greedy)
double Utils::constructive_heuristic(int numCities, bool useCenterCity) {

    vector<City> cities;
    /* cities = this->receiveCoordinatesParameters(inputFile, numCities, distance_type);

    this->cities = cities; */

    cities = this->getCities();

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



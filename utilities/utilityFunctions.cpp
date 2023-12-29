#include "../main.h"
using namespace std;

// sqrt((positionOne[0] – positionTwo[0])² + (positionOne[1] - positionTwo[1])²)
float calculateDistance(vector<float> positionOne, vector<float> positionTwo) {
    float subtractOne = positionOne[0] - positionTwo[0];
    float subtractTwo = positionOne[1] - positionTwo[1];
    float squaredOne = subtractOne * subtractOne;
    float squareTwo = subtractTwo * subtractTwo;
    float finalBeforeSquare = squaredOne + squareTwo;
    return sqrt(finalBeforeSquare);
}
//tan^-1(numerator/denominator)
float inverseTangentOfDivision(float numerator, float denominator) {
    if (numerator == 0.f || denominator == 0.f) return 0.f;
    float res = numerator / denominator;
    return atan(res);
}
// cos^-1(factorOne²-factorTwo²-factorThree²/(-2*factorTwo*factorThree))
float triangleAngleUsingInverseCosine(float factorOne, float factorTwo, float factorThree) {
    float numerator = pow(factorOne, 2.f) - pow(factorTwo, 2.f) - pow(factorThree, 2.f);
    float denominator = -2.f * factorTwo * factorThree;
    float dividend = numerator / denominator;
    return acos(dividend);
}
// sqrt(side1²+side2²)
float hypotenuse(float side1, float side2) {
    float sqr1 = pow(side1, 2.f);
    float sqr2 = pow(side2, 2.f);
    float additive = sqr1 + sqr2;
    return sqrt(additive);
}

float convertRadiansToDegrees(float radians) {
    return radians * (180 / M_PI);
}

void parseNumberArrayToFloatVector(picojson::value coordJson, vector<float>* list) {
    const picojson::array& defaultCoordAlias = coordJson.get<picojson::array>();
    for (auto i = defaultCoordAlias.begin();i != defaultCoordAlias.end();++i) {
        float coord = i->get<double>();
        list->push_back(coord);
    }
}

void parseCoordinateListFromJsonArray(picojson::value coordJsonList, vector<vector<float>>* list) {
    const picojson::array& NestedAlias = coordJsonList.get<picojson::array>();
    for (auto vec = NestedAlias.begin(); vec != NestedAlias.end(); ++vec) {
        vector<float> coordinate;
        const picojson::array& coord = vec->get<picojson::array>();
        for (auto single = coord.begin(); single != coord.end(); single++) {
            float axis = single->get<double>();
            coordinate.push_back(axis);
        }
        list->push_back(coordinate);
    }
}

string returnArrayOfJsonsFromList(vector<string>* jsons) {
    stringstream result;
    int endOfList = jsons->size();
    result << "[";
    for (int i = 0; i < endOfList; i++) {
        result << jsons->at(i);
        if (i + 1 < endOfList) result << ",";
    }
    result << "]";
    return result.str();
}
#include "../main.h"
using namespace std;

float calculateCAngleFromSides(float a, float b, float c)
{
    // broken out into steps to make readable
    float numerator = pow(a, 2) + pow(b, 2) - pow(c, 2);
    float denominator = 2 * a * b;
    float divisionResult = numerator / denominator;
    float degreeOfAdjacentAngle = acos(divisionResult);
    float degreeConversion = degreeOfAdjacentAngle * (180 / 3.1415);
    return degreeConversion; // * (180 / 3.1415) turns radians into degrees.
}

float calculateDistance(vector<float> positionOne, vector<float> positionTwo)
{
    float subtractOne = positionOne[0] - positionTwo[0];
    float subtractTwo = positionOne[1] - positionTwo[1];
    float squaredOne = subtractOne * subtractOne;
    float squareTwo = subtractTwo * subtractTwo;
    float finalBeforeSquare = squaredOne + squareTwo;
    return sqrt(finalBeforeSquare);
}

void parseNumberArrayToFloatVector(picojson::value coordJson, vector<float> *list)
{
    const picojson::array &defaultCoordAlias = coordJson.get<picojson::array>();
    for (auto i = defaultCoordAlias.begin();i != defaultCoordAlias.end();++i)
    {
        float coord = i->get<double>();
        list->push_back(coord);
    }
}

void parseCoordinateListFromJsonArray(picojson::value coordJsonList, vector<vector<float>> *list)
{
    const picojson::array &NestedAlias = coordJsonList.get<picojson::array>();
    for (auto vec = NestedAlias.begin(); vec != NestedAlias.end(); ++vec)
    {
        vector<float> coordinate;
        const picojson::array &coord = vec->get<picojson::array>();
        for(auto single = coord.begin(); single != coord.end(); single++) {
            float axis = single->get<double>();
            coordinate.push_back(axis);
        }
        list->push_back(coordinate);
    }
}
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
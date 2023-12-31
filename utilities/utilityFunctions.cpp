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
float convertRadiansToDegrees(float radians) {
    return radians * (180 / M_PI);
}
float convertDegreesToRadians(float degrees) {
    return degrees * (M_PI / 180);
}
// atan2(y/x) returns angle in degrees
float inverseTangent2(flatCoordinate* coord) {
    if (coord->x == 0.f || coord->y == 0.f) return 0.f;
    float radianResult = atan2(coord->y, coord->x);
    float degreeResult = convertRadiansToDegrees(radianResult);
    return degreeResult;
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

float calculateSideViewTrianglesLongSide(flatCoordinate* xyCoord, float z, float L1) {
    float xSqr = pow(xyCoord->x, 2);
    float ySqr = pow(xyCoord->y, 2);
    float lengthSqr = pow((z - L1), 2);
    float additive = xSqr + ySqr + lengthSqr;
    return sqrt(additive);
}

// returns result in degrees
float inverseCosineOfInnerTriangle(float PS, float L2, float L3) {
    float numerator = pow(PS, 2.f) - pow(L2, 2.f) - pow(L3, 2.f);
    float denominator = 2 * L3 * L2;
    float dividend = numerator / denominator;
    float radianAngleResult = acos(dividend);
    float degreeResult = convertRadiansToDegrees(radianAngleResult);
    return degreeResult;
}
//returns opposite side length provided right triangle hypotenuse and adjacent angle
//must inform function of whether angle is degrees or radians
float rightTriOppositeSideLength(float hypotenuse, float adjacentAngle, bool angleIsInRadian) {
    float angle = adjacentAngle;
    if (angleIsInRadian == false) angle = convertDegreesToRadians(adjacentAngle);
    float angleSin = sin(angle);
    float oppositeSide = angleSin * hypotenuse;
    return oppositeSide;
}
// returns adjacent side length provided right triangle hypotenuse and adjacent angle 
float rightTriAdjacentSideLength(float hypotenuse, float adjacentAngle, bool angleIsInRadian) {
    float angle = adjacentAngle;
    if (angleIsInRadian == false) angle = convertDegreesToRadians(adjacentAngle);
    float angleCos = cos(angle);
    float adjacentSide = angleCos * hypotenuse;
    return adjacentSide;
}

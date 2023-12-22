#include "../main.h"
using namespace std;

class TriangleTracker {
    public:
    vector<float> sideLengths = { 1.f, 1.f, 1.f };
    vector<float> triangleAngles = { 60.f, 60.f, 60.f };
    vector<vector<float>> pointCoordinates;
    public:
    TriangleTracker(vector<vector<float>> pointCoordinates) {
        sideLengths[0] = calculateDistance(pointCoordinates[0], pointCoordinates[1]);
        sideLengths[1] = calculateDistance(pointCoordinates[1], pointCoordinates[2]);
        sideLengths[2] = calculateDistance(pointCoordinates[2], pointCoordinates[0]);
        this->pointCoordinates = pointCoordinates;
        createAngles();
    }
    void setNewCoordinates(vector<vector<float>> pointCoordinates) {
        sideLengths[0] = calculateDistance(pointCoordinates[0], pointCoordinates[1]);
        sideLengths[1] = calculateDistance(pointCoordinates[1], pointCoordinates[2]);
        sideLengths[2] = calculateDistance(pointCoordinates[2], pointCoordinates[0]);
        createAngles();
    }
    float calculateArcCosineDegrees(float a, float b, float c) {
        // broken out into steps to make readable
        float numerator = pow(a, 2) + pow(b, 2) - pow(c, 2);
        float denominator = 2 * a * b;
        float divisionResult = numerator / denominator;
        float degreeOfAdjacentAngle = acos(divisionResult);
        float degreeConversion = degreeOfAdjacentAngle * (180 / 3.1415);
        return degreeConversion; // * (180 / 3.1415) turns radians into degrees.
    }

    void createAngles() {
        // arccosine calculations are the same for all angles with a reordering of the sides.
        // therefore we can use one function and simply replace the variables.
        float angleC = calculateArcCosineDegrees(sideLengths[0], sideLengths[1], sideLengths[2]);
        triangleAngles[2] = angleC;
        float angleB = calculateArcCosineDegrees(sideLengths[0], sideLengths[2], sideLengths[1]);
        triangleAngles[1] = angleB;
        float angleA = calculateArcCosineDegrees(sideLengths[1], sideLengths[2], sideLengths[0]);
        triangleAngles[0] = angleA;
    }
    coordinateChanges calculateNewCoordinateChanges(float newSideLength, flatCoordinate* desiredPosition) {

        float newDegrees = calculateArcCosineDegrees(sideLengths[0], sideLengths[1], newSideLength);
        float degreesOfChange = newDegrees - triangleAngles[2];
        float radians = abs(degreesOfChange) * (M_PI / 180);
        float origin[2] = { pointCoordinates[1].at(0), pointCoordinates[1].at(1) };
        float knownEnd[2] = { pointCoordinates[2].at(0), pointCoordinates[2].at(1) };

        float vectorX = knownEnd[0] - origin[0];
        float vectorY = knownEnd[1] - origin[1];

        float cosTheta = cos(radians);
        float sinTheta = sin(radians);

        float vectorPrimeX = (cosTheta * vectorX) - (sinTheta * vectorY);
        float vectorPrimeY = (sinTheta * vectorX) - (cosTheta * vectorY);

        flatCoordinate adjustedPosition;
        if (degreesOfChange < 0) {
            adjustedPosition.x = origin[0] - vectorPrimeX;
            adjustedPosition.y = origin[1] - vectorPrimeY;
        }
        else {
            adjustedPosition.x = origin[0] + vectorPrimeX;
            adjustedPosition.y = origin[1] + vectorPrimeY;
        }

        coordinateChanges update(newDegrees, degreesOfChange, &adjustedPosition);

        return update;

    }
};
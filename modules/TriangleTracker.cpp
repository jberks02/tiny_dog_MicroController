#include "../main.h"
using namespace std;

class TriangleTracker
{
public:
    vector<float> sideLengths = {1.f, 1.f, 1.f};
    vector<float> triangleAngles = {60.f, 60.f, 60.f};
    vector<vector<float>> pointCoordinates;

public:
    TriangleTracker(vector<vector<float>> pointCoordinates)
    {
        sideLengths[0] = calculateDistance(pointCoordinates[0], pointCoordinates[1]);
        sideLengths[1] = calculateDistance(pointCoordinates[1], pointCoordinates[2]);
        sideLengths[2] = calculateDistance(pointCoordinates[2], pointCoordinates[0]);
        this->pointCoordinates = pointCoordinates;
        createAngles();
    }
    void setNewCoordinates(vector<vector<float>> pointCoordinates)
    {
        sideLengths[0] = calculateDistance(pointCoordinates[0], pointCoordinates[1]);
        sideLengths[1] = calculateDistance(pointCoordinates[1], pointCoordinates[2]);
        sideLengths[2] = calculateDistance(pointCoordinates[2], pointCoordinates[0]);
        createAngles();
    }
    float calculateArcCosineDegrees(float a, float b, float c)
    {
        // broken out into steps to make readable
        float numerator = pow(a, 2) + pow(b, 2) - pow(c, 2);
        float denominator = 2 * a * b;
        float divisionResult = numerator / denominator;
        float degreeOfAdjacentAngle = acos(divisionResult);
        float degreeConversion = degreeOfAdjacentAngle * (180 / 3.1415);
        return degreeConversion; // * (180 / 3.1415) turns radians into degrees.
    }

    void createAngles()
    {
        // arccosine calculations are the same for all angles with a reordering of the sides.
        // therefore we can use one function and simply replace the variables.
        float angleC = calculateArcCosineDegrees(sideLengths[0], sideLengths[1], sideLengths[2]);
        triangleAngles[2] = angleC;
        float angleB = calculateArcCosineDegrees(sideLengths[0], sideLengths[2], sideLengths[1]);
        triangleAngles[1] = angleB;
        float angleA = calculateArcCosineDegrees(sideLengths[1], sideLengths[2], sideLengths[0]);
        triangleAngles[0] = angleA;
    }

public:
    void setSideALength(float len)
    {
        sideLengths[0] = len;
        createAngles();
    }

    void setSideBLength(float len)
    {
        sideLengths[1] = len;
        createAngles();
    }

    void setSideCLength(float len)
    {
        sideLengths[2] = len;
        createAngles();
    }

    float returnNewCangle(float a, float b, float c)
    {
        // broken out into steps to make readable
        double numerator = pow(a, 2) + pow(b, 2) - pow(c, 2);
        double denominator = 2 * a * b;
        double divisionResult = numerator / denominator;
        float degreeOfAdjacentAngle = acos(divisionResult);
        float degreeConversion = degreeOfAdjacentAngle * (180 / 3.1415);
        return degreeConversion; // * (180 / 3.1415) turns radians into degrees.
    }
    void getNewEndEffectorCoordinate(vector<float> *cCordinate, float oldDistance)
    {
        // Degrees × (π / 180) = Radians
        // Radians  × (180 / π) = Degrees
        // float oppositeAngle;
        float xOffset;
        float yOffset;
        float oppositeAngle;

        vector<float> coordinates = {0.f, 0.f, 0.f};

        if (sideLengths[2] > oldDistance)
        {
            // get angle used for sin calculations in degrees
            oppositeAngle = 180 - (90 + triangleAngles[1]);
            // convert to radians
            oppositeAngle = oppositeAngle * (M_PI / 180);
        }
        else
        {
            // create angle adjacent to known angle 2 and 90 degree angle
            float angle4 = 180 - triangleAngles[1];
            // get new opposite angle
            oppositeAngle = 180 - (90 + angle4);
            // convert to radians
            oppositeAngle = oppositeAngle * (M_PI / 180);
        }

        xOffset = sin(oppositeAngle) * sideLengths[2];
        yOffset = sqrt(pow(sideLengths[2], 2) - pow(xOffset, 2));

        coordinates[0] = sideLengths[2] > oldDistance ? 0 - xOffset : 0 + xOffset;
        coordinates[1] = 110 - yOffset;

        cCordinate->swap(coordinates);
    }
};
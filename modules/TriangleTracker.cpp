#include "../main.h"
using namespace std;

class TriangleTracker
{
public:
    int endAffectorEndex = 2;

    double sideLengths[3] = {1.0, 1.0, 1.0};

    double triangleAngles[3] = {60.0, 60.0, 60.0};

public:
    TriangleTracker(double a, double b, double c)
    {
        sideLengths[0] = a;
        sideLengths[1] = b;
        sideLengths[2] = c;
        createAngles();
    }

private:
    double calculateDistance(int positionOne[2], int positionTwo[2])
    {
        int subtractOne = positionOne[0] - positionTwo[0];
        int subtractTwo = positionOne[1] - positionTwo[1];
        int squaredOne = subtractOne * subtractOne;
        int squareTwo = subtractTwo * subtractTwo;
        double finalBeforeSquare = (double)squaredOne + (double)squareTwo;
        return sqrt(finalBeforeSquare);
    }

    float calculateArcCosineDegrees(double a, double b, double c)
    {
        // broken out into steps to make readable
        double numerator = pow(a, 2) + pow(b, 2) - pow(c, 2);
        double denominator = 2 * a * b;
        double divisionResult = numerator / denominator;
        double degreeOfAdjacentAngle = acos(divisionResult);
        double degreeConversion = degreeOfAdjacentAngle * (180 / 3.1415);
        return degreeConversion; // * (180 / 3.1415) turns radians into degrees.
    }

    void createAngles()
    {
        // arccosine calculations are the same for all angles with a reordering of the sides.
        // therefore we can use one function and simply replace the variables.
        double angleC = calculateArcCosineDegrees(sideLengths[0], sideLengths[1], sideLengths[2]);
        triangleAngles[2] = angleC;
        double angleB = calculateArcCosineDegrees(sideLengths[0], sideLengths[2], sideLengths[1]);
        triangleAngles[1] = angleB;
        double angleA = calculateArcCosineDegrees(sideLengths[1], sideLengths[2], sideLengths[0]);
        triangleAngles[0] = angleA;
    }

public:
    void setSideALength(double len)
    {
        sideLengths[0] = len;
        createAngles();
    }

    void setSideBLength(double len)
    {
        sideLengths[1] = len;
        createAngles();
    }

    void setSideCLength(double len)
    {
        sideLengths[2] = len;
        createAngles();
    }

    float returnNewCangle(int a, int b, int c)
    {
        // broken out into steps to make readable
        double numerator = pow(a, 2) + pow(b, 2) - pow(c, 2);
        double denominator = 2 * a * b;
        double divisionResult = numerator / denominator;
        float degreeOfAdjacentAngle = acos(divisionResult);
        float degreeConversion = degreeOfAdjacentAngle * (180 / 3.1415);
        return degreeConversion; // * (180 / 3.1415) turns radians into degrees.
    }
    void getNewEndEffectorCoordinate(vector<int> *cCordinate, double oldDistance)
    {
        // Degrees × (π / 180) = Radians
        // Radians  × (180 / π) = Degrees
        // float oppositeAngle;
        float xOffset;
        float yOffset;
        float oppositeAngle;

        vector<int> coordinates = {0, 0};

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
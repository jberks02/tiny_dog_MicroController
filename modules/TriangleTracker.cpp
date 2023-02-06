#include "../main.h"
using namespace std;

class TriangleTracker {
    public: int endAffectorEndex = 2;
    public: double sideLengths[3] = {1.0, 1.0, 1.0};
    public: double triangleAngles[3] = {60.0, 60.0, 60.0};
    public: TriangleTracker(double a, double b, double c) {
        sideLengths[0] = a;
        sideLengths[1] = b;
        sideLengths[2] = c;
        createAngles();
    }
    private: double calculateDistance(int positionOne[2], int positionTwo[2]) {
        int subtractOne = positionOne[0] - positionTwo[0];
        int subtractTwo = positionOne[1] - positionTwo[1];
        int squaredOne = subtractOne * subtractOne;
        int squareTwo = subtractTwo * subtractTwo;
        double finalBeforeSquare = (double)squaredOne + (double)squareTwo;
        return sqrt(finalBeforeSquare);
    }
    private: float calculateArcCosineDegrees(double a, double b, double c) {
        //broken out into steps to make readable
        double numerator = pow(a,2) + pow(b,2) - pow(c,2);
        double denominator = 2 * a * b;
        double divisionResult = numerator/denominator;
        double degreeOfAdjacentAngle = acos(divisionResult);
        double degreeConversion = degreeOfAdjacentAngle * (180 / 3.1415);
        return degreeConversion; // * (180 / 3.1415) turns radians into degrees. 
    }
    private: void createAngles() {
        //arccosine calculations are the same for all angles with a reordering of the sides. 
        //therefore we can use one function and simply replace the variables.
        double angleC = calculateArcCosineDegrees(sideLengths[0], sideLengths[1], sideLengths[2]);
        triangleAngles[2] = angleC;
        double angleB = calculateArcCosineDegrees(sideLengths[0], sideLengths[2], sideLengths[1]);
        triangleAngles[1] = angleB;
        double angleA = calculateArcCosineDegrees(sideLengths[1], sideLengths[2], sideLengths[0]);
        triangleAngles[0] = angleA;
    }
    public: void setSideALength(double len) {
        sideLengths[0] = len;
        createAngles();
    }
    public: void setSideBLength(double len) {
        sideLengths[1] = len;
        createAngles();
    }
    public: void setSideCLength(double len) {
        sideLengths[2] = len;
        createAngles();
    }

public:
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
};
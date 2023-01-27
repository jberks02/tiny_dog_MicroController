#include "../main.h"
using namespace std;

class TriangleTracker {
    protected: int endAffectorEndex = 2;
    protected: int sideLengths[3] = {80, 116, 96};
    protected: float triangleAngles[3] = {90.f, 30.f, 60.f};
    public: TriangleTracker(int a, int b, int c) {
        
        createAngles();
    }
    private: float calculateDistance(int positionOne[2], int positionTwo[2]) {
        int subtractOne = positionOne[0] - positionTwo[0];
        int subtractTwo = positionOne[1] - positionTwo[1];
        int squaredOne = subtractOne * subtractOne;
        int squareTwo = subtractTwo * subtractTwo;
        int finalBeforeSquare = squaredOne + squareTwo;
        return sqrt(finalBeforeSquare);
    }
    private: float calculateArcCosineDegrees(int a, int b, int c) {
        //broken out into steps to make readable
        double numerator = pow(a,2) + pow(b,2) - pow(c,2);
        double denominator = 2 * a * b;
        double divisionResult = numerator/denominator;
        float degreeOfAdjacentAngle = acos(divisionResult);
        float degreeConversion = degreeOfAdjacentAngle * (180 / 3.1415);
        return degreeConversion; // * (180 / 3.1415) turns radians into degrees. 
    }
    private: void createAngles() {
        //arccosine calculations are the same for all angles with a reordering of the sides. 
        //therefore we can use one function and simply replace the variables.
        float angleC = calculateArcCosineDegrees(sideLengths[0], sideLengths[1], sideLengths[2]);
        float angleB = calculateArcCosineDegrees(sideLengths[0], sideLengths[2], sideLengths[1]);
        float angleA = calculateArcCosineDegrees(sideLengths[1], sideLengths[2], sideLengths[0]);
        float newAngleSeries[3] = {angleA, angleB, angleC};
        memcpy(triangleAngles, newAngleSeries, 3);
    }
    protected: void setSideALength(int len) {
        sideLengths[0] = len;
        createAngles();
    }
    protected: void setSideBLength(int len) {
        sideLengths[1] = len;
        createAngles();
    }
    protected: void setSideCLength(int len) {
        sideLengths[2] = len;
        createAngles();
    }
};
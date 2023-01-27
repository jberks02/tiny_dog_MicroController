#include "../main.h"
using namespace std;
//(0,125,0) default position of rotation point one
// default second rotation point coordinate (0, 125, -33)
// default coordinate (0,0,0)
//{(0, 125), (-80,125), (0,0)} default xy plane position
//{(125, -33), (125, 0), (0, 0)} default yz plane position
//{90,90,90} default servo angles
//{'r', 'l' 'r'} default servo movement values for rotate, linear, rotate
//{(0,125,0), (0,125,0), (0,125,-33)} default servo coordinates
class ExtensionTracker
{
private:
    int yz[3][2];

private:
    int xy[3][2];
    // in order of x,y,z
private:
    int coordinate[3];

private:
    float ServoAngles[3];

private:
    char servoMovements[3];

public:
    ExtensionTracker(
        int yzPlane[3][2],
        int xyPlane[3][2],
        int defaultCoordinate[3],
        float defaultServoAngles[3],
        char servoMovementTypes[3], 
        int servoCoordinates[3][3])
    {
        memcpy(yz, yzPlane, 2);
        memcpy(xy, xyPlane, 2);
        memcpy(coordinate, defaultCoordinate, 3);
        memcpy(ServoAngles, defaultServoAngles, 3);
        memcpy(servoMovements, servoMovementTypes, 3);
    }
    // always pass with ordering with x, then y
private:
    double calculateDistance(int positionOne[2], int positionTwo[2])
    {
        int subtractOne = positionOne[0] - positionTwo[0];
        int subtractTwo = positionOne[1] - positionTwo[1];
        double squaredOne = subtractOne * subtractOne;
        double squareTwo = subtractTwo * subtractTwo;
        double finalBeforeSquare = squaredOne + squareTwo;
        return sqrt(finalBeforeSquare);
    }
    // spindle angle conversion
private:
    float convertCAngleOnNormalTriangle(float armAngle)
    {
        float squaredArmAngle = pow(armAngle, 2);
        float a = squaredArmAngle * 0.085572322414428;
        float b = squaredArmAngle * 7.1963562753036;
        float c = 142.65734265734;
        return a - b + c;
    }
};

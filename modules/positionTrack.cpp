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
    vector<vector<int>> yz;

private:
    vector<vector<int>> xy;
private:
    vector<int> coordinate;

private:
    vector<float> ServoAngles;

private:
    vector<char> servoMovements;

private:
    TriangleTracker xyTriangle = TriangleTracker(1.0, 1.0, 1.0);

private:
    TriangleTracker yzTriangle = TriangleTracker(1.0, 1.0, 1.0);

public: ExtensionTracker(
        vector<vector<int>> yzPlane,
        vector<vector<int>> xyPlane,
        vector<int> defaultCoordinate,
        vector<float> defaultServoAngles,
        vector<char> servoMovementTypes,
        vector<vector<int>> servoCoordinates)
    {
        yz = yzPlane;
        xy = xyPlane;
        coordinate = defaultCoordinate;
        ServoAngles = defaultServoAngles;
        servoMovements = servoMovementTypes;
        double xyA = calculateDistance(xyPlane[0], xyPlane[1]);
        double xyB = calculateDistance(xyPlane[1], xyPlane[2]);
        double xyC = calculateDistance(xyPlane[2], xyPlane[0]);
        xyTriangle.setSideALength(xyA);
        xyTriangle.setSideBLength(xyB);
        xyTriangle.setSideCLength(xyC);
        double yzA = calculateDistance(yzPlane[0], yzPlane[1]);
        double yzB = calculateDistance(yzPlane[1], yzPlane[2]);
        double yzC = calculateDistance(yzPlane[2], yzPlane[0]);
        yzTriangle.setSideALength(yzA);
        yzTriangle.setSideBLength(yzB);
        yzTriangle.setSideCLength(yzC);
    }
    // always pass with ordering with x, then y
private:
    double calculateDistance(vector<int> positionOne, vector<int> positionTwo)
    {
        int subtractOne = positionOne[0] - positionTwo[0];
        int subtractTwo = positionOne[1] - positionTwo[1];
        double squaredOne = subtractOne * subtractOne;
        double squareTwo = subtractTwo * subtractTwo;
        double finalBeforeSquare = squaredOne + squareTwo;
        return sqrt(finalBeforeSquare);
    }
//     // spindle angle conversion
private:
    float convertCAngleOnNormalTriangle(float armAngle)
    {
        double squaredArmAngle = pow(armAngle, 2.0);
        double a = squaredArmAngle * (-0.020358651916202);
        double b = armAngle * 6.7477746344583;
        double c = -214.89571246474;
        double product = a+b+c;
        return product;
    };
float findHypotneuse(double a, double b) {
    double aSquared = pow(a, 2.0);
    double bSquared = pow(b, 2.0);
    float cSquared = aSquared + bSquared;
    float hypotenuese = sqrt(cSquared);
    return hypotenuese;
}
public:
    void getServoAnglesForPoint(vector<int> newEndEffectorPoint, vector<float> *newServoAngles)
    {
        newServoAngles->clear();
        double floatContainer[3] = {0.0, 0.0, 0.0};
        double xydistanceToOldPoint = xyTriangle.sideLengths[2];
        // First new Servo Angle Set for servo 3
        // vector<float> newServoAngles = {0.f, 0.f, 0.f};
        vector<int> xyNewCoordinate = {newEndEffectorPoint[0], newEndEffectorPoint[1]};
        double newXYcLength = calculateDistance(xy[0], xyNewCoordinate);
        xyTriangle.setSideCLength(newXYcLength);
        floatContainer[2] = convertCAngleOnNormalTriangle(xyTriangle.triangleAngles[2]);
        vector<int> standardZero = {0,0};
        // vector<int> endEffectorPointTranslation;
        xyTriangle.getNewEndEffectorCoordinate(&standardZero, xydistanceToOldPoint);
        // set second new servo angle for servo 0
        // use new XYcLength is used for side a of new Triangle
        //  use existing a length as b length
        //  c is the distance from the old point to the new point
        double distanceBetweenNewAndOldXYend = calculateDistance(standardZero, xyNewCoordinate);
        // angle c is contained within this triangle.
        if (distanceBetweenNewAndOldXYend > 0.0 & standardZero[0] != 0)
        {
            TriangleTracker servoOneRotationTriangle(newXYcLength, xydistanceToOldPoint, distanceBetweenNewAndOldXYend);
            cout << to_string(newXYcLength) << to_string(distanceBetweenNewAndOldXYend) << endl;
            if (xyNewCoordinate[0] > 0)
            {
                floatContainer[0] = ServoAngles[0] + servoOneRotationTriangle.triangleAngles[2];
            }
            else
            {
                floatContainer[0] = ServoAngles[0] - servoOneRotationTriangle.triangleAngles[2];
            }
        }
        else
        {
            floatContainer[0] = ServoAngles[0];
        }
        // set new servo angle for servo 2, z axis
        //  a is distance to new point, b is distance to old point, c is distance between new and old
        vector<int> newYZcoordinate = {newEndEffectorPoint[1], newEndEffectorPoint[2]};
        double distanceToNewPoint = calculateDistance(yz[0], newYZcoordinate);
        double distanceToOldPoint = yzTriangle.sideLengths[1];
        double distanceBetweenPoints = calculateDistance(newYZcoordinate, yz[2]);
        if (distanceBetweenPoints > 0.0 & newEndEffectorPoint[2] != 0)
        {
            TriangleTracker servoTwoRoationTriangle(distanceToNewPoint, distanceToOldPoint, distanceBetweenPoints);
            if (newYZcoordinate[1] > coordinate[2])
            {
                floatContainer[1] = ServoAngles[1] + servoTwoRoationTriangle.triangleAngles[2];
            }
            else
            {
                floatContainer[1] = ServoAngles[1] - servoTwoRoationTriangle.triangleAngles[2];
            }
            if(distanceToNewPoint > newXYcLength) {
                xyTriangle.setSideCLength(distanceToNewPoint);
                floatContainer[2] = convertCAngleOnNormalTriangle(xyTriangle.triangleAngles[2]);
            }
        }
        else
        {
            floatContainer[1] = ServoAngles[1];
        }
        
        for(int i = 0; i < 3; i++) {
            newServoAngles->push_back(floatContainer[i]);
        };
        // newServoAngles->assign(floatContainer, floatContainer + 3);

        // xyTriangle.setSideCLength(calculateDistance(xy[0], xy[2]));
    };
};

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
    vector<vector<float>> yz;
    vector<vector<float>> xy;
    vector<float> coordinate;
    vector<float> ServoAngles;
    TriangleTracker xyTriangle = TriangleTracker({{2.f, 0.f}, {1.f, 2.f}, {0.f, 0.f}});
    TriangleTracker yzTriangle = TriangleTracker({{2.f, 0.f}, {1.f, 2.f}, {0.f, 0.f}});
    vector<PositioningServo> mServos;
    vector<MovementSeries> MovementSets;

public:
    ExtensionTracker(
        vector<vector<float>> yzPlane,
        vector<vector<float>> xyPlane,
        vector<float> defaultCoordinate,
        // x, y z ordering on servos
        vector<PositioningServo> servos)
    {
        yz = yzPlane;
        xy = xyPlane;
        mServos = servos;
        coordinate = defaultCoordinate;
        xyTriangle.setNewCoordinates(xyPlane);
        yzTriangle.setNewCoordinates(yzPlane);
    }
    // always pass with ordering with x, then y
private:
    float calculateDistance(vector<float> positionOne, vector<float> positionTwo)
    {
        float subtractOne = positionOne[0] - positionTwo[0];
        float subtractTwo = positionOne[1] - positionTwo[1];
        float squaredOne = subtractOne * subtractOne;
        float squareTwo = subtractTwo * subtractTwo;
        float finalBeforeSquare = squaredOne + squareTwo;
        return sqrt(finalBeforeSquare);
    }
    float findRotationTranslation(vector<float> rotationPoint, vector<float> destination, vector<float> origin)
    {
        float sideA = calculateDistance(rotationPoint, destination);
        float sideB = calculateDistance(rotationPoint, origin);
        float sideC = calculateDistance(origin, destination);
        float cAngle = calculateCAngleFromSides(sideA, sideB, sideC);
        return cAngle;
    }
    // linear translations are only allowed for y axis and c side of triangle ensure
    float findLinearTranslation(TriangleTracker *tri, float newLength, vector<float> *EEnewCoordinate)
    {
        float previousDistanceToEndEffector;
        previousDistanceToEndEffector = tri->sideLengths[2];
        tri->setSideCLength(newLength);
        tri->getNewEndEffectorCoordinate(EEnewCoordinate, previousDistanceToEndEffector);
        return tri->triangleAngles[2];
    }

public:
    void addMovementSeries(MovementSeries newMove)
    {
        MovementSets.push_back(newMove);
    }
    void getServoAnglesForPoint(vector<float> newEndEffectorPoint, vector<float> *newServoAngles)
    {
        // set up vars that will be needed throughout
        newServoAngles->clear();
        float floatContainer[3] = {0.f, 0.f, 0.f};
        vector<float> xyNewCoordinate = {newEndEffectorPoint[0], newEndEffectorPoint[1]};
        vector<float> newYZcoordinate = {newEndEffectorPoint[1], newEndEffectorPoint[2]};
        vector<float> standardZero(coordinate);
        // sum of rotations will always find a good result, linear translations however will not so those will have to be done first,
        float newXYcLength = calculateDistance(xy[0], xyNewCoordinate);
        float newyzEndYPlaneLength = calculateDistance(yz[1], newYZcoordinate);
        float linearTranslationLength = newXYcLength > newyzEndYPlaneLength ? newXYcLength : newyzEndYPlaneLength;
        floatContainer[mServos[1].servoIndex] = findLinearTranslation(&xyTriangle, linearTranslationLength, &standardZero);
        floatContainer[mServos[1].servoIndex] = mServos[1].convert(standardZero[1], xyNewCoordinate[1], floatContainer[mServos[1].servoIndex]);
        // ensure that relevant points need rotation before starting process;
        float currentAndDestinationXcoordinateDifference = standardZero[0] - xyNewCoordinate[0];
        if (currentAndDestinationXcoordinateDifference != 0)
        {
            floatContainer[mServos[0].servoIndex] = findRotationTranslation(mServos[0].servoPosition, xyNewCoordinate, standardZero);
            floatContainer[mServos[0].servoIndex] = mServos[0].convert(standardZero[0], xyNewCoordinate[0], floatContainer[mServos[0].servoIndex]);
        }
        else
        {
            floatContainer[mServos[0].servoIndex] = mServos[0].defaultAngle;
        }
        float curAndDestDifferenceOnZ = newYZcoordinate[1] - standardZero[2];
        if (curAndDestDifferenceOnZ != 0)
        {
            floatContainer[mServos[2].servoIndex] = findRotationTranslation({mServos[2].servoPosition[1], mServos[2].servoPosition[2]}, newYZcoordinate, {coordinate[1], coordinate[2]});
            floatContainer[mServos[2].servoIndex] = mServos[2].convert(standardZero[2], newYZcoordinate[1], floatContainer[mServos[2].servoIndex]);
        }
        else
        {
            floatContainer[mServos[2].servoIndex] = mServos[2].defaultAngle;
        }
        for (int i = 0; i < 3; i++)
        {
            newServoAngles->push_back(floatContainer[i]);
        };

        xyTriangle.setSideCLength(calculateDistance(xy[0], xy[2]));
    }
};

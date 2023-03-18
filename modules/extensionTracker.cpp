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
    vector<float> ServoAngles;
    TriangleTracker xyTriangle = TriangleTracker({{2.f, 0.f}, {1.f, 2.f}, {0.f, 0.f}});
    TriangleTracker yzTriangle = TriangleTracker({{2.f, 0.f}, {1.f, 2.f}, {0.f, 0.f}});
    // vector<servoCommand> servoCalls;
    uint64_t startUp = time_us_64();
    vector<extensionCommand> extensionCalls;
    vector<float> coordinate;
public:
    vector<PositioningServo> mServos;
    vector<MovementSeries> MovementSets;
    vector<float> currentPosition;
public:
    string name;

public:
    ExtensionTracker(
        string name,
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
        this->name = name;
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
    void addNewExtensionCommand(extensionCommand newCoordinate)
    {

        uint64_t nextIndex = extensionCalls.size();

        uint64_t runTime = newCoordinate.postDelay * (nextIndex > 0 ? nextIndex : 1) * 1000;

        newCoordinate.executionMoment = startUp + runTime;

        extensionCalls.push_back(newCoordinate);
    }
    void prepareCommands(string name, int iterations)
    {
        startUp = time_us_64();
        for (auto set : MovementSets)
        {
            if (set.name == name)
            {
                for (int i = 0; i < iterations; i++)
                {
                    for (auto &coord : set.series)
                    {
                        extensionCommand com(set.name, coord, set.millisecondDelay);
                        addNewExtensionCommand(com);
                    }
                }
            }
        }
    }
    void checkAndSetNewCoordinate()
    {
        uint64_t now = time_us_64();

        if (extensionCalls.size() == 0) {
            return;
        };
            

        uint64_t nextCall = extensionCalls[0].executionMoment;

        if (now > nextCall)
        {
            currentPosition = extensionCalls[0].coordinate;
            getServoAnglesForPoint(extensionCalls[0].coordinate);
            extensionCalls.erase(extensionCalls.begin());
        };
    }
    void addMovementSeries(MovementSeries newMove)
    {
        MovementSets.push_back(newMove);
    }
    void getServoAnglesForPoint(vector<float> newEndEffectorPoint)
    {
        // set up vars that will be needed throughout
        float floatContainer[3] = {0.f, 0.f, 0.f};
        vector<float> xyNewCoordinate = {newEndEffectorPoint[0], newEndEffectorPoint[1]};
        vector<float> newYZcoordinate = {newEndEffectorPoint[1], newEndEffectorPoint[2]};
        vector<float> standardZero(coordinate);
        // sum of rotations will always find a good result, linear translations however will not so those will have to be done first,
        float newXYcLength = calculateDistance(xy[0], xyNewCoordinate);
        float newyzEndYPlaneLength = calculateDistance(yz[1], newYZcoordinate);
        float linearTranslationLength = newXYcLength > newyzEndYPlaneLength ? newXYcLength : newyzEndYPlaneLength;
        floatContainer[1] = findLinearTranslation(&xyTriangle, linearTranslationLength, &standardZero);
        floatContainer[1] = mServos[1].convert(standardZero[1], xyNewCoordinate[1], floatContainer[1]);
        // ensure that relevant points need rotation before starting process;
        float currentAndDestinationXcoordinateDifference = standardZero[0] - xyNewCoordinate[0];
        if (currentAndDestinationXcoordinateDifference != 0)
        {
            floatContainer[0] = findRotationTranslation(mServos[0].servoPosition, xyNewCoordinate, standardZero);
            floatContainer[0] = mServos[0].convert(standardZero[0], xyNewCoordinate[0], floatContainer[0]);
        }
        else
        {
            floatContainer[0] = mServos[0].defaultAngle;
        }
        float curAndDestDifferenceOnZ = newYZcoordinate[1] - standardZero[2];
        if (curAndDestDifferenceOnZ != 0)
        {
            floatContainer[2] = findRotationTranslation({mServos[2].servoPosition[1], mServos[2].servoPosition[2]}, newYZcoordinate, {coordinate[1], coordinate[2]});
            floatContainer[2] = mServos[2].convert(standardZero[2], newYZcoordinate[1], floatContainer[2]);
        }
        else
        {
            floatContainer[2] = mServos[2].defaultAngle;
        }
        for (int i = 0; i < 3; i++)
        {
            mServos[i].currentAngle = floatContainer[i];
        };

        xyTriangle.setSideCLength(calculateDistance(xy[0], xy[2]));
    }
};

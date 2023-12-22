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
class ExtensionTracker {
    private:
    vector<vector<float>> yz;
    vector<vector<float>> xz;
    vector<float> ServoAngles;
    TriangleTracker xzTriangle = TriangleTracker({ {2.f, 0.f}, {1.f, 2.f}, {0.f, 0.f} });
    TriangleTracker yzTriangle = TriangleTracker({ {2.f, 0.f}, {1.f, 2.f}, {0.f, 0.f} });
    // vector<servoCommand> servoCalls;
    uint64_t startUp = time_us_64();
    vector<float> coordinate;

    public:
    vector<PositioningServo> mServos;
    vector<MovementSeries> MovementSets;
    vector<float> currentPosition;
    vector<extensionCommand> extensionCalls;

    public:
    string name;

    public:
    ExtensionTracker(
        string name,
        vector<float> defaultCoordinate,
        // x, y z ordering on servos
        vector<PositioningServo> servos) {
        yz = {
        {servos[1].servoPosition[1], servos[1].servoPosition[2]},
        {servos[2].servoPosition[1], servos[2].servoPosition[2]},
        {defaultCoordinate[1], defaultCoordinate[2]}
        };
        xz = {
        {servos[0].servoPosition[0], servos[0].servoPosition[2]},
        {servos[1].servoPosition[0], servos[0].servoPosition[2]},
        {defaultCoordinate[0], defaultCoordinate[2]}
        };
        mServos = servos;
        coordinate = defaultCoordinate;
        xzTriangle.setNewCoordinates(xz);
        yzTriangle.setNewCoordinates(yz);
        this->name = name;
    }
    // always pass with ordering with x, then y
    private:
    float calculateDistance(vector<float> positionOne, vector<float> positionTwo) {
        float subtractOne = positionOne[0] - positionTwo[0];
        float subtractTwo = positionOne[1] - positionTwo[1];
        float squaredOne = subtractOne * subtractOne;
        float squareTwo = subtractTwo * subtractTwo;
        float finalBeforeSquare = squaredOne + squareTwo;
        return sqrt(finalBeforeSquare);
    }
    float findRotationTranslation(vector<float> rotationPoint, vector<float> destination, vector<float> origin) {
        float sideA = calculateDistance(rotationPoint, destination);
        float sideB = calculateDistance(rotationPoint, origin);
        float sideC = calculateDistance(origin, destination);
        float cAngle = calculateCAngleFromSides(sideA, sideB, sideC);
        return cAngle;
    }
    // linear translations are only allowed for z axis and c side of triangle ensure
    coordinateChanges findLinearTranslation(TriangleTracker* tri, float newCSideLength, flatCoordinate* desiredPosition) {
        return tri->calculateNewCoordinateChanges(newCSideLength, desiredPosition);
    }

    public:
    void addNewExtensionCommand(extensionCommand newCoordinate) {

        uint64_t nextIndex = extensionCalls.size();

        uint64_t runTime = newCoordinate.postDelay * (nextIndex > 0 ? nextIndex : 1) * 1000;

        newCoordinate.executionMoment = startUp + runTime;

        extensionCalls.push_back(newCoordinate);
    }
    void prepareCommands(string name, int iterations) {
        startUp = time_us_64();
        for (auto set : MovementSets) {
            if (set.name == name) {
                for (int i = 0; i < iterations; i++) {
                    for (auto& coord : set.series) {
                        extensionCommand com(set.name, coord, set.millisecondDelay);
                        addNewExtensionCommand(com);
                    }
                }
            }
        }
    }
    void checkAndSetNewCoordinate() {
        uint64_t now = time_us_64();

        if (extensionCalls.size() == 0) {
            return;
        };

        uint64_t nextCall = extensionCalls[0].executionMoment;
        float servoAngles[3] = { mServos[0].currentAngle, mServos[1].currentAngle, mServos[2].currentAngle };
        if (now > nextCall) {
            currentPosition = extensionCalls[0].coordinate;
            setMotorAnglesForNewPoint(&extensionCalls[0].coordinate);
            extensionCalls.erase(extensionCalls.begin());
            cout << servoAngles[0] << servoAngles[1] << servoAngles[2] << endl;
        };
    }
    void addMovementSeries(MovementSeries newMove) {
        MovementSets.push_back(newMove);
    }
    void setMotorAnglesForNewPoint(vector<float>* newPoint) {
        vector<float> oldXzCoordinate = { coordinate.at(0), coordinate.at(2) };
        vector<float> oldYzCoordinate = { coordinate.at(1), coordinate.at(2) };
        vector<float> newXzCoordinate = { newPoint->at(0), newPoint->at(2) };
        vector<float> newYzCoordinate = { newPoint->at(1), newPoint->at(2) };
        flatCoordinate standardZero(coordinate[1], coordinate[2]);

        float newXDistance = 0.f;
        if (newPoint->at(0) != coordinate[0]) {
            vector<float> xServoXZPosition = { mServos[0].servoPosition[0], mServos[0].servoPosition[2] };
            float newXSweepArmAngle = findRotationTranslation(xServoXZPosition, newXzCoordinate, coordinate);
            newXDistance = calculateDistance(xServoXZPosition, newXzCoordinate);
            //Assigning X Servo
            mServos[0].currentAngle = mServos[0].convert(coordinate[0], newXzCoordinate[0], newXSweepArmAngle);
        }
        else mServos[0].currentAngle = mServos[0].defaultAngle;

        vector<float> yzRotationOrigin = { mServos[1].servoPosition[1], mServos[1].servoPosition[2] };
        float distanceToNewPoint = calculateDistance(yzRotationOrigin, newYzCoordinate);
        float newLength = distanceToNewPoint > newXDistance ? distanceToNewPoint : newXDistance;
        coordinateChanges movementUpdates = yzTriangle.calculateNewCoordinateChanges(newLength, &standardZero);
        //Z Servo Assignment
        mServos[2].currentAngle = mServos[2].convert(coordinate[2], newPoint->at(2), movementUpdates.rotationTotal);//abs(newZAngle - mServos[2].defaultAngle));

        float newYAngle = findRotationTranslation(yzRotationOrigin, newYzCoordinate, { movementUpdates.newPosition.x, movementUpdates.newPosition.y });
        //Y Servo Assignment
        mServos[1].currentAngle = mServos[1].convert(coordinate[1], newPoint->at(1), newYAngle);//(newYAngle - mServos[1].defaultAngle));// - mServos[1].defaultAngle

    }
};

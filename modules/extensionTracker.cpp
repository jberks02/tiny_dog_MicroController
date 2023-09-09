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
        vector<vector<float>> yzPlane,
        vector<vector<float>> xzPlane,
        vector<float> defaultCoordinate,
        // x, y z ordering on servos
        vector<PositioningServo> servos) {
        yz = yzPlane;
        xz = xzPlane;
        mServos = servos;
        coordinate = defaultCoordinate;
        xzTriangle.setNewCoordinates(xzPlane);
        yzTriangle.setNewCoordinates(yzPlane);
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
    float findLinearTranslation(TriangleTracker* tri, float newLength, vector<float>* EEnewCoordinate) {
        float previousDistanceToEndEffector;
        previousDistanceToEndEffector = tri->sideLengths[2];
        tri->setSideCLength(newLength);
        tri->getNewEndEffectorCoordinate(EEnewCoordinate, previousDistanceToEndEffector);
        return tri->triangleAngles[2];
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
        vector<float> standardZero(coordinate);

        float additionalArmLengthRequiredFromXSwing = 0.f;
        if (newPoint->at(0) != coordinate[0]) {
            vector<float> xServoXZPosition = { mServos[0].servoPosition[0], mServos[0].servoPosition[2] };
            float newXSweepArmAngle = findRotationTranslation(xServoXZPosition, newXzCoordinate, coordinate);
            float previousDistance = calculateDistance(mServos[0].servoPosition, oldXzCoordinate);
            float newDistance = calculateDistance(mServos[0].servoPosition, newXzCoordinate);
            additionalArmLengthRequiredFromXSwing = previousDistance - newDistance;
            //Assigning X Servo
            mServos[0].currentAngle = mServos[0].convert(coordinate[0], newXzCoordinate[0], newXSweepArmAngle);
        }
        else mServos[0].currentAngle = mServos[0].defaultAngle;

        vector<float> originForArmLengthCalculation = { mServos[1].servoPosition[1], mServos[1].servoPosition[2] };
        float distanceToNewPoint = calculateDistance(originForArmLengthCalculation, newYzCoordinate);
        float newLength = distanceToNewPoint > additionalArmLengthRequiredFromXSwing ? distanceToNewPoint : additionalArmLengthRequiredFromXSwing;
        if (newLength != yzTriangle.sideLengths[2]) {
            float newZAngle = findLinearTranslation(&yzTriangle, newLength, &standardZero);
            //Z Servo Assignment
            mServos[2].currentAngle = mServos[2].convert(coordinate[2], newYzCoordinate[2], abs(newZAngle - mServos[2].defaultAngle));
        }
        else mServos[2].currentAngle = mServos[2].defaultAngle;


        vector<float> originForArmYSwingCalculation = { mServos[1].servoPosition[1], mServos[1].servoPosition[2] };
        float newYAngle = findRotationTranslation(originForArmYSwingCalculation, newYzCoordinate, oldYzCoordinate);
        //Y Servo Assignment
        mServos[1].currentAngle = mServos[1].convert(standardZero[0], newYzCoordinate[0], newYAngle);
    }
};

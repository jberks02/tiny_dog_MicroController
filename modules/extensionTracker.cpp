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
    uint64_t startUp = time_us_64();
    vector<float> coordinate;
    float link1;
    float link2;
    float link3;
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
        vector<PositioningServo> servos) {
        mServos = servos;
        coordinate = defaultCoordinate;
        this->name = name;
        link1 = calculateDistance({ servos[0].servoPosition[1], servos[0].servoPosition[2] }, { servos[1].servoPosition[1], servos[1].servoPosition[2] });
        link2 = calculateDistance({ servos[1].servoPosition[1], servos[1].servoPosition[2] }, { servos[2].servoPosition[1], servos[2].servoPosition[2] });
        link3 = calculateDistance({ servos[2].servoPosition[1], servos[2].servoPosition[2] }, { defaultCoordinate[1], defaultCoordinate[2] });
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
            threeDOFInverseKinematics(&extensionCalls[0].coordinate);
            extensionCalls.erase(extensionCalls.begin());
        };
    }
    void addMovementSeries(MovementSeries newMove) {
        MovementSets.push_back(newMove);
    }
    void threeDOFInverseKinematics(vector<float>* newPoint) {
        float xzViewVectors[2] = { abs((newPoint->at(2) - mServos[0].servoPosition[2])), abs((newPoint->at(0) - mServos[0].servoPosition[0])) };
        float yTotalDistance = abs(newPoint->at(1) - mServos[0].servoPosition[1]);
        // Step 1
        // theta one is returned in radians
        float thetaOne = convertRadiansToDegrees(inverseTangentOfDivision(xzViewVectors[1], xzViewVectors[0]));
        float r1 = hypotenuse(xzViewVectors[0], xzViewVectors[1]);
        float r2 = yTotalDistance - link1;
        // Step 2
        float r3 = hypotenuse(r1, r2);
        if (r3 > link2 + link3) r3 = link2 + link3 - 1;
        // returns in radians
        float theeTwo = inverseTangentOfDivision(r2, r1);
        //Step 3
        float theeThree = triangleAngleUsingInverseCosine(r3, link2, link3);
        float theeOne = triangleAngleUsingInverseCosine(link3, link2, r3);
        //Step 4
        float thetaThree = 180.f - convertRadiansToDegrees(theeThree);
        float thetaTwo = convertRadiansToDegrees(theeTwo - theeOne);
        //rectify thetas to servo Motors
        mServos[0].currentAngle = newPoint->at(0) > coordinate[0] ? 90.f - thetaOne : 90.f + thetaOne;
        // mServos[1].currentAngle = 90.f + thetaTwo;
        mServos[1].currentAngle = abs(thetaTwo);
        mServos[2].currentAngle = abs(thetaThree);
        cout << thetaOne << thetaTwo << thetaThree << r1 << r2 << r3 << theeOne << theeTwo << theeThree << endl;
    }
};

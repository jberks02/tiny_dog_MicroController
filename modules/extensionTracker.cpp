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
    //Refer to readme, inverse kinematic diagram and values match the summary in the modeling section
    //b in the diagram is always zero because our x motor is our point of origin. 
    void threeDOFInverseKinematics(vector<float>* newPoint) {
        //Calculate Quotient One
        flatCoordinate xyPlaneCoordinate(newPoint->at(0), newPoint->at(1));
        float motorZeroAngle = inverseTangent2(&xyPlaneCoordinate);
        //Calculate theta 3 and establish y z plane values;
        float PQ = hypotenuse(xyPlaneCoordinate.x, xyPlaneCoordinate.y);
        float SQ = newPoint->at(2) - link1;
        float PS = calculateSideViewTrianglesLongSide(&xyPlaneCoordinate, newPoint->at(2), link1);
        float motorTwoAngle = inverseCosineOfInnerTriangle(PS, link2, link3);
        // Calculate theta 2 using results from theta 3
        // theta 3 = alpha - beta
        flatCoordinate alphaSet(PQ, SQ);
        float alpha = inverseTangent2(&alphaSet);
        float s3L3 = rightTriOppositeSideLength(link3, motorTwoAngle, false);
        float c3L3 = rightTriAdjacentSideLength(link3, motorTwoAngle, false);
        flatCoordinate betaSet(link2 + c3L3, s3L3);
        float beta = inverseTangent2(&betaSet);
        float motorOneAngle = alpha - beta;
        // Assign angles
        mServos[0].currentAngle = abs(motorZeroAngle);
        mServos[1].currentAngle = 90.f + motorOneAngle;
        mServos[2].currentAngle = motorTwoAngle;
    }
};

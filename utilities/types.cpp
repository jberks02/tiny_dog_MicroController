#include "../main.h"
using namespace std;

struct ExtensionTrackerArgs {
    public:
    string name;
    vector<vector<float>> yzPlane;
    vector<vector<float>> xzPlane;
    vector<float> defaultCoordinate;
    // x, y z ordering on servos indexes
    vector<float> servos;
    ExtensionTrackerArgs(picojson::value jsonArg) {
        name = jsonArg.get("name").get<string>();
        picojson::value defaultCoordinateList = jsonArg.get("defaultCoordinate");
        picojson::value servoIntList = jsonArg.get("servos");
        parseNumberArrayToFloatVector(defaultCoordinateList, &defaultCoordinate);
        parseNumberArrayToFloatVector(servoIntList, &servos);
    }
};

struct PositioningServoArgs {
    public:
    int servoIndex;
    char movementType;
    float defaultAngle;
    vector<float> servoPosition;
    string conversionType;
    bool inverted;
    string motorType;
    PositioningServoArgs(picojson::value jsonArg) {
        conversionType = jsonArg.get("conversionType").get<string>();
        motorType = jsonArg.get("motorType").get<string>();
        movementType = jsonArg.get("movementType").get<string>()[0];
        servoIndex = jsonArg.get("servoIndex").get<double>();
        inverted = jsonArg.get("inverted").get<bool>();
        defaultAngle = jsonArg.get("defaultAngle").get<double>();
        picojson::value positionList = jsonArg.get("servoPosition");
        parseNumberArrayToFloatVector(positionList, &servoPosition);
    }
};

struct MovementSeriesArgs {
    public:
    string name;
    string type;
    int millisecondDelay;
    vector<vector<float>> series;
    MovementSeriesArgs(picojson::value jsonArg) {
        name = jsonArg.get("name").get<string>();//.to_str();
        type = jsonArg.get("type").get<string>();
        millisecondDelay = jsonArg.get("millisecondDelay").get<double>();
        picojson::value seriesList = jsonArg.get("series");
        parseCoordinateListFromJsonArray(seriesList, &series);
    }
};

struct ExtensionCommandArgs {
    public:
    string name;
    vector<float> coordinate;
    uint64_t postDelay;
    ExtensionCommandArgs(picojson::value jsonArg) {
        name = jsonArg.get("name").get<string>();
        postDelay = jsonArg.get("postDelay").get<double>();
        auto coordList = jsonArg.get("coordinate");
        parseNumberArrayToFloatVector(coordList, &coordinate);
    }
};

struct SeriesCommandArgs {
    public:
    string name;
    string seriesName;
    int iterations;
    SeriesCommandArgs(picojson::value jsonArg) {
        name = jsonArg.get("name").get<string>();
        seriesName = jsonArg.get("seriesName").get<string>();
        iterations = jsonArg.get("iterations").get<double>();
    }
};

struct flatCoordinate {
    public:
    float x;
    float y;
    flatCoordinate(float xCoordinate = 0.f, float yCoordinate = 0.f) {
        x = xCoordinate;
        y = yCoordinate;
    }
};

struct d3Coordinate {
    public:
    float x;
    float y;
    float z;
    d3Coordinate(float xCoordinate, float yCoordinate, float zCoordinate) {
        x = xCoordinate;
        y = yCoordinate;
        z = zCoordinate;
    }
};

struct coordinateChanges {
    public:
    float rotationTotal;
    float rotationChange;
    flatCoordinate newPosition;
    coordinateChanges(float rt, float rc, flatCoordinate* coord) {
        rotationTotal = rt;
        rotationChange = rc;
        newPosition.x = coord->x;
        newPosition.y = coord->y;
    }

};

#include "../main.h"
using namespace std;
using translationFunc = function<void(picojson::value, ExtensionController*)>;

bool pause_updates = false;
bool updates_occurring = false;
string loadedWrite = "";
string lastRead;
bool clear = false;

void setupExtensionTracker(picojson::value json, ExtensionController* controller) {
    ExtensionTrackerArgs argStruct(json);
    for (auto& servoIndex : argStruct.servos) {
        if (controller->motors.size() < servoIndex)
            throw "Positioning Servo is missing from list" + to_string(servoIndex);
    }
    ExtensionTracker zeroIndexTracker(argStruct.name,
        argStruct.defaultCoordinate,
        { controller->motors[argStruct.servos[0]],
         controller->motors[argStruct.servos[1]],
         controller->motors[argStruct.servos[2]] });

    controller->setNewExtensionTracker(zeroIndexTracker);

}
void processNewMotor(picojson::value json, ExtensionController* controller) {
    PositioningServoArgs argStruct(json);

    PositioningServo newMotor(
        argStruct.servoIndex, argStruct.movementType, argStruct.defaultAngle,
        argStruct.servoPosition, argStruct.conversionType,
        argStruct.inverted, argStruct.motorType);

    controller->motors.push_back(newMotor);
}
void newMovementSeries(picojson::value json, ExtensionController* controller) {

    MovementSeriesArgs argStruct(json);

    MovementSeries newSet(argStruct.name, argStruct.type, argStruct.millisecondDelay, argStruct.series);

    int resolutionMultiplication = json.get("resolution").get<double>();

    newSet.increaseResolution(resolutionMultiplication);

    controller->setNewMovementSeriesForExtension(newSet);
}
void processPositionCommand(picojson::value json, ExtensionController* controller) {

    ExtensionCommandArgs argStruct(json);

    extensionCommand command(argStruct.name, argStruct.coordinate, argStruct.postDelay);

    controller->setExtensionToPoint(command);
}
void processExtensionSeriesCall(picojson::value json, ExtensionController* controller) {

    SeriesCommandArgs argStruct(json);

    extensionSeriesCommand seriesCommand(argStruct.name, argStruct.seriesName, argStruct.iterations);

    controller->prepareNextSeries(seriesCommand);
}

void createMotorOutputToRead(picojson::value _parsedCom, ExtensionController* controller) {
    loadedWrite.clear();
    loadedWrite = "{ \"positioningMotors\": [";

    for (int i = 0; i < controller->motors.size(); i++) {
        string jsonOfMotor;
        controller->motors[i].getJsonStringOfClass(&jsonOfMotor);
        loadedWrite.append(jsonOfMotor);
        if (i < controller->motors.size() - 1) {
            loadedWrite.append(",");
        }
    }
    loadedWrite.append("]}");
};

void clearAllItems(picojson::value command, ExtensionController* controller) {
    controller->clearAllItems();
}


map<string, translationFunc> commandMapping{
    {"EXTENSIONTRACKER", setupExtensionTracker},
    {"MOVEMENTSERIES", newMovementSeries},
    {"EXTENSIONSERIESCOMMAND", processExtensionSeriesCall},
    {"POSITIONINGMOTOR", processNewMotor},
    {"EXTENSIONCOMMAND", processPositionCommand},
    {"DELETEALLSTRUCTURES", clearAllItems},
    {"READMOTORS", createMotorOutputToRead}
};

int process_command(string jsonString, int commandLength, ExtensionController* controller) {
    try {
        int passcode = 0;
        replace(jsonString.begin(), jsonString.end(), '\u0002', ' ');
        replace(jsonString.begin(), jsonString.end(), '\u0003', ' ');
        updates_occurring = true;
        lastRead = lastRead + jsonString;
        picojson::value parsedCommand;
        string parsingError = picojson::parse(parsedCommand, jsonString);
        if (!parsingError.empty()) {
            throw parsingError;
        }
        if (parsedCommand.is<picojson::object>()) {
            string command = parsedCommand.get("command").get<string>();
            commandMapping[command](parsedCommand, controller);
        }
        else if (parsedCommand.is<picojson::array>()) {
            picojson::array& listFromReceivedString = parsedCommand.get<picojson::array>();
            for (picojson::array::const_iterator i = listFromReceivedString.begin(); i != listFromReceivedString.end(); ++i) {
                if (i->is<picojson::object>()) {
                    string command = i->get("command").get<string>();
                    commandMapping[command](*i, controller);
                }
            }
        }
        updates_occurring = false;
        return passcode;
    } catch (...) {
        updates_occurring = false;
        return 1;
    }
    updates_occurring = false;
    return 1;
}
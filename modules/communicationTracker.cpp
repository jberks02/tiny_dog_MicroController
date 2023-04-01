#include "../main.h"
using namespace std;
using translationFunc = function<void(picojson::value)>;

bool pause_updates = false;
bool updates_occurring = false;
string loadedWrite;
bool clear = false;
vector<PositioningServo> servos;
vector<MovementSeries> movementSeriesList;
vector<extensionCommand> commands;
vector<ExtensionTrackerArgs> ExtensionTrackerList;
vector<extensionSeriesCommand> seriesCommands;

void setupExtensionTracker(picojson::value json)
{
    ExtensionTrackerArgs argStruct(json);
    for (auto &servoIndex : argStruct.servos)
    {
        if (servos.size() < servoIndex)
            throw "Positioning Servo is missing from list" + to_string(servoIndex);
    }
    ExtensionTrackerList.push_back(argStruct);
}
void processNewMotor(picojson::value json)
{
    PositioningServoArgs argStruct(json);
    PositioningServo newMotor(
        argStruct.servoIndex, argStruct.movementType, argStruct.defaultAngle,
        argStruct.servoPosition, argStruct.conversionType,
        argStruct.inverted, argStruct.motorType);
    servos.push_back(newMotor);
}
void newMovementSeries(picojson::value json)
{

    MovementSeriesArgs argStruct(json);

    MovementSeries newSet(argStruct.name, argStruct.type, argStruct.millisecondDelay, argStruct.series);

    int resolutionMultiplication = json.get("resolution").get<double>();

    newSet.increaseResolution(resolutionMultiplication);

    movementSeriesList.push_back(newSet);
}
void processPositionCommand(picojson::value json)
{

    ExtensionCommandArgs argStruct(json);

    extensionCommand command(argStruct.name, argStruct.coordinate, argStruct.postDelay);

    commands.push_back(command);
}
void processExtensionSeriesCall(picojson::value json)
{

    SeriesCommandArgs argStruct(json);

    extensionSeriesCommand seriesCommand(argStruct.name, argStruct.seriesName, argStruct.iterations);

    seriesCommands.push_back(seriesCommand);
}

void createMotorOutputToRead(picojson::value _parsedCom)
{
    string loadedWrite = "{ \"positioningMotors\": [";

    for (int i = 0; i < servos.size(); i++)
    {
        string jsonOfMotor;
        servos[i].getJsonStringOfClass(&jsonOfMotor);
        loadedWrite.append(jsonOfMotor);
        if (i < servos.size() - 1)
        {
            loadedWrite.append(",");
        }
    }
    loadedWrite.append("]}");
};

void clearAllItems(picojson::value command) {
    servos.clear();
    clear = true;
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

int process_command(string command, int commandLength)
{
    try
    {
        int passcode = 0;

        updates_occurring = true;

        picojson::value parsedCommand;

        string parsingError = picojson::parse(parsedCommand, command);

        if (!parsingError.empty())
            throw parsingError;

        if (!parsedCommand.is<picojson::object>())
            throw "Sent value is not a proper value";

        string command = parsedCommand.get("command").get<string>();

        // if (command == "EXTENSIONTRACKER")
        // {
        //     setupExtensionTracker(parsedCommand);
        // }
        // else if (command == "MOVEMENTSERIES")
        // {
        //     newMovementSeries(parsedCommand);
        // }
        // else if (command == "EXTENSIONSERIESCOMMAND")
        // {
        //     processExtensionSeriesCall(parsedCommand);
        // }
        // else if (command == "POSITIONINGMOTOR")//
        // {
        //     processNewMotor(parsedCommand);
        // }
        // else if (command == "EXTENSIONCOMMAND")
        // {
        //     processPositionCommand(parsedCommand);
        // }
        // else if (command == "READMOTORS")
        // {
        //     createMotorOutputToRead(parsedCommand);
        // }
        // else
        // {
        //     passcode = 1;
        // }

        updates_occurring = false;
        return passcode;
    }
    catch (...)
    {
        return 1;
    }
    return 1;
}
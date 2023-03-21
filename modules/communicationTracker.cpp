#include "../main.h"
using namespace std;

class Communications
{
public:
    bool pause_updates = false;
    bool updates_occurring = false;
    vector<vector<vector<float>>> xyPlaneList;
    vector<vector<vector<float>>> yzPlaneList;
    string loadedWrite;
    vector<float> defaultCoordinate;
    vector<PositioningServo> servos;
    vector<MovementSeries> movementSeriesList;
    vector<extensionCommand> commands;
    vector<ExtensionTrackerArgs> ExtensionTrackerList;
    vector<extensionSeriesCommand> seriesCommands;
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

            if (command == "EXTENSIONTRACKER")
            {
                setupExtensionTracker(parsedCommand);
            }
            else if (command == "MOVEMENTSERIES")
            {
                newMovementSeries(parsedCommand);
            }
            else if (command == "EXTENSIONSERIESCOMMAND")
            {
                processExtensionSeriesCall(parsedCommand);
            }
            else if (command == "POSITIONINGMOTOR")
            {
                processNewMotor(parsedCommand);
            }
            else if (command == "EXTENSIONCOMMAND")
            {
                processPositionCommand(parsedCommand);
            }
            else if (command == "READMOTORS")
            {
                createMotorOutputToRead();
            }
            else
            {
                passcode = 1;
            }

            updates_occurring = false;
            return passcode;
        }
        catch (...)
        {
            return 1;
        }
        return 1;
    }

private:
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
    void createMotorOutputToRead()
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
};

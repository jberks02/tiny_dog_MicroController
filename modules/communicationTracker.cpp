#include "../main.h"
using namespace std;

// const char *json = "{\"a\":1}";
// picojson::value v;
// std::string err;
// const char *json_end = picojson::parse(v, json, json + strlen(json), &err);
// if (!err.empty())
// {
//     std::cerr << err << std::endl;
// }

class Communications
{
public:
    bool pause_updates = false;
    bool updates_occurring = false;
    vector<vector<vector<float>>> xyPlaneList;
    vector<vector<vector<float>>> yzPlaneList;
    string name;
    vector<float> defaultCoordinate;
    vector<PositioningServo> servos;
    vector<MovementSeries> movementSeriesList;
    vector<extensionCommand> commands;
    vector<ExtensionTrackerArgs> ExtensionTrackerList;
    vector <extensionSeriesCommand> seriesCommands;
    int process_command(string command, int commandLength)
    {
        try
        {
            pause_updates = true;
            updates_occurring = true;

            picojson::value parsedCommand;

            string parsingError = picojson::parse(parsedCommand, command);

            if (!parsingError.empty())
                throw parsingError;

            if (!parsedCommand.is<picojson::object>())
                throw "Sent value is not a proper value";

            string command = parsedCommand.get("command").get<string>();

            // picojson::object ObjectToCreate = parsedCommand;

            // to_lower(objectToCreate);

            if (command == "EXTENSIONTRACKER")
            {
                setupExtensionTracker(parsedCommand);
                return 0;
            }
            else if (command == "MOVEMENTSERIES")
            {
                newMovementSeries(parsedCommand);
                return 0;
            }
            else if (command == "EXTENSIONSERIESCOMMAND")
            {
                processExtensionSeriesCall(parsedCommand);
                return 0;
            }
            else if (command == "POSITIONINGMOTOR")
            {
                processNewMotor(parsedCommand);
                return 0;
            }
            else if (command == "EXTENSIONCOMMAND")
            {
                processPositionCommand(parsedCommand);
                return 0;
            }
            
            pause_updates = false;
            updates_occurring = false;
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
    void processExtensionSeriesCall(picojson::value json) {

        SeriesCommandArgs argStruct(json);

        extensionSeriesCommand seriesCommand(argStruct.name, argStruct.seriesName, argStruct.iterations);

        seriesCommands.push_back(seriesCommand);

    } 
};

#include "../main.h"
using namespace std; 

class Communications
{
public:
    bool pause_updates = false;

public:
    bool updates_occurring = false;

public:
    vector<vector<vector<float>>> xyPlaneList;

public:
    vector<vector<vector<float>>> yzPlaneList;

public:
    string name;

public:
    vector<float> defaultCoordinate;

public:
    vector<PositioningServo> servos;

public:
    int process_command(vector<uint8_t> command, int commandLength)
    {
        try
        {

            int lastIndex = 0;

            while (command[0] == '\001')
            {
                shift_left(&command);
            }

            string objectToCreate = extractStringPartial(command, &lastIndex, ' ');

            // to_lower(objectToCreate);

            if (objectToCreate == "EXTENSIONTRACKER")
            {
                // function to process extensionTracker
                return 0;
            }
            else if (objectToCreate == "MOVEMENTSERIES")
            {
                // function to add new series
                return 0;
            }
            else if (objectToCreate == "EXTENSIONSERIESCOMMAND")
            {
                // function to run new series for number of iterations.
                return 0;
            }
            else if (objectToCreate == "POSITIONINGMOTOR")
            {
                processNewMotor(lastIndex, command);
                return 0;
            } 
            else if (objectToCreate == "XYPLANE") {
                processXyPlane(lastIndex, command);
            }
        }
        catch (...)
        {
            return 1;
        }
        return 1;
    }
private:

    // function to create new server
    // args for servo: int servoIndex, char movementType, float defaultAngle,
    // vector<float> servoPosition, string conversionType = "default", bool inverted = false,
    // string motorType = "servo"
    void processXyPlane(lastIndex, command) {

    } 
    void processNewMotor(int lastIndex, vector<uint8_t> set)
    {
        if (set[lastIndex] == ' ')
            lastIndex++;
        string servoIndex = extractStringPartial(set, &lastIndex, (char)' ');
        string moveType = extractStringPartial(set, &lastIndex, ' ');
        string defaultAngle = extractStringPartial(set, &lastIndex, ' ');
        if(set[lastIndex] != '{') throw set;//bad input
        lastIndex++;
        vector<string> servoPosition = {extractStringPartial(set, &lastIndex, ','), extractStringPartial(set, &lastIndex, ','), extractStringPartial(set, &lastIndex, '}')};
        string conversionType = extractStringPartial(set, &lastIndex, ' ');
        string inverted = extractStringPartial(set, &lastIndex, ' ');
        string motorType = extractStringPartial(set, &lastIndex, ' ');
        servos.push_back(
            PositioningServo(atoi(servoIndex.c_str()), moveType[0], stof(defaultAngle.c_str()),
                             {stof(servoPosition[0].c_str()), stof(servoPosition[1].c_str()), stof(servoPosition[2].c_str())},
                             conversionType, atoi(inverted.c_str()), motorType));
    }
    string extractStringPartial(vector<uint8_t> set, int *lastIndex, char breakAt)
    {
        string segment;
        for (int i = *lastIndex; i < set.size(); i++)
        {
            if (set[i] == breakAt)
                break;
            segment.push_back(set[i]);
            lastIndex++;
        }
        lastIndex++;
        return segment;
    }
    // float extractNumberInCommand(vector<uint8_t> set, int *lastIndex, char breakAt) {
    //     string segment;
    //     for(int i = *lastIndex; i < set.size(); i++) {
    //         if(set[i] == breakAt) break;
    //         segment.push_back(set[i]);
    //         lastIndex++;
    //     }
    //     lastIndex++;
    //     return stof(segment.c_str());
    // }
    //  void processExtensionTracker(int lastIndex, uint8_t *set, int setLength)
    //  {
    //      //getName of extension
    //      string extensionName;
    //      addToStringTillCommaBreak(&extensionName, set, &lastIndex);
    //      name = extensionName;
    //      lastIndex++;
    //      //get planes of extension
    //      string protoXyPlane;
    //      int forwardBraces;
    //      int backBraces;
    //      int l = lastIndex;
    //      for(int charIndex = l; charIndex < setLength; charIndex++) {
    //          if(set[charIndex] == ',') break;
    //          if(set[charIndex] == '{') forwardBraces++;
    //          if(set[charIndex] == '}') backBraces++;
    //          protoXyPlane.push_back(set[charIndex]);
    //          lastIndex = charIndex;
    //      }
    //      lastIndex++;
    //      if(forwardBraces != backBraces) return;//alert user that data is missing
    //      vector<vector<float>> xy;
    //  }
    int set_command_index(int *lastIndex, uint8_t *set, int setLength, int startIndex)
    {

        string indexString = "";

        for (int i = startIndex; i < setLength; i++)
        {
            *lastIndex = i;
            if (set[i] == ' ' && indexString == "")
                continue;
            if (set[i] == ' ' && indexString != "")
                break;
            indexString = indexString + (char)set[i];
        }

        return atoi(indexString.c_str());
    }
    void shift_left(vector<uint8_t> *list)
    {
        list->erase(list->begin());
    }
};

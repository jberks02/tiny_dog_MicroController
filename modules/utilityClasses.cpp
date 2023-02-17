#include "../main.h"
using namespace std;

class MovementSeries
{
public:
    string name;
    string type;
    int millisecondDelay;
    vector<vector<float>> series;
    vector<vector<float>> angleSeries;
    MovementSeries(string name, string type, int millisecondDelay, vector<vector<float>> series) {
        this->name = name;
        this->type = type;
        this->millisecondDelay = millisecondDelay;
        this->series.swap(series);
    };
    void increaseResolution(int resolutionMultiplication) {
        for (int m = 0; m < resolutionMultiplication; m++) {
            for (int i = 0; i < series.size(); i++)
            {
                int nextIndex = i + 1;
                if (i == series.size() - 1) nextIndex = 0;
                vector<float> newPoint;
                calculateHalfWayDigit(series[i], series[nextIndex], &newPoint);
                if (i != series.size() - 1) series.insert(series.begin() + nextIndex, newPoint);
                else series.push_back(newPoint);
                i++;
            }
        }
    };
private:
    void calculateHalfWayDigit(vector<float> curPoint, vector<float> nextPoint, vector<float> *newPoint) {
        vector<float> tempNewPoint;
        float x = (curPoint[0] + nextPoint[0]) == 0 ? 0 : (curPoint[0] + nextPoint[0])/2.f;
        float y = (curPoint[1] + nextPoint[1]) == 0 ? 0 : (curPoint[1] + nextPoint[1])/2.f;
        float z = (curPoint[2] + nextPoint[2]) == 0 ? 0 : (curPoint[2] + nextPoint[2])/2.f;
        tempNewPoint = {x,y,z};
        newPoint->swap(tempNewPoint);
    }
};

class ExtensionServo {
public: 
    int servoIndex;
    char rotationDirection;
    float defaultAngle;
    vector<int> servoPosition;
    ExtensionServo(int servoIndex, char rotationDirection, float defaultAngle, vector<int> servoPosition) {
        this->servoIndex = servoIndex;
        this->rotationDirection = rotationDirection;
        this->defaultAngle = defaultAngle;
        this->servoPosition.swap(servoPosition);
    }
};

// Example of implimentation of Movement Series class that can be created.
class MovementSeriesExample
{
public:
    string name = "Forward";     // descriptive name to describe what this series will do.
    string type = "Progressive"; // a simple description some movements will be progressive, some will be regressive, some may be just to lean or animate
    int millisecondDelay = 200;  // this will be the delay between each coordinate progression
    vector<vector<float>> series = {
        {40, 0, 20},
        {35, 0, -20},
        {30, 0, 0},
        {25, 0, 20},
        {20, 0, -20},
        {15, 0, 0},
        {10, 0, 20},
        {5, 0, -20},
        {0, 0, 0},
        {-5, 0, 20},
        {-10, 0, 0},
        {-15, 0, 0},
        {-20, 0, 0},
        {-25, 0, 0},
        {-30, 0, 0},
        {-35, 0, 0},
        {-40, 0, 0},
    };
    vector<vector<float>> angleSeries; // to be filled out as coordinate point calculations are ran
};

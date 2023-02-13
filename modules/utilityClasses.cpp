#include "../main.h"
using namespace std;

class MovementSeries
{
public:
    string name;
    string type;
    int millisecondDelay;
    vector<vector<int>> series;
    vector<vector<int>> anglesSeries;
};

// Example of implimentation of Movement Series class that can be created.
class MovementSeriesExample
{
public:
    string name = "Forward";     // descriptive name to describe what this series will do.
    string type = "Progressive"; // a simple description some movements will be progressive, some will be regressive, some may be just to lean or animate
    int millisecondDelay = 200;  // this will be the delay between each coordinate progression
    vector<vector<int>> series = {
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
    vector<vector<int>> anglesSeries; // to be filled out as coordinate point calculations are ran
};

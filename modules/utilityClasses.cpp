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
    int millisecondDelay = 200;   // this will be the delay between each coordinate progression
    vector<vector<int>> series = {
        // {0, 40, 0},
        // {70, 45, 0},
        // {65, 40, 0},
        // {60, 35, 0},
        // {55, 0, 0},
        // {50, 0, 0},
        // {45, 0, 0},
        // {40, 0, 0},
        // {35, 0, 0},
        // {30, 0, 0},
        {80, 0, 0},
        {20, 0, 0},
        {15, 0, 0},
        {10, 0, 0},
        {5, 0, 0},
        {0, 0, 0},
        {-5, 0, 0},
        {-10, 0, 0},
        {-15, 0, 0},
        {-20, 0, 0},
        {-25, 0, 0},
        {-20, 0, 0},
        {-15, 0, 0},
        {-10, 0, 0}, 
        {-5, 0, 0},
        {0, 0, 0},
        {5, 0, 0}, 
        {10, 0, 0},
        {15, 0, 0},
        {20, 0, 0}
        // {-30, 0, 0},
        // {-35, 0, 0},
        // {-40, 0, 0},
        // {-45, 0, 0},
        // {-50, 0, 0},
        // {-50, 0, 0},
        // {-50, 0, 0},
        // {-50, 10, 0},
        // {-50, 20, 0},
        // {-50, 20, 0},
        // {-40, 25, 0},
        // {-30, 30, 0},
        // {-20, 35, 0},
        // {-10, 40, 0},
    };
    vector<vector<int>> anglesSeries; // to be filled out as coordinate point calculations are ran
};

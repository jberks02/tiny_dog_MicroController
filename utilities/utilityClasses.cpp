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
    MovementSeries(string name, string type, int millisecondDelay, vector<vector<float>> series)
    {
        this->name = name;
        this->type = type;
        this->millisecondDelay = millisecondDelay;
        this->series.swap(series);
    };
    void increaseResolution(int resolutionMultiplication)
    {
        for (int m = 0; m < resolutionMultiplication; m++)
        {
            for (int i = 0; i < series.size(); i++)
            {
                int nextIndex = i + 1;
                if (i == series.size() - 1)
                    nextIndex = 0;
                vector<float> newPoint;
                calculateHalfWayDigit(series[i], series[nextIndex], &newPoint);
                if (i != series.size() - 1)
                    series.insert(series.begin() + nextIndex, newPoint);
                else
                    series.push_back(newPoint);
                i++;
            }
        }
    };

private:
    void calculateHalfWayDigit(vector<float> curPoint, vector<float> nextPoint, vector<float> *newPoint)
    {
        vector<float> tempNewPoint;
        float x = (curPoint[0] + nextPoint[0]) == 0 ? 0 : (curPoint[0] + nextPoint[0]) / 2.f;
        float y = (curPoint[1] + nextPoint[1]) == 0 ? 0 : (curPoint[1] + nextPoint[1]) / 2.f;
        float z = (curPoint[2] + nextPoint[2]) == 0 ? 0 : (curPoint[2] + nextPoint[2]) / 2.f;
        tempNewPoint = {x, y, z};
        newPoint->swap(tempNewPoint);
    }
};

class PositioningServo
{
public:
    int servoIndex;
    char movementType;
    string motorType;
    float defaultAngle;
    float currentAngle;
    bool inverted;
    string conversionType;
    vector<float> servoPosition;
    map<char, float> quadraticEquationFactors{{'a', -0.02036}, {'b', 6.7478}, {'c', -214.8957}};
    map<char, float> linearFactors{{'m', 4.f}, {'b', 0}};
    map<char, float> exponentialFactors{{'a', 1.1}, {'b', -1}, {'c', -42}};
    PositioningServo(int servoIndex, char movementType, float defaultAngle, vector<float> servoPosition, string conversionType = "default", bool inverted = false, string motorType = "servo")
    {
        this->servoIndex = servoIndex;
        this->movementType = movementType;
        this->defaultAngle = defaultAngle;
        this->currentAngle = defaultAngle;
        this->conversionType = conversionType;
        this->servoPosition.swap(servoPosition);
        this->inverted = inverted;
        this->motorType = motorType;
    }

private:
    float
    defaultConversion(float defaultRelevantNumber, float newRelNumber, float newAngle)
    {
        if (defaultRelevantNumber < newRelNumber)
        {
            return defaultAngle + newAngle;
        }
        else
        {
            return defaultAngle - newAngle;
        }
    }
    float convertOnQuadraticScale(float angle)
    {
        float squaredArmAngle = pow(angle, 2.0);
        float a = squaredArmAngle * (quadraticEquationFactors['a']);
        float b = angle * quadraticEquationFactors['b'];
        float c = quadraticEquationFactors['c'];
        float product = a + b + c;
        return product;
    };
    float convertLinear(float Angle)
    {
        float product = Angle * linearFactors['m'];
        return product + linearFactors['b'];
    }
    float convertExponential(float Angle)
    {
        float exponent = Angle + exponentialFactors['c'];
        float powered = pow(exponentialFactors['a'], exponent);
        return powered + exponentialFactors['b'];
    }

public:
    float convert(float defaultRelevantNumber, float newRelNumber, float newAngle)
    {
        float newAngleValue;
        if (conversionType == "default")
        {
            newAngleValue = defaultConversion(defaultRelevantNumber, newRelNumber, newAngle);
        }
        else if (conversionType == "quadratic")
        {
            newAngleValue = convertOnQuadraticScale(newAngle);
        }
        else if (conversionType == "linear")
        {
            newAngleValue = convertLinear(newAngle);
        }
        else if (conversionType == "exponential")
        {
            newAngleValue = convertExponential(newAngle);
        }
        else
        {
            newAngleValue = newAngle;
        }
        // if(inverted == true & newAngleValue > 90.f) {
        //     return 180 - newAngleValue;
        // } else if (inverted == true & newAngleValue < 90.f) {
        //     return newAngleValue - 180;
        // } else {
        //     return newAngleValue;
        // }
        return inverted == true ? 180 - newAngleValue : newAngleValue;
    }
    void setQuadraticFactors(map<char, float> factors)
    {
        quadraticEquationFactors['a'] = factors['a'];
        quadraticEquationFactors['b'] = factors['b'];
        quadraticEquationFactors['c'] = factors['c'];
    }
    void setLinearFactors(map<char, float> factors)
    {
        linearFactors['m'] = factors['m'];
        linearFactors['b'] = factors['b'];
    }
    void setExponentialFactors(map<char, float> factors)
    {
        exponentialFactors['a'] = factors['a'];
        exponentialFactors['b'] = factors['b'];
        exponentialFactors['c'] = factors['c'];
    }
};

struct extensionCommand
{
    string name;
    vector<float> coordinate;
    uint64_t postDelay;
    uint64_t executionMoment;
    extensionCommand(string n, vector<float> c, uint64_t pd)
    {
        name = n;
        coordinate = c;
        postDelay = pd;
    }
};

struct servoCommand
{
    int servo;
    float angle;
    uint64_t postDelay;
};

struct extensionSeriesCommand
{
    string extensionName;
    string seriesName;
    int iterations;
    extensionSeriesCommand(string name, string sname, int i)
    {
        extensionName = name;
        seriesName = sname;
        iterations = i;
    }
};

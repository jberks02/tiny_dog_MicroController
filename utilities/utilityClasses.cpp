#include "../main.h"
using namespace std;

class MovementSeries {
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
            for (int i = 0; i < series.size(); i++) {
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
    void calculateHalfWayDigit(vector<float> curPoint, vector<float> nextPoint, vector<float>* newPoint) {
        vector<float> tempNewPoint;
        float x = (curPoint[0] + nextPoint[0]) == 0 ? 0 : (curPoint[0] + nextPoint[0]) / 2.f;
        float y = (curPoint[1] + nextPoint[1]) == 0 ? 0 : (curPoint[1] + nextPoint[1]) / 2.f;
        float z = (curPoint[2] + nextPoint[2]) == 0 ? 0 : (curPoint[2] + nextPoint[2]) / 2.f;
        tempNewPoint = { x, y, z };
        newPoint->swap(tempNewPoint);
    }
};

class PositioningServo {
    public:
    int servoIndex;
    char movementType;
    string motorType;
    float defaultAngle;
    float currentAngle;
    bool inverted;
    string conversionType;
    vector<float> servoPosition;
    map<char, float> quadraticEquationFactors{ {'a', -0.02036}, {'b', 6.7478}, {'c', -214.8957} };
    map<char, float> linearFactors{ {'m', 1.f}, {'b', 0.f} };
    map<char, float> exponentialFactors{ {'a', 1.1}, {'b', -1}, {'c', -42} };
    PositioningServo(int servoIndex, char movementType, float defaultAngle, vector<float> servoPosition, string conversionType = "default", bool inverted = false, string motorType = "servo") {
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
    void defaultConversion(float newAngle) {
        if (inverted == true) {
            currentAngle = 180 - newAngle;
        }
        else {
            currentAngle = newAngle;
        }
    }
    public:
    void setServoAngle(float newAngle) {
        float newAngleValue;
        if (conversionType == "default") {
            defaultConversion(newAngle);
        }
        else {
            currentAngle = newAngle;
        }
    }
    void setQuadraticFactors(map<char, float> factors) {
        quadraticEquationFactors['a'] = factors['a'];
        quadraticEquationFactors['b'] = factors['b'];
        quadraticEquationFactors['c'] = factors['c'];
    }
    void setLinearFactors(map<char, float> factors) {
        linearFactors['m'] = factors['m'];
        linearFactors['b'] = factors['b'];
    }
    void setExponentialFactors(map<char, float> factors) {
        exponentialFactors['a'] = factors['a'];
        exponentialFactors['b'] = factors['b'];
        exponentialFactors['c'] = factors['c'];
    }
    void getJsonStringOfClass(string* jsonString) {
        string moveT(1, movementType);
        string inv = inverted == true ? "true" : "false";
        jsonString->append("{");
        jsonString->append("\"servoIndex\":" + to_string(servoIndex) + ",");
        jsonString->append("\"movementType\":\"" + moveT + "\",");
        jsonString->append("\"motorType\":\"" + motorType + "\",");
        jsonString->append("\"defaultAngle\":" + to_string(defaultAngle) + ",");
        jsonString->append("\"currentAngle\":" + to_string(currentAngle) + ',');
        jsonString->append("\"inverted\":" + inv + ',');
        jsonString->append("\"conversionType\":" + conversionType + ",");
        jsonString->append("\"servoPosition\":[");
        for (int i = 0; i < 3; i++) {
            jsonString->append(to_string(servoPosition[i]));
            if (i < 2)
                jsonString->append(",");
        }
        jsonString->append("]");
        jsonString->append("}");
    }
};

struct extensionCommand {
    string name;
    vector<float> coordinate;
    uint64_t postDelay;
    uint64_t executionMoment;
    extensionCommand(string n, vector<float> c, uint64_t pd) {
        name = n;
        coordinate = c;
        postDelay = pd;
    }
};

struct servoCommand {
    int servo;
    float angle;
    uint64_t postDelay;
};

struct extensionSeriesCommand {
    string extensionName;
    string seriesName;
    int iterations;
    extensionSeriesCommand(string name, string sname, int i) {
        extensionName = name;
        seriesName = sname;
        iterations = i;
    }
};

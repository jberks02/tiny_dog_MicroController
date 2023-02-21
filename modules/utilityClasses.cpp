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
    float defaultAngle;
    bool inverted;
    string conversionType;
    vector<float> servoPosition;
    map<char, float> quadraticEquationFactors{{'a', -0.02036}, {'b', 6.7478}, {'c', -214.8957}};
    map<char, float> linearFactors{{'m', 4.f}, {'b', 0}};
    map<char, float> exponentialFactors{{'a', 1.1}, {'b', -1}, {'c', -42}};
    PositioningServo(int servoIndex, char movementType, float defaultAngle, vector<float> servoPosition, string conversionType = "default", bool inverted = false)
    {
        this->servoIndex = servoIndex;
        this->movementType = movementType;
        this->defaultAngle = defaultAngle;
        this->conversionType = conversionType;
        this->servoPosition.swap(servoPosition);
        this->inverted = inverted;
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
        if (conversionType == "default")
        {
            return defaultConversion(defaultRelevantNumber, newRelNumber, newAngle);
        }
        else if (conversionType == "quadratic")
        {
            return convertOnQuadraticScale(newAngle);
        }
        else if (conversionType == "linear")
        {
            return convertLinear(newAngle);
        }
        else if (conversionType == "exponential")
        {
            return convertExponential(newAngle);
        }
        else
        {
            return newAngle;
        }
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
    void setExponentialFactors(map<char, float> factors) {
        exponentialFactors['a'] = factors['a'];
        exponentialFactors['b'] = factors['b'];
        exponentialFactors['c'] = factors['c'];
    }
};

struct Plane
{
    vector<float[3]> plane;
    vector<Plane> connections;
};
//Define a static 3d object through sending vectors of face coordinates
//it is assumed that points provided for planes will be connected points one after the other.
class Body {
public:
    vector<vector<float[3]>> faces;
    vector<PositioningServo> *connectedMotors;
    Plane *root = NULL;
    Body(vector<vector<float[3]>> faces) {
        this->faces = faces;
        verifyFacesConnect();
    }
    void createPlaneLinkedList(vector<float[3]> initialPlane) {
        root = new Plane;
        root->plane = initialPlane;
    }
    void traverseAndAssign(vector<float[3]> newPlane) {
        
    }
    int verifyFacesConnect() {
        vector<int> indexesWhereNoConnectWasFound;
        for (auto &face : faces)
        {
            if (root == NULL) {
                createPlaneLinkedList(face);
                continue;
            };
            traverseAndAssign(face);
        }
    }
};

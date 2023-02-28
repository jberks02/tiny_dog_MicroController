#include "../main.h"
using namespace std;

class ExtensionController
{
private:
    vector<ExtensionTracker> endEffectors;
    vector<extensionSeriesCommand> extensionSeriesCall;
    PCA9685 *servoController = NULL;
public:
    void setServoController(PCA9685 *servoController) {
        this->servoController = servoController;
    }
    int setNewExtensionTracker(ExtensionTracker extt)
    {
        for (auto &effector : endEffectors)
        {
            if (effector.name == extt.name)
                return 2; // item name already exists
        }
        endEffectors.push_back(extt);
        return 0; // good return code
    }
    int setNewMovementSeriesForExtension(string extension, MovementSeries newSeries)
    {
        for (auto &effector : endEffectors)
        {
            if (effector.name == extension)
            {
                effector.MovementSets.push_back(newSeries);
                return 0;
            }
        }
        return 4; // extension not found
    }
    void prepareNextSeries(extensionSeriesCommand com) {
        for (auto &effector : endEffectors) {
            if(effector.name == com.extensionName) {
                effector.prepareCommands(com.seriesName, com.iterations);
                return;
            };
        }
    }
    int runExtensions() {
        try {
            while (true)
            {
                for(auto extension : endEffectors) {
                    extension.checkAndSetNewCoordinate();
                    for(int i = 0; i < 3; i++) {
                        servoController->servoSetAngle(extension.mServos[i].currentAngle, extension.mServos[i].servoIndex);
                    }
                }
            }
        } catch (...) {
            return 1;
        }
        return 0;
    }
};

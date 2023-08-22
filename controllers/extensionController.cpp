#include "../main.h"
using namespace std;

class ExtensionController {
    public:
    vector<ExtensionTracker> endEffectors;
    vector<extensionSeriesCommand> extensionSeriesCall;
    // PCA9685 *servoController = NULL;
    PCA9685* servoController = NULL;
    bool clearing = false;
    bool inUse = false;

    public:
    void clearAllItems() {
        clearing = true;
        while (inUse == true) {
            tight_loop_contents();
        }
        endEffectors.clear();
        extensionSeriesCall.clear();
        clearing = false;
    }
    void setServoController(PCA9685* servoController) {
        this->servoController = servoController;
    }
    int setNewExtensionTracker(ExtensionTracker extt) {
        for (auto& effector : endEffectors) {
            if (effector.name == extt.name)
                return 2; // item name already exists
        }
        endEffectors.push_back(extt);
        return 0; // good return code
    }
    int setNewMovementSeriesForExtension(string extension, MovementSeries newSeries) {
        for (auto& effector : endEffectors) {
            if (effector.name == extension) {
                effector.MovementSets.push_back(newSeries);
                return 0;
            }
        }
        return 4; // extension not found
    }
    void prepareNextSeries(extensionSeriesCommand com) {
        for (auto& effector : endEffectors) {
            if (effector.name == com.extensionName) {
                effector.prepareCommands(com.seriesName, com.iterations);
                return;
            };
        }
    }
    void setExtensionToPoint(extensionCommand command) {
        for (auto& effector : endEffectors) {
            if (effector.name == command.name) {
                effector.setMotorAnglesForNewPoint(&command.coordinate);
            }
        }
    }
    int runExtensions() {
        try {
            while (true) {
                while (clearing == true) {
                    tight_loop_contents();
                }
                inUse = true;
                for (auto& extension : endEffectors) {
                    extension.checkAndSetNewCoordinate();
                    for (auto& servo : extension.mServos) {
                        servoController->servoSetAngle(servo.currentAngle, servo.servoIndex);
                    }
                }
                inUse = false;
            }
        } catch (...) {
            return 1;
        }
        return 0;
    }
};

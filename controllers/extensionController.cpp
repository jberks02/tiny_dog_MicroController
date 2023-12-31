#include "../main.h"
using namespace std;

class ExtensionController {
    public:
    vector<ExtensionTracker> endEffectors;
    vector<extensionSeriesCommand> extensionSeriesCall;
    vector<PositioningServo> motors;
    PCA9685* servoController = NULL;
    bool clearing = false;
    bool inUse = false;

    public:
    string constructJsonOfBody() {
        stringstream controller;
        controller << "{ \"Extensions\":";
        controller << to_string(endEffectors.size()) << ",";
        controller << "\"seriesCalls\":" << to_string(extensionSeriesCall.size());
        controller << ",\"servoAngles\":[";
        for (int i = 0; i < 16; i++) {
            controller << to_string(servoController->currentAngle[i]);
            if (i + 1 < 16) controller << ",";
        }
        controller << "]}";
        return controller.str();
    }
    void clearAllItems() {
        clearing = true;
        while (inUse == true) {
            tight_loop_contents();
        }
        endEffectors.clear();
        extensionSeriesCall.clear();
        motors.clear();
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
    int setNewMovementSeriesForExtension(MovementSeries newSeries) {
        for (auto& effector : endEffectors) {
            effector.MovementSets.push_back(newSeries);
        }
        return 0;
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
                effector.threeDOFInverseKinematics(&command.coordinate);
            }
        }
    }
    void runExtensions() {
        try {
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
        } catch (...) {
            printf("Extension Run Failed");
        }
    }
};

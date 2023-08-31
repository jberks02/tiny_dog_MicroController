/*
 * Tiny Dog MicroController
 *
 * @version     1.0.0
 * @author     Justin Berkshire
 * @copyright   2022
 * @licence     MIT
 *
 */
#include "main.h"
using namespace std;
ExtensionController* controller = NULL;
SpiInterface* communication = NULL;

const uint LED = PICO_DEFAULT_LED_PIN;
bool updateComing = false;

void splitToSecondCore() {
    try {
        while (true) {
            controller->runExtensions();
        }
    } catch (...) {
        printf("Failure in second core.");
    }
}

int main() {
    stdio_init_all();
    printf("StartUpInProgress");
    controller = new ExtensionController();
    communication = new SpiInterface();
    // ExtensionController controller;
    // SpiInterface communication;

    PCA9685 ServoController(0.f, 181.f, 64, 4, 5);

    controller->setServoController(&ServoController);

    gpio_init(LED);

    gpio_set_dir(LED, GPIO_OUT);

    gpio_put(LED, 1);

    AdcController variableInputs;

    communication->setToSlave();

    multicore_reset_core1();
    multicore_launch_core1(splitToSecondCore);

    while (true) {
        try {
            gpio_put(LED, 1);
            sleep_ms(500);
            vector<string> applicationBodies = { variableInputs.constructJsonOfOutput(), controller->constructJsonOfBody() };
            string write = returnArrayOfJsonsFromList(&applicationBodies);
            // multicore_launch_core1(splitToSecondCore);
            // multicore_fifo_pop_blocking();
            communication->transferBytes(&write, &updateComing);
            // multicore_reset_core1();
            applicationBodies.clear();
            write.clear();
            gpio_put(LED, 0);
            if (ExtensionTrackerList.size() > 0) {
                printf("Writing Extensions");
                for (auto& zeroArg : ExtensionTrackerList) {
                    ExtensionTracker zeroIndexTracker(zeroArg.name,
                        zeroArg.yzPlane,
                        zeroArg.xyPlane,
                        zeroArg.defaultCoordinate,
                        { servos[zeroArg.servos[0]],
                         servos[zeroArg.servos[1]],
                         servos[zeroArg.servos[2]] });
                    // ExtensionTrackerList.erase(ExtensionTrackerList.begin());
                    controller->setNewExtensionTracker(zeroIndexTracker);
                }
                ExtensionTrackerList.clear();
            }
            if (seriesCommands.size() > 0) {
                for (auto& com : seriesCommands) {
                    controller->prepareNextSeries(com);
                }
                seriesCommands.clear();
            }
            if (movementSeriesList.size() > 0) {
                for (auto& ms : movementSeriesList) {
                    for (auto& ext : controller->endEffectors) {
                        controller->setNewMovementSeriesForExtension(ext.name, ms);
                    }
                }
                movementSeriesList.clear();
            }
            if (commands.size() > 0) {
                printf("writingCommand");
                for (auto& com : commands) {
                    controller->setExtensionToPoint(com);
                }
                commands.clear();
            }
            if (clear == true) {
                multicore_reset_core1();
                controller->inUse = false;
                controller->clearAllItems();
                clear = false;
                multicore_launch_core1(splitToSecondCore);
            }
        } catch (...) {
            printf("Primary Loop Failure");
        }
    }
    return 0;
};

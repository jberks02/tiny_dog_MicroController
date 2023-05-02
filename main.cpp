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

ExtensionController controller;
const uint LED = PICO_DEFAULT_LED_PIN;

// void splitToSecondCore() {
//     controller.runExtensions();
// }

int main() {
    stdio_init_all();

    PCA9685 ServoController(0.f, 181.f, 64, 0, 1);

    controller.setServoController(&ServoController);

    // vector<vector<float>> yzPlane = {{86.61356, -33.f}, {86.61356, 0.f}, {0.f, 0.f}};
    // vector<vector<float>> xyPlane = {{0.f, 86.61356}, {-80.f, 86.61356}, {0, 0.f}};
    // vector<float> endaffectorCoordinate = {0.f, 0.f, 0.f};
    // vector<PositioningServo> servos = {PositioningServo(0, 'r', 90.f, {0.f, 86.61356, 0.f}), PositioningServo(1, 'l', 32.f, {-80.f, 86.61356, 0.f}, "quadratic"), PositioningServo(2, 'r', 90.f, {0.f, 86.61356, -33.f}, "default", true)}; // {0.f, 110.f, 0.f}, {0.f, 110.f, -33.f}
    // ExtensionTracker rfLeg("rightFront", yzPlane, xyPlane, endaffectorCoordinate, servos);
    // controller.setNewExtensionTracker(rfLeg);
    // MovementSeries test("test", "transition", 20, {{0.f, 30.f, 0.f}, {90.f, -20.f, -60.f}, {-60.f, -20.f, 60.f}}); //{0.f,20.f,0.f},
    // MovementSeries test2("test2", "transition", 20, {{0.f, 30.f, 0.f}, {90.f, -20.f, 0.f}, {-60.f, -20.f, 0.f}});  //{0.f,20.f,0.f},
    // test.increaseResolution(5);
    // test2.increaseResolution(5);
    // controller.setNewMovementSeriesForExtension("rightFront", test);
    // controller.setNewMovementSeriesForExtension("rightFront", test2);
    // extensionSeriesCommand series("rightFront", "test2", 3);
    // extensionSeriesCommand series2("rightFront", "test", 4);
    // controller.prepareNextSeries(series);
    // controller.prepareNextSeries(series2);
    // multicore_reset_core1();
    // multicore_launch_core1(splitToSecondCore);

    // controller.runExtensions();

    gpio_init(LED);

    gpio_set_dir(LED, GPIO_OUT);

    gpio_put(LED, 1);

    // Communications translationLayer;

    SpiInterface communication;

    communication.setToSlave();

    // piI2cCommumnication piComs;

    while (true)
    {
        gpio_put(LED, 0);
        communication.transfer32Bytes();
        gpio_put(LED, 1);
        if (ExtensionTrackerList.size() > 0)
        {
            for (auto &zeroArg : ExtensionTrackerList)
            {
                ExtensionTracker zeroIndexTracker(zeroArg.name,
                                                  zeroArg.yzPlane,
                                                  zeroArg.xyPlane,
                                                  zeroArg.defaultCoordinate,
                                                  {servos[zeroArg.servos[0]],
                                                   servos[zeroArg.servos[1]],
                                                   servos[zeroArg.servos[2]]});
                ExtensionTrackerList.erase(ExtensionTrackerList.begin());
                controller.setNewExtensionTracker(zeroIndexTracker);
            }
            ExtensionTrackerList.clear();
        }
        if (seriesCommands.size() > 0)
        {
            for (auto &com : seriesCommands)
            {
                controller.prepareNextSeries(com);
            }
            seriesCommands.clear();
        }
        if (movementSeriesList.size() > 0)
        {
            for (auto &ms : movementSeriesList)
            {
                for (auto &ext : controller.endEffectors)
                {
                    controller.setNewMovementSeriesForExtension(ext.name, ms);
                }
            }
            movementSeriesList.clear();
        }
        if (commands.size() > 0)
        {
            for (auto &com : commands)
            {
                controller.setExtensionToPoint(com);
            }
            commands.clear();
        }
        if (clear == true)
        {
            controller.clearAllItems();
            clear = false;
        }
        controller.runExtensions();
    }

    return 0;
};

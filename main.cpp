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

void secondaryLoop()
{
    controller.runExtensions();
}

int main()
{
    stdio_init_all();

    const uint LED = PICO_DEFAULT_LED_PIN;

    gpio_init(LED);

    gpio_set_dir(LED, GPIO_OUT);

    gpio_put(LED, 1);

    PCA9685 ServoController(0.f, 181.f, 64, 0, 1);

    controller.setServoController(&ServoController);

    vector<vector<float>> yzPlane = {{86.61356, -33.f}, {86.61356, 0.f}, {0.f, 0.f}};
    vector<vector<float>> xyPlane = {{0.f, 86.61356}, {-80.f, 86.61356}, {0, 0.f}};
    vector<float> endaffectorCoordinate = {0.f, 0.f, 0.f};
    vector<PositioningServo> servos = {PositioningServo(0, 'r', 90.f, {0.f, 86.61356, 0.f}), PositioningServo(1, 'l', 32.f, {-80.f, 86.61356, 0.f}, "quadratic"), PositioningServo(2, 'r', 90.f, {0.f, 86.61356, -33.f}, "default", true)}; // {0.f, 110.f, 0.f}, {0.f, 110.f, -33.f}

    ExtensionTracker rfLeg("rightFront", yzPlane, xyPlane, endaffectorCoordinate, servos);

    controller.setNewExtensionTracker(rfLeg);

    MovementSeries test("test", "transition", 20, {{0.f, 30.f, 0.f}, {90.f, -20.f, -60.f}, {-60.f, -20.f, 60.f}}); //{0.f,20.f,0.f},
    MovementSeries test2("test2", "transition", 20, {{0.f, 30.f, 0.f}, {90.f, -20.f, 0.f}, {-60.f, -20.f, 0.f}});  //{0.f,20.f,0.f},

    test.increaseResolution(5);
    test2.increaseResolution(5);

    controller.setNewMovementSeriesForExtension("rightFront", test);
    controller.setNewMovementSeriesForExtension("rightFront", test2);

    multicore_reset_core1();
    multicore_launch_core1(secondaryLoop);

    Communications translationLayer;

    SpiInterface communication(&translationLayer);

    while (true)
    {
        gpio_put(LED, 1);
        communication.setToSlave();
        communication.exchangeByteMessage();
        gpio_put(LED, 0);
        if (translationLayer.ExtensionTrackerList.size() > 0)
        {
            ExtensionTrackerArgs zeroArg = translationLayer.ExtensionTrackerList[0];
            ExtensionTracker zeroIndexTracker(zeroArg.name,
                             zeroArg.yzPlane,
                             zeroArg.xyPlane,
                             zeroArg.defaultCoordinate,
                             {translationLayer.servos[zeroArg.servos[0]],
                              translationLayer.servos[zeroArg.servos[1]],
                              translationLayer.servos[zeroArg.servos[2]]});
            translationLayer.ExtensionTrackerList.erase(translationLayer.ExtensionTrackerList.begin());
            controller.setNewExtensionTracker(zeroIndexTracker);
        }
        gpio_put(LED, 1);
        if(translationLayer.seriesCommands.size() > 0) {
            for(auto &com : translationLayer.seriesCommands) {
                controller.prepareNextSeries(com);
            }
            translationLayer.seriesCommands.clear();
        }
        if(translationLayer.movementSeriesList.size() > 0) {
            for (auto &ms : translationLayer.movementSeriesList) {
                for(auto &ext : controller.endEffectors) {
                    controller.setNewMovementSeriesForExtension(ext.name, ms);
                }
            }
        }
        gpio_put(LED, 0);
        // if()
        // extensionSeriesCommand newCommand("rightFront", "test", 2);
        // extensionSeriesCommand dupCom("rightFront", "test2", 2);
        // controller.prepareNextSeries(newCommand);
        // controller.prepareNextSeries(dupCom);
        // controller.prepareNextSeries(newCommand);
        // sleep_ms(1000 * 10);
        // extensionSeriesCommand nextCommand("rightFront", "test", 3);
        // controller.prepareNextSeries(nextCommand);
        // sleep_ms(1000 * 10);
    }
};


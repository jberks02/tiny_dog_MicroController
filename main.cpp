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

ExtensionController *controller = NULL;
const uint LED = PICO_DEFAULT_LED_PIN;

void splitToSecondCore()
{
    try
    {
        controller->runExtensions();
    }
    catch (...)
    {
        for (int i = 0; i < 4; i++)
        {
            int ledState = gpio_get(LED);
            gpio_put(LED, ledState == 0);
            sleep_ms(200);
        }
        splitToSecondCore();
    }
}

int main()
{
    stdio_init_all();

    controller = new ExtensionController();

    PCA9685 ServoController(0.f, 181.f, 64, 4, 5);

    controller->setServoController(&ServoController);

    gpio_init(LED);

    gpio_set_dir(LED, GPIO_OUT);

    gpio_put(LED, 1);

    SpiInterface communication;

    communication.setToSlave();

    multicore_reset_core1();
    multicore_launch_core1(splitToSecondCore);

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
                controller->setNewExtensionTracker(zeroIndexTracker);
            }
            ExtensionTrackerList.clear();
        }
        if (seriesCommands.size() > 0)
        {
            for (auto &com : seriesCommands)
            {
                controller->prepareNextSeries(com);
            }
            seriesCommands.clear();
        }
        if (movementSeriesList.size() > 0)
        {
            for (auto &ms : movementSeriesList)
            {
                for (auto &ext : controller->endEffectors)
                {
                    controller->setNewMovementSeriesForExtension(ext.name, ms);
                }
            }
            movementSeriesList.clear();
        }
        if (commands.size() > 0)
        {
            for (auto &com : commands)
            {
                controller->setExtensionToPoint(com);
            }
            commands.clear();
        }
        if (clear == true)
        {
            controller->clearAllItems();
            clear = false;
        }
    }
    return 0;
};

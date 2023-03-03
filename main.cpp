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

    MovementSeries test("test", "transition", 20, {{0.f, 20.f, 0.f}, {90.f, -20.f, -60.f}, {-60.f, -20.f, 60.f}}); //{0.f,20.f,0.f},
    MovementSeries test2("test2", "transition", 20, {{0.f, 20.f, 0.f}, {90.f, -20.f,0.f}, {-60.f, -20.f, 0.f}}); //{0.f,20.f,0.f},

    test.increaseResolution(5);
    test2.increaseResolution(5);

    controller.setNewMovementSeriesForExtension("rightFront", test);
    controller.setNewMovementSeriesForExtension("rightFront", test2);

    multicore_reset_core1();
    multicore_launch_core1(secondaryLoop);

    while (true)
    {   
        gpio_put(LED, 1);
        extensionSeriesCommand newCommand("rightFront", "test", 2);
        extensionSeriesCommand dupCom("rightFront", "test2", 2);
        controller.prepareNextSeries(newCommand);
        controller.prepareNextSeries(dupCom);
        controller.prepareNextSeries(newCommand);
        sleep_ms(1000 * 10);
        gpio_put(LED, 0);
        extensionSeriesCommand nextCommand("rightFront", "test", 3);
        controller.prepareNextSeries(nextCommand);
        sleep_ms(1000 * 10);
    }
};

// int main()
// {
//     stdio_init_all();

//     const uint LED = PICO_DEFAULT_LED_PIN;

//     gpio_init(LED);

//     gpio_set_dir(LED, GPIO_OUT);

//     gpio_put(LED, 1);

//     PCA9685 ServoController(0.f, 181.f, 64, 0, 1);

//     vector<vector<float>> yzPlane = {{86.61356, -33.f}, {86.61356, 0.f}, {0.f, 0.f}};
//     vector<vector<float>> xyPlane = {{0.f, 86.61356}, {-80.f, 86.61356}, {0, 0.f}};
//     vector<float> endaffectorCoordinate = {0.f, 0.f, 0.f};
//     vector<PositioningServo> servos = {PositioningServo(0, 'r', 90.f, {0.f, 86.61356, 0.f}), PositioningServo(1, 'l', 32.f, {-80.f, 86.61356, 0.f}, "quadratic"), PositioningServo(2, 'r', 90.f, {0.f, 86.61356, -33.f})}; // {0.f, 110.f, 0.f}, {0.f, 110.f, -33.f}

//     ExtensionTracker rfLeg("rightFront", yzPlane, xyPlane, endaffectorCoordinate, servos);
//     vector<float> legFootForward;

//     rfLeg.getServoAnglesForPoint({0.f, 0.f, 0.f}, &legFootForward);

//     for (int i = 0; i < 3; i++)
//     {
//         ServoController.servoSetAngle(legFootForward[i], i);
//     }

//     // MovementSeries test("test", "transition", 30, {{0.f, 20.f, -60.f}, {90.f, -20.f, 60.f}, {-60.f, -20.f, 0.f}}); //{0.f,20.f,0.f},
//     MovementSeries test("test", "transition", 20, {{0.f, 20.f, 0.f}, {90.f, -20.f, 50.f}, {-60.f, -20.f, -50.f}});

//     test.increaseResolution(5);

//     while (true)
//     {
//         gpio_put(LED, 1);
//         // sleep_ms(1000);
//         for (auto &coordinate : test.series)
//         {
//             rfLeg.getServoAnglesForPoint(coordinate, &legFootForward);
//             for (int i = 0; i < 3; i++)
//             {
//                 ServoController.servoSetAngle(rfLeg.mServos[i].currentAngle, rfLeg.mServos[i].servoIndex);
//             }
//             float coor[3] = {coordinate[0], coordinate[1], coordinate[2]};
//             float angs[3] = {legFootForward[0], legFootForward[1], legFootForward[2]};
//             PositioningServo l[3] = {rfLeg.mServos[0], rfLeg.mServos[1], rfLeg.mServos[2]};
//             for (int i = 0; i < 3; i++)
//             {
//                 cout << coor[i] << angs[i] << l[i].inverted << endl;
//             }
//             sleep_ms(test.millisecondDelay);
//         }
//         gpio_put(LED, 0);
//         // sleep_ms(1000);
//     }

//     return 0;
// }

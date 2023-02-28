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

// ExtensionController controller;

// void secondaryLoop()
// {
//     controller.runExtensions();
// }

// int main()
// {
//     stdio_init_all();

//     const uint LED = PICO_DEFAULT_LED_PIN;

//     gpio_init(LED);

//     gpio_set_dir(LED, GPIO_OUT);

//     gpio_put(LED, 1);

//     PCA9685 ServoController(0.f, 181.f, 64, 0, 1);

//     controller.setServoController(&ServoController);

//     vector<vector<float>> yzPlane = {{86.61356, -33.f}, {86.61356, 0.f}, {0.f, 0.f}};
//     vector<vector<float>> xyPlane = {{0.f, 86.61356}, {-80.f, 86.61356}, {0, 0.f}};
//     vector<float> endaffectorCoordinate = {0.f, 0.f, 0.f};
//     vector<PositioningServo> servos = {PositioningServo(0, 'r', 90.f, {0.f, 86.61356, 0.f}), PositioningServo(2, 'l', 32.f, {-80.f, 86.61356, 0.f}, "quadratic"), PositioningServo(1, 'r', 90.f, {0.f, 86.61356, -33.f})}; // {0.f, 110.f, 0.f}, {0.f, 110.f, -33.f}

//     ExtensionTracker rfLeg("rightFront", yzPlane, xyPlane, endaffectorCoordinate, servos);

//     controller.setNewExtensionTracker(rfLeg);

//     MovementSeries test("test", "transition", 30, {{0.f, 20.f, -60.f}, {90.f, -20.f, 60.f}, {-60.f, -20.f, 0.f}}); //{0.f,20.f,0.f},

//     test.increaseResolution(5);

//     controller.setNewMovementSeriesForExtension("rightFront", test);

//     multicore_launch_core1(secondaryLoop);

//     while (true)
//     {
//         extensionSeriesCommand newCommand("rightFront", "test", 2);
//         controller.prepareNextSeries(newCommand);
//         sleep_ms(1000 * 60 * 2);
//         extensionSeriesCommand nextCommand("rightFront", "test", 3);
//         controller.prepareNextSeries(nextCommand);
//         sleep_ms(1000 * 60 * 2);
//     }
// };

 int main() {
    stdio_init_all();

    const uint LED = PICO_DEFAULT_LED_PIN;

    gpio_init(LED);

    gpio_set_dir(LED, GPIO_OUT);

    gpio_put(LED, 1);

    PCA9685 ServoController(0.f, 181.f, 64, 0, 1);

    vector<vector<float>> yzPlane = {{86.61356, -33.f}, {86.61356, 0.f}, {0.f, 0.f}};
    vector<vector<float>> xyPlane = {{0.f, 86.61356}, {-80.f, 86.61356}, {0, 0.f}};
    vector<float> endaffectorCoordinate = {0.f,0.f,0.f};
    vector<PositioningServo> servos = {PositioningServo(0, 'r', 90.f, {0.f, 86.61356, 0.f}), PositioningServo(2, 'l', 32.f, {-80.f, 86.61356, 0.f}, "quadratic"), PositioningServo(1, 'r', 90.f, {0.f, 86.61356, -33.f})}; // {0.f, 110.f, 0.f}, {0.f, 110.f, -33.f}

    ExtensionTracker rfLeg("rightFront", yzPlane, xyPlane, endaffectorCoordinate, servos);
    vector<float> legFootForward;

    rfLeg.getServoAnglesForPoint({0.f,0.f,0.f}, &legFootForward);

    for(int i = 0; i < 3; i++) {
        ServoController.servoSetAngle(legFootForward[i], i);
    }

    // MovementSeries test("test", "transition", 30, {{0.f, 20.f, -60.f}, {90.f, -20.f, 60.f}, {-60.f, -20.f, 0.f}}); //{0.f,20.f,0.f},
    MovementSeries test("test", "transition", 30, {{0.f, 20.f, 0.f}, {90.f, -20.f, 0.f}, {-60.f, -20.f, 0.f}});

    test.increaseResolution(5);
    sleep_ms(500);

    while (true)
    {
        gpio_put(LED, 1);
        // sleep_ms(1000);
        for (auto & coordinate : test.series) {
            rfLeg.getServoAnglesForPoint(coordinate, &legFootForward);
            for (int i = 0; i < 3; i++)
            {
                ServoController.servoSetAngle(legFootForward[i], rfLeg.mServos[i].servoIndex);
            }
            float coor[3] = {coordinate[0], coordinate[1], coordinate[2]};
            for(int i = 0; i < 3; i++) {
                cout << coor[i] << endl;
            }
            sleep_ms(test.millisecondDelay);
        }
        gpio_put(LED, 0);
        // sleep_ms(1000);
    }

    return 0;
}

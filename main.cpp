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

int main() {
    stdio_init_all();

    const uint LED = PICO_DEFAULT_LED_PIN;

    gpio_init(LED);

    gpio_set_dir(LED, GPIO_OUT);

    gpio_put(LED, 1);

    PCA9685 ServoController(0.f, 181.f, 64, 0, 1);

    ServoController.servoSetAngle(90, 0);
    ServoController.servoSetAngle(90, 1);
    ServoController.servoSetAngle(90, 2);

    vector<vector<float>> yzPlane = {{110.f, -33.f}, {110.f, 0.f}, {0.f,0.f}};
    vector<vector<float>> xyPlane = {{0.f,110.f}, {-80.f,110.f}, {0,0.f}};
    vector<float> endaffectorCoordinate = {0.f,0.f,0.f};
    vector<float> servoAngles = {90.f,90.f,90.f};
    vector<char> movementTypes = {'r', 'l', 'r'};
    vector<vector<float>> servoPlacement = {{0.f, 110.f, 0.f}, {0.f, 110.f, 0.f}, {0.f, 110.f, -33.f}};

    ExtensionTracker rfLeg(yzPlane, xyPlane, endaffectorCoordinate, servoAngles, movementTypes, servoPlacement);

    sleep_ms(1000);

    vector<float> legFootForward;
    MovementSeriesExample series;

    MovementSeries test("test", "transition", 1, {{0.f,50.f,0.f}, {50.f, 0.f, 0.f}, {-50.f, 0.f, 0.f}});

    test.increaseResolution(5);

    ServoController.servoSetAngle(90, 0);
    ServoController.servoSetAngle(90, 1);
    ServoController.servoSetAngle(90, 2);
    sleep_ms(500);

    while (true)
    {
        gpio_put(LED, 1);
        sleep_ms(1000);
        for (auto & coordinate : test.series) {
            rfLeg.getServoAnglesForPoint(coordinate, &legFootForward);
            for (int i = 0; i < 3; i++)
            {
                ServoController.servoSetAngle(legFootForward[i], i);
            }
            float things[3] = {coordinate[0], coordinate[1], coordinate[2]};
            float newAngles[3] = {legFootForward[0], legFootForward[1], legFootForward[2]};
            sleep_ms(series.millisecondDelay);
            cout << things[0] << things[1] << things[2] << newAngles[0] << newAngles[1] << newAngles[2] << endl;
        }
        gpio_put(LED, 0);
        sleep_ms(1000);
    }

    return 0;
}

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

    vector<vector<int>> yzPlane = {{110, -33}, {110, 0}, {0,0}};
    vector<vector<int>> xyPlane = {{0,110}, {-80,110}, {0,0}};
    vector<int> endaffectorCoordinate = {0,0,0};
    vector<float> servoAngles = {90,90,90};
    vector<char> movementTypes = {'r', 'l', 'r'};
    vector<vector<int>> servoPlacement = {{0, 110, 0}, {0, 110, 0}, {0, 110, -33}};

    ExtensionTracker rfLeg(yzPlane, xyPlane, endaffectorCoordinate, servoAngles, movementTypes, servoPlacement);

    sleep_ms(1000);

    vector<float> legFootForward;
    MovementSeriesExample series;

    ServoController.servoSetAngle(90, 0);
    ServoController.servoSetAngle(90, 1);
    ServoController.servoSetAngle(90, 2);
    sleep_ms(500);

    while (true)
    {
        gpio_put(LED, 1);
        sleep_ms(1000);
        for (auto & coordinate : series.series) {
            rfLeg.getServoAnglesForPoint(coordinate, &legFootForward);
            for (int i = 0; i < 3; i++)
            {
                ServoController.servoSetAngle(legFootForward[i], i);
            }
            sleep_ms(series.millisecondDelay);
        }
        gpio_put(LED, 0);
        // sleep_ms(1000);
    }

    return 0;
}

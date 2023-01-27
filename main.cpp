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

int main()
{
    stdio_init_all();

    const uint LED = PICO_DEFAULT_LED_PIN;

    gpio_init(LED);

    gpio_set_dir(LED, GPIO_OUT);

    gpio_put(LED, 1);

    PCA9685 ServoController(0.f, 181.f, 64, 0, 1);

    ServoController.servoSetAngle(90, 0);
    ServoController.servoSetAngle(90, 1);
    ServoController.servoSetAngle(90, 2);

    sleep_ms(1000);

    while (true)
    {
        gpio_put(LED, 1);
        sleep_ms(500);
        ServoController.servoSetAngle(0.0, 2);
        sleep_ms(1000);
        ServoController.servoSetAngle(90.0, 2);
        sleep_ms(1000);
        ServoController.servoSetAngle(180.0,2);
        sleep_ms(1000);
        ServoController.servoSetAngle(90.0, 0);
        ServoController.servoSetAngle(90.0, 1);
        ServoController.servoSetAngle(90.0, 2);
        // ServoController.servoSetAngle(0, 0);
        // sleep_ms(500);
        // ServoController.servoSetAngle(90, 0);
        // sleep_ms(500);
        // ServoController.servoSetAngle(0, 1);
        // sleep_ms(500);
        // ServoController.servoSetAngle(90, 1);
        // sleep_ms(500);
        for(int i = 0; i < 5; i++) {
            ServoController.servoSetAngle(0.f, 2);
            sleep_ms(1000);
            ServoController.servoSetAngle(90.0, 2);
            sleep_ms(1000);
            ServoController.servoSetAngle(179.0, 2);
            sleep_ms(1000);
        }
        // for (int i = 40; i < 180; i++) {
        //     ServoController.servoSetAngle(i, 0);
        //     sleep_ms(10);
        // };
        // for (int i = 180; i > 40; i--) {
        //     ServoController.servoSetAngle(i, 0);
        //     sleep_ms(10);
        // };
        // for (int i = 50; i < 140; i++) {
        //     ServoController.servoSetAngle(i, 1);
        //     sleep_ms(10);
        // };
        // for (int i = 140; i > 50; i--) {
        //     ServoController.servoSetAngle(i, 1);
        //     sleep_ms(10);
        // };
        for (int i = 0; i < 180; i++){
            ServoController.servoSetAngle(i, 2);
            sleep_ms(10);
        };
        for (int i = 180; i > 0; i--) {
            ServoController.servoSetAngle(i, 2);
            sleep_ms(10);
        };
        // for(int i = 0; i < 5; i++) {
        //     ServoController.servoSetAngle(180, 0);
        //     sleep_ms(500);
        //     ServoController.servoSetAngle(90, 0);
        //     sleep_ms(500);
        //     ServoController.servoSetAngle(0, 0);
        //     sleep_ms(500);
        // }
        gpio_put(LED, 0);
        sleep_ms(500);
    }

    return 0;
}

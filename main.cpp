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
    printf("Start Up In Progress");
    controller = new ExtensionController();
    communication = new SpiInterface();
    PCA9685 ServoController(0.f, 181.f, 64, 4, 5);
    controller->setServoController(&ServoController);
    gpio_init(LED);
    gpio_set_dir(LED, GPIO_OUT);
    gpio_put(LED, 1);
    AdcController variableInputs;
    multicore_reset_core1();
    multicore_launch_core1(splitToSecondCore);
    while (true) {
        try {
            gpio_put(LED, 0);
            sleep_ms(500);
            vector<string> applicationBodies = { variableInputs.constructJsonOfOutput(), controller->constructJsonOfBody() };
            string write = returnArrayOfJsonsFromList(&applicationBodies);
            string newCommandSet = communication->transferBytes(&write, &updateComing);
            gpio_put(LED, 1);
            process_command(newCommandSet, newCommandSet.size(), controller);
            applicationBodies.clear();
            write.clear();
        } catch (...) {
            printf("Primary Loop Failure");
        }
    }
    return 0;
};

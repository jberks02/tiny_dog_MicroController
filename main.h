/*
 * picoCPP for Raspberry Pi Pico
 *
 * @version     1.0.0
 * @author      Justin Berkshire
 * @copyright   2021
 * @licence     MIT
 *
 */
#ifndef _PICOCPP_MAIN_HEADER_
#define _PICOCPP_MAIN_HEADER_

/*
 * C++ HEADERS
 */
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <map>

/*
 * PICO HEADERS
 */
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "hardware/adc.h"
#include "hardware/uart.h"

//My HEADERS
#include "./modules/utilityFunctions.cpp"
#include "./modules/utilityClasses.cpp"
#include "./modules/TriangleTracker.cpp"
#include "./modules/positionTrack.cpp"
#include "./modules/PCA9685.cpp"

#ifdef __cplusplus
extern "C"
{
#endif

    /*
 * Usual header code here
 */

#ifdef __cplusplus
}
#endif

#endif // _PICOCPP_MAIN_HEADER_
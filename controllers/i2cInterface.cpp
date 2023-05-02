#include "../main.h"
using namespace std;

uint8_t I2C_ADDR = 0x3E;

class piI2cCommumnication
{
private:
    int numberOfBytesReady = 0;
    string write = "";
    string read = "";
    bool writeNeedsConstruct = true;
public:
    piI2cCommumnication()
    {
        i2c_init(i2c1, 10000);
        i2c_set_slave_mode(i2c1, true, I2C_ADDR);
        gpio_set_function(18, GPIO_FUNC_I2C);
        gpio_set_function(19, GPIO_FUNC_I2C);
        gpio_pull_up(18);
        gpio_pull_up(19);
    }
    void checkAndReadBuffer() {

        if(i2c_get_read_available(i2c1) == 0) return;

        bool stillReading = true;

        //wait for full message to finish sending

        char rxData[numberOfBytesReady];

        i2c_read_raw_blocking(i2c1, (uint8_t *)rxData, numberOfBytesReady);

        read = string(rxData);

        process_command(read, read.size());

        this->numberOfBytesReady = 0;
    }
};

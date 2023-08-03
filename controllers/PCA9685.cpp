#include "../main.h"
using namespace std;

// hardware registers
#define SRV_REG_BASE _u(0x08)
#define MOT_REG_BASE _u(0x28)
#define PWM_REG_ADDRESS _u(0xfe)
#define READ_ADDRESS_ONE _u(0xfa)
#define READ_ADDRESS_TWO _u(0xfb)
#define READ_ADDRESS_THREE _u(0xfc)
#define READ_ADDRESS_FOUR _u(0xfd)
#define READ_READY_ON_DEVICE _u(0x00)
// commands
#define KIT_RESET _u(0x06)
#define KIT_BLOCK_WRITE _u(0x00)
#define PWM_VALUE_20ms_PULSE _u(0x78)
#define WAKE_UP _u(0x01)

class PCA9685
{

private:
    uint I2CAddress;

private:
    uint sda;

private:
    uint scl;

private:
    uint freq = 100000;

private:
    int servoRegOffset = 4;

private:
    double angleMultiplier;

public:
    double sAngleMin = 0.0f;

public:
    double sAngleMax = 180.0f;

public:
    float currentAngle[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

public:
    PCA9685(double servoAngleMin = 0.0f, double servoAngleMax = 180.0f, uint address = 108, uint sdaPin = 8, uint sclPin = 9)
    {
        // establish I2C connection
        // init pins
        sda = sdaPin;
        scl = sclPin;
        I2CAddress = address;
        gpio_init(sda);
        gpio_init(scl);
        // init i2c and set pin function
        i2c_init(i2c0, freq);
        gpio_set_function(sda, GPIO_FUNC_I2C);
        gpio_set_function(scl, GPIO_FUNC_I2C);
        gpio_pull_up(sda);
        gpio_pull_up(scl);
        // found this, not sure what it does
        bi_decl(bi_2pins_with_func(sda, scl, GPIO_FUNC_I2C));
        // put in known position
        swReset();
        // set up servo pwm settings for the board.
        // SET servo pwm clock to 20ms this can vary by servo, but 20ms is typical for hobby servos
        uint8_t pwmbuf[1] = {PWM_VALUE_20ms_PULSE};
        reg_write(PWM_REG_ADDRESS, pwmbuf, 1);
        // begin proper value writing for initialization
        uint8_t zeroOutBuff[1] = {0x00};
        reg_write(READ_ADDRESS_ONE, zeroOutBuff, 1);
        reg_write(READ_ADDRESS_TWO, zeroOutBuff, 1);
        reg_write(READ_ADDRESS_THREE, zeroOutBuff, 1);
        reg_write(READ_ADDRESS_FOUR, zeroOutBuff, 1);
        // come out of sleep
        uint8_t wakeup[1] = {WAKE_UP};
        reg_write(READ_READY_ON_DEVICE, wakeup, 1);
        float minMilli = 175.f;
        float maxMilli = 1275.f;
        angleMultiplier = (maxMilli - minMilli) / (servoAngleMax - servoAngleMin);
    }

private:
    int reg_read(const uint8_t reg, uint8_t *buf, const uint8_t nbytes)
    {

        int num_bytes_read = 0;

        // Check to make sure caller is asking for 1 or more bytes
        if (reg < 0)
        {
            return 0;
        }

        // Read data from register(s) over I2C
        //   i2c_write_blocking(i2c0, I2CAddress, &reg, 1, true);
        num_bytes_read = i2c_read_blocking(i2c0, 108, buf, nbytes, false);

        return num_bytes_read;
    }

private:
    int reg_write(const uint8_t reg, uint8_t *buf, const uint8_t nbytes)
    {

        int num_bytes_read = 0;
        uint8_t msg[nbytes + 1];

        // Check to make sure caller is sending 1 or more bytes
        if (nbytes < 1)
        {
            return 0;
        }

        // Append register address to front of data packet
        msg[0] = reg;
        for (int i = 0; i < nbytes; i++)
        {
            msg[i + 1] = buf[i];
        }

        // Write data to register(s) over I2C
        i2c_write_blocking(i2c0, I2CAddress, msg, (nbytes + 1), false);

        return num_bytes_read;
    }

public:
    void swReset()
    {
        uint8_t buf[1] = {0x01};
        reg_write(KIT_RESET, buf, 1);
    }

public:
    void servoSetAngle(double angle, uint servo)
    {
        if (angle == currentAngle[servo])
            return;

        if (angle > sAngleMax)
            angle = 179.0;
        if (angle < sAngleMin)
            angle = 0.0;
        if (servo > 15)
            throw "Invalid Servo Number Asked to Adjust";

        // Calculate Servo Register
        uint8_t servoRegister = SRV_REG_BASE + (servo * servoRegOffset);
        // Calculate mills to set servo pwm to. this is currently going to use default numbers; dynamic cacalculation examples can be found in https://github.com/jberks02/buttonControlledArm
        int newServoMills = (angle * 2.2755) + 102;

        uint8_t highByte[1] = {(newServoMills >> 8) & 0x01};

        uint8_t lowByte[1] = {newServoMills & 0xFF};
        uint8_t highByteServoReg = servoRegister + 1;

        reg_write(servoRegister, lowByte, 1);
        reg_write(highByteServoReg, highByte, 1);

        currentAngle[servo] = angle;
        
    };
};
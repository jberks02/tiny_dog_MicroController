#include "../main.h"
using namespace std;

class SpiInterface
{

private:
    uint16_t write_buffer[256];

public:
    uint16_t read_buffer[256];

private:
    int writeBufLength;

private:
    Communications *peripherals;

public:
    SpiInterface(Communications *perf, uint tx = 19, uint sck = 18, uint csn = 17, uint rx = 16, uint baudrate = 1000 * 100, uint CPOL = 0, uint CPHA = 0)
    {
        try
        {
            peripherals = perf;
            memset(write_buffer, 0, 32);
            memset(read_buffer, 0, 32);
            // initiatialize spi worker interface
            spi_init(spi0, baudrate);
            spi_set_slave(spi0, true);
            spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
            gpio_set_function(rx, GPIO_FUNC_SPI);
            gpio_set_function(sck, GPIO_FUNC_SPI);
            gpio_set_function(tx, GPIO_FUNC_SPI);
            gpio_set_function(csn, GPIO_FUNC_SPI);
            // Make the SPI pins available to picotool
            bi_decl(bi_4pins_with_func(rx, tx, sck, csn, GPIO_FUNC_SPI));

            // clear_read_buffer();
            // clear_write_buffer();
        }
        catch (...)
        {
            printf("Failure to initialize spi bus");
        }
    }

public:
    void setToSlave()
    {
        spi_set_slave(spi0, true);
    }

public:
    void clear_write_buffer()
    {
        for (int i = 0; i < sizeof(write_buffer); i++)
        {
            write_buffer[i] = 0;
        };
    }

public:
    void clear_read_buffer()
    {
        for (int i = 0; i < sizeof(read_buffer); i++)
        {
            read_buffer[i] = 0;
        };
    }

    public:
        void construct_new_write()
        {
            try
            {

                peripherals->pause_updates = true;

                while (peripherals->updates_occurring == true)
                {
                    tight_loop_contents();
                }

                string newMessage = "{ \"positioningMotors\": [";

                for(int i = 0; i < peripherals->servos.size(); i++) {
                    string jsonOfMotor;
                    peripherals->servos[i].getJsonStringOfClass(&jsonOfMotor);
                    newMessage.append(jsonOfMotor);
                    if(i < peripherals->servos.size() - 1) {
                        newMessage.append(",");
                    }
                }
                newMessage.append("]}");

                peripherals->pause_updates = false;

                char newCommand[newMessage.length()];

                strcpy(newCommand, newMessage.c_str());

                memset(write_buffer, 0, newMessage.length());

                for (int i = 0; i < newMessage.length(); i++)
                {
                    write_buffer[i] = newCommand[i];
                }
            }
            catch (...)
            {
                printf("could not construct new write");
                return;
            }
        }

public:
    int exchangeByteMessage()
    {
        try
        {

            bool readable = spi_is_readable(spi0);
            bool writeable = spi_is_writable(spi0);

            if (writeable == false || readable == false)
                return 0;

            construct_new_write();

            uint16_t check_read[1];
            uint16_t check_write[1] = {sizeof(write_buffer)};

            spi_write16_read16_blocking(spi0, check_write, check_read, 1);

            if (check_read[0] == '\000')
                return 0;

            int exchangeLength;

            if (sizeof(write_buffer) > check_read[0])
            {
                exchangeLength = sizeof(write_buffer);
            }
            else
            {
                exchangeLength = check_read[0];
            }

            memset(read_buffer, 0, check_read[0]);

            int bytesExchanged = spi_write16_read16_blocking(spi0, write_buffer, read_buffer, exchangeLength);

            for (int i = 0; i < 256; i++)
            {
                string command;
                command.push_back(read_buffer[i]);
                peripherals->process_command(command, command.size());
            }

            clear_read_buffer();

            return 1;
        }
        catch (...)
        {
            printf("Single byte exchange failed");
            return -1;
        }
    }
};
#include "../main.h"
using namespace std;

class SpiInterface
{

// private:
//     uint8_t write_buffer[256];

// public:
//     uint8_t read_buffer[256];

// private:
//     int writeBufLength;
    // Communications *peripherals;
    // Communications *perf,
private: string write;
public:
    SpiInterface(uint tx = 19, uint sck = 18, uint csn = 17, uint rx = 16, uint baudrate = 4000, uint CPOL = 0, uint CPHA = 0)
    {
        try
        {
            // peripherals = perf;
            // memset(write_buffer, 0, 32);
            // memset(read_buffer, 0, 32);
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

// public:
    // void clear_write_buffer()
    // {
    //     for (int i = 0; i < sizeof(write_buffer); i++)
    //     {
    //         write_buffer[i] = 0;
    //     };
    // }

// public:
//     void clear_read_buffer()
//     {
//         for (int i = 0; i < sizeof(read_buffer); i++)
//         {
//             read_buffer[i] = 0;
//         };
//     }

    public:
        void construct_new_write()
        {
            try
            {

                pause_updates = true;

                while (updates_occurring == true)
                {
                    tight_loop_contents();
                }
                
                write.swap(loadedWrite);

                pause_updates = false;
                
            }
            catch (...)
            {
                printf("could not construct new write");
                return;
            }
        }

// public:
//     uint waitForByteLengthMessage() {
//             uint8_t check_read[4];
//             uint8_t check_write[4];

//             int sizeOfWrite = sizeof(write_buffer);

//             convertInt32ToUint8Array(check_write, sizeOfWrite);

//             uint8_t temp_read[1];
//             uint8_t temp_write[1] = {0x02};
//             for(int i = 0; i < 6; i++) {
//                 if(i == 0) {
//                     spi_write_read_blocking(spi0, temp_write, temp_read, 1);
//                     if(temp_read[0] == 0x02) continue;
//                     else i--;
//                 } else {
//                     temp_write[0] = check_write[i - 1];
//                     spi_write_read_blocking(spi0, temp_write, temp_read, 1);
//                     if(temp_read[0] == 0x03) continue;
//                     if(i > 4) break;
//                     check_read[i - 1] = temp_read[0];
//                 }
//                 // spi_write
//             };
//             // spi_write_read_blocking(spi0, check_write, check_read, 4);

//             uint returnedLength = convertByteArrayToInt(check_read);

//             uint exchangeLength;

//             if (returnedLength > sizeOfWrite)
//             {
//                 exchangeLength = returnedLength;
//             }
//             else
//             {
//                 exchangeLength = sizeOfWrite;
//             }

//             return exchangeLength;
//     }
    uint exchangeByteMessage()
    {
        try
        {

            construct_new_write();
            
            char rx_length[10];
            char tx_length[10];

            memset(tx_length, 0x00, sizeof(tx_length));
            memset(rx_length, 0x00, sizeof(rx_length));
            
            string writeStrengthLength = to_string(write.size());
            strcpy(tx_length, writeStrengthLength.c_str());

            spi_write_read_blocking(spi0, (uint8_t *)tx_length, (uint8_t *)rx_length, 10);

            uint sentNumber = std::atoi(rx_length);

            uint transactionSize = max(sentNumber, write.size());

            char read_ar[transactionSize];
            char write_ar[transactionSize];

            memset(read_ar, 0x00, transactionSize);
            memset(write_ar, 0x00, transactionSize);

            for(int i = 0; i < write.size(); i++) {
                write_ar[i] = write[i];
            }

            spi_write_read_blocking(spi0, (uint8_t *)write_ar, (uint8_t *)read_ar, transactionSize);

            string read(read_ar);

            process_command(read, read.size());

            return 1;
        }
        catch (...)
        {
            printf("Single byte exchange failed");
            return -1;
        }
    }
};
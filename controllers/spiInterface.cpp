#include "../main.h"
using namespace std;

class SpiInterface
{
private:
    string write = "";
    string read = "";
    bool writeNeedsConstruct = true;
public:
    SpiInterface(uint tx = 19, uint sck = 18, uint csn = 17, uint rx = 16, uint baudrate = 1000*100, uint CPOL = 0, uint CPHA = 0)
    {
        try
        {
            spi_init(spi0, baudrate);
            spi_set_slave(spi0, true);
            spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_LSB_FIRST);
            gpio_set_function(rx, GPIO_FUNC_SPI);
            gpio_set_function(sck, GPIO_FUNC_SPI);
            gpio_set_function(tx, GPIO_FUNC_SPI);
            gpio_set_function(csn, GPIO_FUNC_SPI);
            // Make the SPI pins available to picotool
            bi_decl(bi_4pins_with_func(rx, tx, sck, csn, GPIO_FUNC_SPI));
        }
        catch (...)
        {
            printf("Failure to initialize spi bus");
        }
    }
    void setToSlave()
    {
        spi_set_slave(spi0, true);
    }
    // void spiDynamicExchange()
    // {

    //     string read;

    //     // if(charIndex >= static_cast<int>(write.size())) {
    //     //     charIndex = 0;
    //     //     construct_new_write();
    //     // }

    //     construct_new_write();

    //     char tx[write.size()];
    //     strcpy(tx, write.c_str());

    //     char lastByteRead = 0x00;
    //     int charIndex = 0;

    //     while (lastByteRead != 0x03)
    //     {
    //         uint8_t nextWrite[1];
    //         nextWrite[0] = sizeof(tx) <= charIndex ? (uint8_t)tx[charIndex] : 0x00;
    //         uint8_t rx[1];

    //         spi_write_read_blocking(spi0, nextWrite, rx, 1);
    //         lastByteRead = rx[1];
    //         if (rx[1] > 0x20)
    //         {
    //             read.push_back(static_cast<char>(rx[1]));
    //         }
    //         charIndex = charIndex + 1;
    //     }

    //     process_command(read, read.size());
    // }
    // uint exchangeByteMessage()
    // {
    //     try
    //     {

    //         construct_new_write();

    //         char rx_length[10];
    //         char tx_length[10];

    //         memset(tx_length, 0x00, sizeof(tx_length));
    //         memset(rx_length, 0x00, sizeof(rx_length));

    //         string writeStrengthLength = to_string(write.size());
    //         strcpy(tx_length, writeStrengthLength.c_str());

    //         spi_write_read_blocking(spi0, (uint8_t *)tx_length, (uint8_t *)rx_length, 10);

    //         uint sentNumber = std::atoi(rx_length);

    //         uint transactionSize = max(sentNumber, write.size());

    //         char read_ar[transactionSize];
    //         char write_ar[transactionSize];

    //         memset(read_ar, 0x00, transactionSize);
    //         memset(write_ar, 0x00, transactionSize);

    //         for (int i = 0; i < write.size(); i++)
    //         {
    //             write_ar[i] = write[i];
    //         }

    //         spi_write_read_blocking(spi0, (uint8_t *)write_ar, (uint8_t *)read_ar, transactionSize);

    //         string read(read_ar);

    //         process_command(read, read.size());

    //         return 1;
    //     }
    //     catch (...)
    //     {
    //         printf("Single byte exchange failed");
    //         return -1;
    //     }
    // }

private:
    void construct_new_write()
    {
        try
        {

            pause_updates = true;

            while (updates_occurring == true)
            {
                tight_loop_contents();
            }
            string writeWithStartStop = '\u0002' + loadedWrite + '\u0003';

            write.swap(loadedWrite);

            pause_updates = false;

            writeNeedsConstruct = false;

        }
        catch (...)
        {
            printf("could not construct new write");
            return;
        }
    }

public:
    void transfer32Bytes() {
        try {

            if (spi_is_readable(spi0) == false) return;
            if(writeNeedsConstruct == true) {
                construct_new_write();
            };

            char read_ar[32];
            char write_ar[32];

            memset(read_ar, 0x00, 32);
            memset(write_ar, 0x00, 32);

            for(int i = 0; i < 32; i++) {
                if(write.length() > 0) {
                    write_ar[i] = write[0];
                    write.erase(write.begin());
                } else {
                    writeNeedsConstruct = true;
                }
            }

            spi_write_read_blocking(spi0, (uint8_t *)write_ar, (uint8_t *)read_ar, 32);

            bool readComplete = false; 

            read = read + string(read_ar);

            for(int i = 0; i < 32; i++) {
                if(read_ar[i] == 0x03) readComplete = true;
            };

            if(readComplete == true) {
                process_command(read, read.size());
                read.clear();
            }

        } catch(...) {
            cout << "Failure to transfer 32 Bytes" << endl;
        }
    }
    // void readOutSpiBuffer() {

    //     if(spi_is_readable(spi0) == false) return;
    
    //     vector<char> dynamicRead;
        
    //     while (spi_is_readable(spi0))
    //     {
    //         uint8_t rx[1] = {0x00};
    //         spi_read_blocking(spi0, {0x00}, rx, 1);
    //         if (rx[0] > 0x14)
    //         {
    //             dynamicRead.push_back(rx[0]);
    //         }
    //     }

    //     string readBytes(dynamicRead.data(), dynamicRead.size());

    //     process_command(readBytes, readBytes.size());

    // }
    // void writeOutLatestStatement() {

    //     if(spi_is_writable(spi0) == false) return;

    //     construct_new_write();

    //     char write_ar[write.size()];

    //     memset(write_ar, 0x00, write.size());

    //     for (int i = 0; i < write.size(); i++) {
    //         write_ar[i] = write[i];
    //     };

    //     spi_write_blocking(spi0, (uint8_t *)write_ar, sizeof(write_ar));

    //     loadedWrite.clear();

    // }
    // void fullReadWriteTransfer()
    // {

    //     vector<char> dynamicRead; 
    //     char tx[6] = {0x02, 'P', 'i', 'c', 'o', 0x03};
    //     bool writeReady = spi_is_writable(spi0);
    //     bool readReady = spi_is_readable(spi0);

    //     if(writeReady == true && readReady == false) {
    //         spi_write_blocking(spi0, (uint8_t *)tx, sizeof(tx));
    //     } else if (writeReady == false && readReady == true) {
    //         while(spi_is_readable(spi0)) {
    //             uint8_t rx[1] = {0x00};
    //             spi_read_blocking(spi0, {0x00} ,rx, 1);
    //             if(rx[0] > 0x14) {
    //                 dynamicRead.push_back(rx[0]);
    //             }
    //         }
    //     } else if (writeReady && readReady) {
    //         int byteIndex = 0;
    //         while(spi_is_readable(spi0) && spi_is_writable(spi0)) {
    //             uint8_t nextWrite[1] = {0x00};
    //             nextWrite[0] = sizeof(tx) > byteIndex ? tx[byteIndex] : 0x00;
    //             uint8_t nextRead[1] = {0x00};
    //             spi_write_read_blocking(spi0, nextWrite, nextRead, 1);
    //             if(nextRead[0] > 0x14) dynamicRead.push_back(nextRead[0]);
    //             byteIndex = byteIndex + 1;
    //         }
    //         if(byteIndex < sizeof(tx) && spi_is_writable(spi0)) {
    //             uint remainingWriteLength = sizeof(tx) - byteIndex;
    //             uint8_t remainingWrite[remainingWriteLength];
    //             for(int i = 0; i < remainingWriteLength; i++) {
    //                 remainingWrite[i] = tx[i + remainingWriteLength];
    //             }
    //             spi_write_blocking(spi0, remainingWrite, remainingWriteLength);
    //         }
    //     } else return;

    //     string readBytes(dynamicRead.data(), dynamicRead.size());

    //     process_command(readBytes, readBytes.size());

    // }
};
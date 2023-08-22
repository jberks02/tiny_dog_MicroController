#include "../main.h"
using namespace std;

class SpiInterface {
    private:
    string write = "";
    string read = "";
    bool writeNeedsConstruct = true;
    public:
    SpiInterface(uint tx = 19, uint sck = 18, uint csn = 17, uint rx = 16, uint baudrate = 7000, uint CPOL = 0, uint CPHA = 0) {
        try {
            spi_init(spi0, baudrate);
            spi_set_slave(spi0, true);
            spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_LSB_FIRST);
            gpio_set_function(rx, GPIO_FUNC_SPI);
            gpio_set_function(sck, GPIO_FUNC_SPI);
            gpio_set_function(tx, GPIO_FUNC_SPI);
            gpio_set_function(csn, GPIO_FUNC_SPI);
            // Make the SPI pins available to picotool
            bi_decl(bi_4pins_with_func(rx, tx, sck, csn, GPIO_FUNC_SPI));
        } catch (...) {
            printf("Failure to initialize spi bus");
        }
    }
    void setToSlave() {
        spi_set_slave(spi0, true);
    }

    private:
    void construct_new_write() {
        try {

            pause_updates = true;

            while (updates_occurring == true) {
                tight_loop_contents();
            }
            string writeWithStartStop = '\u0002' + loadedWrite + '\u0003';

            write.swap(loadedWrite);

            pause_updates = false;

            writeNeedsConstruct = false;

        } catch (...) {
            printf("could not construct new write");
            return;
        }
    }

    public:
    void transfer32Bytes() {
        try {

            if (spi_is_readable(spi0) == false) return;
            if (writeNeedsConstruct == true) {
                // construct_new_write();
            };

            char read_ar[32];
            char write_ar[32];

            memset(read_ar, 0x00, 32);
            memset(write_ar, 0x00, 32);

            // for(int i = 0; i < 32; i++) {
            //     if(write.length() > 0) {
            //         write_ar[i] = write[0];
            //         write.erase(write.begin());
            //     } else {
            //         writeNeedsConstruct = true;
            //     }
            // }

            spi_read_blocking(spi0, 0x01, (uint8_t*)read_ar, 32);

            bool readComplete = false;

            read = read + string(read_ar);

            vector<uint8_t> templist;

            for (int i = 0; i < 32; i++) {
                uint8_t byteValue = (uint8_t)read_ar[i];
                templist.push_back(byteValue);
                if (byteValue == 0x03) {
                    readComplete = true;
                }
                if (byteValue < 0x20) {
                    read_ar[i] = ' ';
                }
            };

            if (readComplete == true) {
                process_command(read, read.size());
                cout << templist[1] << endl;
                read.clear();
            }

        } catch (...) {
            cout << "Failure to transfer 32 Bytes" << endl;
        }
    }
};
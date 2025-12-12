#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "spi_lib.h"

int main()
{
    spi_dev_t *dev = spi_init(3, 0, 0, 1000000, 8);
    if (!dev)
    {
        printf("SPI Init failed\n");
        return -1;
    }

    uint8_t tx_buf[] = {0x01, 0x02};
    uint8_t rx_buf[2];

    spi_transfer(dev, tx_buf, rx_buf, 2);

    printf("TX: %02x %02x\n", tx_buf[0], tx_buf[1]);
    printf("RX: %02x %02x\n", rx_buf[0], rx_buf[1]);

    if (tx_buf[0] == rx_buf[0] && tx_buf[1] == rx_buf[1])
    {
        printf("Loopback Test Passed! \n");
    }
    else
    {
        printf("Loopback Test Failed!\n");
    }

    spi_deinit(dev);

    return 0;
}

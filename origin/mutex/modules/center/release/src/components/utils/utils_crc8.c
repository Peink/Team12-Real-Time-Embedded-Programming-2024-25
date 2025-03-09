
#include "utils_crc8.h"

//maxim  x8 + x5 + x4 + 1
uint8_t utils_calc_crc8(const uint8_t *data, uint16_t length)
{
    uint8_t crc;
    uint8_t i;
    crc = 0;
    while(length--)
    {
        crc ^= *data++;
        for(i = 0; i < 8; i++)
        {
            if(crc & 0x01)
            {
                crc = (crc >> 1) ^ 0x8C;
            } else crc >>= 1;
        }
    }
    return crc;
}

uint16_t utils_crc16_addfun(uint8_t b, uint16_t acc)
{
    /* CCITT CRC16 polynomial ^16 + ^12 + ^5 + 1 */
    acc ^= b;
    acc = (acc >> 8) | (acc << 8);
    acc ^= (acc & 0xff00) << 4;
    acc ^= (acc >> 8) >> 4;
    acc ^= (acc & 0xff00) >> 5;
    return acc;
}

unsigned short utils_crc16_datafun(const uint8_t* dat, uint16_t len, uint16_t acc)
{
    int i;

    for (i = 0; i < len; ++i) {
        acc = utils_crc16_addfun(*dat, acc);
        ++dat;
    }
    return acc;
}
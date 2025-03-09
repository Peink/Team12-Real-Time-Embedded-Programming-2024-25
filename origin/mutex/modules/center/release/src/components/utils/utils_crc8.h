#ifndef _UTILS_CRC8_H_
#define _UTILS_CRC8_H_
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t utils_calc_crc8(const uint8_t *data, uint16_t length);
uint16_t utils_crc16_addfun(uint8_t b, uint16_t acc);
unsigned short utils_crc16_datafun(const uint8_t* dat, uint16_t len, uint16_t acc);

#ifdef __cplusplus
}
#endif /* end of __cplusplus */

#endif

#ifndef _UTILS_CRC_H_
#define _UTILS_CRC_H_
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 ***********************************************************************************************************************
 * @brief 计算单个字节与之前数据的crc16值
 ***********************************************************************************************************************
 */
unsigned short utils_crc16_add(uint8_t b, uint16_t acc);

/**
 ***********************************************************************************************************************
 * @brief 计算一端数据与之前数据的crc16值
 ***********************************************************************************************************************
 */
unsigned short utils_crc16_data(const uint8_t* data, uint16_t len,
                                uint16_t acc);

#ifdef __cplusplus
}
#endif /* end of __cplusplus */

#endif
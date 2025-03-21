#include "utils.h"

#include <stdint.h>

/**
 ***********************************************************************************************************************
 * @brief 计算校验和
 * @param data 需要校验的数据
 * @param len 数据长度
 ***********************************************************************************************************************
 */
uint8_t utils_checksum(const uint8_t *data, uint16_t len) {
  uint16_t i;
  uint8_t check_sum = 0;

  for (i = 0; i < len; i++) {
    check_sum += *data++;
  }

  return check_sum;
}

/**
 ***********************************************************************************************************************
 * 字节序转换(大小端)
 ***********************************************************************************************************************
 */
void utils_swap_byte_order(const char *data_in, char *data_out,
                           uint8_t data_size) {
  for (uint8_t i = 0; i < data_size; i++) {
    data_out[i] = data_in[data_size - i - 1];
  }
}

/**
 ***********************************************************************************************************************
 * @brief 转换大小端字节序
 ***********************************************************************************************************************
 */
uint16_t utils_b2i16_swap(char value[2]) {
  uint16_t number = 0;
  number = ((value[1] << 8) & 0xff00) | (value[0] & 0xff);
  return number;
}

/**
 ***********************************************************************************************************************
 * 字符转hex
 ***********************************************************************************************************************
 */
int utils_char2hex(char c, uint8_t *x) {
  if (c >= '0' && c <= '9') {
    *x = c - '0';
  } else if (c >= 'a' && c <= 'f') {
    *x = c - 'a' + 10;
  } else if (c >= 'A' && c <= 'F') {
    *x = c - 'A' + 10;
  } else {
    return -1;
  }

  return 0;
}

/**
 ***********************************************************************************************************************
 * HEX转字符
 ***********************************************************************************************************************
 */
int hex2char(uint8_t x, char *c) {
  if (x <= 9) {
    *c = x + '0';
  } else if (x <= 15) {
    *c = x - 10 + 'a';
  } else {
    return -1;
  }

  return 0;
}

/**
 ***********************************************************************************************************************
 * 二进制数据转HEX
 ***********************************************************************************************************************
 */
uint8_t utils_bin2hex(const uint8_t *buf, uint8_t buflen, char *hex,
                      uint8_t hexlen) {
  if (hexlen < (buflen * 2 + 1)) {
    return 0;
  }

  for (uint8_t i = 0; i < buflen; i++) {
    if (hex2char(buf[i] >> 4, &hex[2 * i]) < 0) {
      return 0;
    }
    if (hex2char(buf[i] & 0xf, &hex[2 * i + 1]) < 0) {
      return 0;
    }
  }

  hex[2 * buflen] = '\0';
  return 2 * buflen;
}

/**
 ***********************************************************************************************************************
 * HEX转二进制数据
 ***********************************************************************************************************************
 */
uint8_t utils_hex2bin(const char *hex, uint8_t hexlen, uint8_t *buf,
                      uint8_t buflen) {
  uint8_t dec;

  if (buflen < hexlen / 2 + hexlen % 2) {
    return 0;
  }

  /* if hexlen is uneven, insert leading zero nibble */
  if (hexlen % 2) {
    if (utils_char2hex(hex[0], &dec) < 0) {
      return 0;
    }
    buf[0] = dec;
    hex++;
    buf++;
  }

  /* regular hex conversion */
  for (uint8_t i = 0; i < hexlen / 2; i++) {
    if (utils_char2hex(hex[2 * i], &dec) < 0) {
      return 0;
    }
    buf[i] = dec << 4;

    if (utils_char2hex(hex[2 * i + 1], &dec) < 0) {
      return 0;
    }
    buf[i] += dec;
  }

  return hexlen / 2 + hexlen % 2;
}

uint16_t calc_avg_filter(filter_t *filter, uint16_t val, uint8_t nums) {
  if (filter->cnt == nums + 1) {
    filter->result = (filter->result * (nums - 1) + val) / nums;
    return filter->result;
  }

  if (filter->cnt == nums) {
    filter->result = filter->result / nums;
    filter->cnt++;
    return filter->result;
  } else {
    filter->result += val;
    filter->cnt++;
    return 0;
  }

  return 0;
}
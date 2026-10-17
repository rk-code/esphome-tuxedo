#pragma once
#include <cstdint>
#include <vector>

// Calculate Modbus RTU CRC16
inline uint16_t modbus_crc16(const std::vector<uint8_t> &data) {
  uint16_t crc = 0xFFFF;

  for (uint8_t b : data) {
    crc ^= b;
    for (int i = 0; i < 8; i++) {
      if (crc & 1) {
        crc = (crc >> 1) ^ 0xA001;
      } else {
        crc >>= 1;
      }
    }
  }

  return crc;
}

// Calculate Modbus RTU CRC16
inline uint16_t modbus_crc16(const uint8_t *data, size_t len) {
  uint16_t crc = 0xFFFF;

  for (size_t i = 0; i < len; i++) {
    crc ^= data[i];

    for (int j = 0; j < 8; j++) {
      if (crc & 1)
        crc = (crc >> 1) ^ 0xA001;
      else
        crc >>= 1;
    }
  }

  return crc;
}

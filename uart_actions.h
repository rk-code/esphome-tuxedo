// uart_actions.h
#pragma once
#include "modbus_crc.h"

#ifndef TEST
  #include "esphome.h"
  #include "esp_log.h"
#else
  #include "myesp.h"
#endif

#include <cstdint>
#include <vector>
#include <cstring>

namespace actions {

inline uint16_t u16_be(const uint8_t *buffer, size_t idx) // AB
{
  return (buffer[idx] << 8) + buffer[idx+1];
}

inline uint16_t u16_le(const uint8_t *buffer, size_t idx) // BA
{
  return (buffer[idx+1] << 8) + buffer[idx];
}

// ------------------------------------------------------------
// FLOAT32, Modbus Big Endian
// ------------------------------------------------------------
//auto float32_be = [](const uint8_t *p) -> float {
inline auto float32_be(const uint8_t *p) -> float {
  uint32_t raw =
      ((uint32_t)p[0] << 24) |
      ((uint32_t)p[1] << 16) |
      ((uint32_t)p[2] << 8)  |
      ((uint32_t)p[3]);

  float value;
  memcpy(&value, &raw, sizeof(value));
  return value;
};

void master(std::vector<uint8_t> bytes) {
  static std::vector<uint8_t> buffer;
  // Neue Bytes an den Puffer anhängen
  for (auto byte : bytes) {
    buffer.push_back(byte);
  }
  ESP_LOGI("MASTER", "RX: %s", format_hex_pretty(bytes).c_str());
  if (buffer.size() >= 8) {
    // each request should have at least 8 bytes:
    //
    uint8_t slave = buffer[0];
    uint8_t fc    = buffer[1];

    uint16_t crc_calc = modbus_crc16(buffer.data(), 6);
    int16_t nr_bytes = 0;
    int16_t nr_response = 0;

    if (crc_calc != u16_le(buffer.data(),6)) {
      ESP_LOGI("MASTER", ">=8 bytes, wrong CRC at [6][7]");
      return;
    }
    uint16_t quantitiy;
    switch (fc) {
      case 0x01: // Read coil:            [slave][fc][reg_addr_h][reg_addr_l][nr_h][nr_l][crc_l][crc_h]
      case 0x02: // Read Discrete Inputs:
        quantitiy = u16_be(buffer.data(),4);
        nr_bytes = quantitiy / 8 + ((quantitiy % 8) ? 1 : 0);
        nr_response = 1 + 1 + nr_bytes + 2; // slave, fc, payload, crc low and high byte
        break;
      case 0x03: // Read Holding Registers [slave][fc][reg_addr_hi][reg_addr_lo][nr_h][nr_l][crc_l][crc_h]
      case 0x04: // Read Input Registers
        quantitiy = u16_be(buffer.data(),4); // number of 2 byte register
        nr_bytes = 2*quantitiy;
        nr_response = 1 + 1 + nr_bytes + 2;
        break;
      case 0x05: // Write Single Coil (8 bytes): [slave][fc][reg_addr_hi][reg_addr_lo][0x00 | 0xFF][0x00][crc_l][crc_h]
        nr_bytes = 2;
        nr_response = 8;
        break;
      case 0x06: // Write Single Holding Register (8 bytes): [slave][fc][reg_addr_hi][reg_addr_lo][val_hi][val_lo][crc_l][crc_h]
        nr_bytes = 2;
        break;
      default:
        break;
    }
    // check crc, crc is transmitted with low byte first and than high byte: [lowByte][highByte]
    uint16_t calc = modbus_crc16(buffer.data(), 6);
    if (calc == u16_le(buffer.data(),6)) {
      // crc ok
      // forward to SLAVE
      // uart_2 !!! uart_1 only for dev
#ifndef TEST
      id(uart_1).write_array(buffer.data(), 8);
#else
      printf("SLAVE UART:%s\n", format_hex_pretty(buffer).c_str());
#endif
      buffer.erase(buffer.begin(), buffer.begin() + 8);
    } else {
      ESP_LOGI("MASTER", "wrong CRC");
    }
  }
}

} // namespace actions

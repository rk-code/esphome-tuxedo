// myesp.h
#include <cstdint>
//#include <iostream>
#include <string>
#include <vector>

#include <cstdio>
#include <cstdarg>
#include <cstdint>
#include <time.h>

std::string format_hex_pretty(std::vector<uint8_t> bytes) {
  std::string s;

  for (uint8_t b : bytes)
  {
    char buffer[3];
    snprintf(buffer, sizeof(buffer), "%02X", b);
    s += buffer;
    s += '.';
  }
  if (s.length() > 0)
    s.pop_back();
  return s;
}

enum LogLevel { LOG_E = 'E', LOG_W = 'W', LOG_I = 'I', LOG_D = 'D' };

static int64_t get_time_us() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

void esp_log_write(LogLevel level, const char* tag, const char* fmt, ...) {
    static int64_t start = get_time_us();
    int64_t us = get_time_us() - start;

    std::printf("[%c][%ld.%03ld][%s]: ", level, us / 1000000, (us / 1000) % 1000, tag);

    va_list args;
    va_start(args, fmt);
    std::vprintf(fmt, args);
    va_end(args);

    std::printf("\n");
}

#define ESP_LOGE(tag, fmt, ...) esp_log_write(LOG_E, tag, fmt, ##__VA_ARGS__)
#define ESP_LOGW(tag, fmt, ...) esp_log_write(LOG_W, tag, fmt, ##__VA_ARGS__)
#define ESP_LOGI(tag, fmt, ...) esp_log_write(LOG_I, tag, fmt, ##__VA_ARGS__)
#define ESP_LOGD(tag, fmt, ...) esp_log_write(LOG_D, tag, fmt, ##__VA_ARGS__)

#include <string>
#include <vector>
#include <sstream>
#include <cstdint>

std::vector<uint8_t> hex_string_to_bytes(const std::string& str) {
    std::vector<uint8_t> data;
    std::istringstream iss(str);
    std::string token;

    while (std::getline(iss, token, '.')) {
        unsigned int value;
        std::istringstream hex_stream(token);

        if ((hex_stream >> std::hex >> value) &&
            value <= 0xFF) {
            data.push_back(static_cast<uint8_t>(value));
        }
    }

    return data;
}

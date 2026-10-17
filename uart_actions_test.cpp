#define TEST

#include "uart_actions.h"

#include <cstdio>
#include <cstdarg>

using namespace std;

int main() {
  vector<uint8_t> byte = { 0x21, 0x02, 0x00, 0x00, 0x00, 0x11, 0xBF, 0x66};

  actions::master(byte);
}

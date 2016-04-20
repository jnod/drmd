#include "drmd.h"

static uint8_t buffer[2];
static uint8_t result_code;
static uint16_t voltage;

int main() {
  if (!bcm2835_init()) {
    printf("bcm2835_init failed. Are you running as root??\n");
    return 1;
  }

  if (!bcm2835_i2c_begin()) {
    printf("bcm2835_i2c_begin failed. Are you running as root??\n");
    return 1;
  }

  bcm2835_i2c_setSlaveAddress(0b00010100);
  bcm2835_i2c_set_baudrate(100000);

  while (true) {
    result_code = bcm2835_i2c_read(buffer, 2);

    switch (result_code) {
      case BCM2835_I2C_REASON_OK:
        voltage = buffer[0] << 8 | buffer[1];
        printf("%d\n", voltage);
        break;
      case BCM2835_I2C_REASON_ERROR_NACK:
      case BCM2835_I2C_REASON_ERROR_CLKT:
      case BCM2835_I2C_REASON_ERROR_DATA:
    }
  }

  return 0;
}
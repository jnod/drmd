#include "drmd.h"

static uint8_t buffer[2];
static uint8_t result_code;
static uint16_t voltage;

int main() {
  if (!bcm2835_init()) {
    printf("bcm2835_init failed. Are you running as root??\n");
    return 1;
  }

  bcm2835_i2c_begin();
  bcm2835_i2c_setSlaveAddress(0b00010100);
  bcm2835_i2c_set_baudrate(100000);
  bcm2835_gpio_fsel(RPI2_GPIO_4, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_write(RPI2_GPIO_4, LOW);

  int count = 0;
  int sum = 0;

  while (1) {
    result_code = bcm2835_i2c_read(buffer, 2);

    switch (result_code) {
      case BCM2835_I2C_REASON_OK:
        voltage = buffer[0] << 8 | buffer[1];
        sum += voltage;
        count++;
        break;
      case BCM2835_I2C_REASON_ERROR_NACK:
      case BCM2835_I2C_REASON_ERROR_CLKT:
      case BCM2835_I2C_REASON_ERROR_DATA:
        break;
    }

    if (count >= 30) {
      printf("%d\n", (sum / count));
      count = 0;
      sum = 0;
    }
  }

  return 0;
}

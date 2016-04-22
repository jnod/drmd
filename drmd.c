#include "drmd.h"

static typedef enum { TRUE, FALSE } Bool;
static typedef enum { MOVE_STEPPER, READ_UV, UI } State;

static void   clean();
static int    initialize();
static void   interrupt(int);
static State  machine(State);
static State  stateMoveStepper();
static State  stateReadUV(Bool);
static State  stateUI();

static Bool       sigint_set = FALSE;
static uint8_t    buffer[2];
static uint16_t   voltage;

int main() {
  if (initialize()) return 1; // exit if there was an error initializing
  State state = UI;

  while (1) {
    if (sigint_set == TRUE) {
      if (state == UI) break;
      state = UI;
      sigint_set = FALSE;
    }
    state = machine(state);
  }

  clean();
  return 0;
}

static void clean() {
  // revert to default state
}

static int initialize() {
  if (!bcm2835_init()) {
    printf("bcm2835_init failed. Are you running as root??\n");
    return 1;
  }

  bcm2835_i2c_begin();
  bcm2835_i2c_set_baudrate(100000);

  bcm2835_i2c_setSlaveAddress(0b00010100); // Select UV LED ADC
  buffer[0] = 0b10100000; // Code to enable continuous calibration on ADC
  bcm2835_i2c_write(buffer, 1);

  bcm2835_gpio_fsel(PDN1, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_write(PDN1, HIGH); // Turn on UV LED

  signal(SIGINT, interrupt); // Set interrupt(int) to handle Ctrl+c events

  return 0;
}

static void interrupt(int signo) {
  if (signo == SIGINT) {
    sigint_set = TRUE;
  }
}

static State machine(State state) {
  switch (state) {
    case MOVE_STEPPER:
      return stateMoveStepper();
    case READ_UV:
      return stateReadUV();
    case UI:
      return stateUI;
  }
}

static State stateReadUV(Bool continuous) {
  static int count = 0;
  static int sum = 0;

  bcm2835_i2c_setSlaveAddress(0b00010100); // Select UV LED ADC

  switch (bcm2835_i2c_read(buffer, 2)) {
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

  if (count >= 15) {
    printf("%.2f%%\n", ((float)sum / count / 65536 * 100));
    count = 0;
    sum = 0;

    if (continuous == FALSE) return UI; 
  }

  return READ_UV;
}

static State stateMoveStepper() {
  printf("(move stepper)");

  return UI;
}

static State stateUI() {

}
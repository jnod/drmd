#include "drmd.h"

typedef enum { TRUE, FALSE } Bool;
typedef enum { MOVE_STEPPER, READ_UV, UI } State;

static void       cleanAndExit();
static void       configInput(uint8_t, uint8_t);
static void       configOutput(uint8_t, uint8_t);
static int64_t    getTimestampNs();
static int        initialize();
static void       interrupt(int);
static State      machine(State);
static State      stateMoveStepper();
static State      stateReadUV();
static State      stateUI();
static void       writeToPin(uint8_t, uint8_t);

static Bool       sigint_set = FALSE;
static State      state = UI;
static uint8_t    buffer[2];
static uint16_t   voltage;
static int        stepper_position = 0;
static int        stepper_target = 0;

int main() {
  if (initialize()) return 1; // exit if there was an error initializing

  while (1) {
    if (sigint_set == TRUE) {
      state = UI;
      printf("\n");

      sigint_set = FALSE;
    }

    state = machine(state);
  }

  return 0;
}

static void cleanAndExit() {
  configInput(PDN1, PULLDOWN);
  configInput(PDN3, PULLDOWN);

  exit(0);
}

static void configInput(uint8_t pin, uint8_t pullmode) {
  bcm2835_gpio_fsel(pin, INPUT);
  bcm2835_gpio_set_pud(pin, pullmode);
}

static void configOutput(uint8_t pin, uint8_t logiclevel) {
  bcm2835_gpio_fsel(pin, OUTPUT);
  bcm2835_gpio_write(pin, logiclevel);
}

static int64_t getTimestampNs() {
  struct timespec currenttime;
  clock_gettime(CLOCK_MONOTONIC, &currenttime);
  return currenttime.tv_sec * 1e9 + currenttime.tv_nsec;
}

static int initialize() {
  if (!bcm2835_init()) {
    printf("Error: bcm2835_init failed. Must run as root.\n");
    return 1;
  }

  bcm2835_i2c_begin();
  bcm2835_i2c_set_baudrate(100000);

  bcm2835_i2c_setSlaveAddress(0b00010100); // Select UV LED ADC
  buffer[0] = 0b10100000; // Code to enable continuous calibration on ADC
  bcm2835_i2c_write(buffer, 1);

  configOutput(PDN1, LOW); // uv led off
  configOutput(PDN3, LOW); // peristaltic pump off

  signal(SIGINT, interrupt); // Set interrupt(int) to handle Ctrl+c events

  return 0;
}

static void interrupt(int signo) {
  if (signo == SIGINT) {
    if (state == UI) {
      printf("\n");
      cleanAndExit();
    }
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
      return stateUI();
  }
}

static State stateReadUV() {
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
    printf("%.2f %%\n", ((float)sum / count / 65536 * 100));
    count = 0;
    sum = 0;
  }

  return READ_UV;
}

static State stateMoveStepper() {
  static int64_t prevtime = 0;
  int64_t currenttime = getTimestampNs();
  if (prevtime == 0) prevtime = currenttime;

  printf("(target = %d, timestamp = %ld)\n", stepper_target, currenttime);
  stepper_position = stepper_target;

  return UI;
}

static State stateUI() {
  static char command[100];

  printf("> ");
  fgets(command, 100, stdin);

  if (strncmp(command, "exit", 4) == 0) {
    cleanAndExit();
  } else if (strncmp(command, "read uv", 7) == 0) {
    printf("Press (ctrl+c) to stop:\n");
    return READ_UV;
  } else if (strncmp(command, "move stepper", 12) == 0) {
    int distance = 0;
    
    if (command[12] != '\0' && command[13] != '\0') {
      if (sscanf(&command[13], "%d", &distance)) {
        stepper_target = stepper_position + distance;
        return MOVE_STEPPER;
      } else {
        printf("Error: Third parameter must be an integer.\n");
      }
    } else {
      printf("Error: Must provide an integer distance X to move (move stepper X).\n");
    }
  } else if (strncmp(command, "pump on", 7) == 0) {
    bcm2835_gpio_write(PDN3, HIGH);
    printf("Peristaltic pump successfully activated.\n");
  } else if (strncmp(command, "pump off", 8) == 0) {
    bcm2835_gpio_write(PDN3, LOW);
    printf("Peristaltic pump successfully deactivated.\n");
  } else if (strncmp(command, "uv on", 5) == 0) {
    bcm2835_gpio_write(PDN1, HIGH);
    printf("UV LED successfully activated.\n");
  } else if (strncmp(command, "uv off", 6) == 0) {
    bcm2835_gpio_write(PDN1, LOW);
    printf("UV LED successfully deactivated.\n");
  } else {
    printf("Error: Invalid command.\n");
  }

  return UI;
}

static void writeToPin(uint8_t pin, uint8_t logiclevel) {

}
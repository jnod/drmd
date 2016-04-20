#ifndef DRMD_H_
#define DRMD_H_

#include <bcm2835.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// GPIO pin mappings to the bcm2835 for rpi b+ and rpi 2 40 pin headers
#define RPI_GPIO_2     RPI_BPLUS_GPIO_J8_03
#define RPI_GPIO_3     RPI_BPLUS_GPIO_J8_05
#define RPI_GPIO_4     RPI_BPLUS_GPIO_J8_07
#define RPI_GPIO_5     RPI_BPLUS_GPIO_J8_29
#define RPI_GPIO_6     RPI_BPLUS_GPIO_J8_31
#define RPI_GPIO_7     RPI_BPLUS_GPIO_J8_26
#define RPI_GPIO_8     RPI_BPLUS_GPIO_J8_24
#define RPI_GPIO_9     RPI_BPLUS_GPIO_J8_21
#define RPI_GPIO_10    RPI_BPLUS_GPIO_J8_19
#define RPI_GPIO_11    RPI_BPLUS_GPIO_J8_23
#define RPI_GPIO_12    RPI_BPLUS_GPIO_J8_32
#define RPI_GPIO_13    RPI_BPLUS_GPIO_J8_33
#define RPI_GPIO_14    RPI_BPLUS_GPIO_J8_08
#define RPI_GPIO_15    RPI_BPLUS_GPIO_J8_10
#define RPI_GPIO_16    RPI_BPLUS_GPIO_J8_36
#define RPI_GPIO_17    RPI_BPLUS_GPIO_J8_11
#define RPI_GPIO_18    RPI_BPLUS_GPIO_J8_12
#define RPI_GPIO_19    RPI_BPLUS_GPIO_J8_35
#define RPI_GPIO_20    RPI_BPLUS_GPIO_J8_38
#define RPI_GPIO_21    RPI_BPLUS_GPIO_J8_40
#define RPI_GPIO_22    RPI_BPLUS_GPIO_J8_15
#define RPI_GPIO_23    RPI_BPLUS_GPIO_J8_16
#define RPI_GPIO_24    RPI_BPLUS_GPIO_J8_18
#define RPI_GPIO_25    RPI_BPLUS_GPIO_J8_22
#define RPI_GPIO_26    RPI_BPLUS_GPIO_J8_37
#define RPI_GPIO_27    RPI_BPLUS_GPIO_J8_13

#define RPI_SDA1       RPI_GPIO_2
#define RPI_SCL1       RPI_GPIO_3
#define RPI_PDN1       RPI_GPIO_4

#endif
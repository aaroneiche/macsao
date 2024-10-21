#include "lib_swi2c.h"

static i2c_device_t device = {
    // .pin_scl = GPIO_PC3,
    // .pin_sda = GPIO_PC4,
    .pin_scl = GPIO_PA2,
    .pin_sda = GPIO_PD6,

    // .address = 0x3C,
    .address = 0x78,
};

static i2c_device_t eeprom = {
    .pin_scl = GPIO_PA2,
    .pin_sda = GPIO_PD6,
    .address = 0xA0,
};
#ifndef OUR_DRIVER_H__
#define OUR_DRIVER_H__

#include <stdint.h>
#include <zephyr/device.h>

typedef struct our_driver_data {
    uint32_t counter;
} our_driver_data;

uint32_t our_driver_get_count(const struct device *dev);

#endif
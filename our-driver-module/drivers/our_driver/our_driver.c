#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>
#include "our_driver.h"

#define DT_DRV_COMPAT our_driver

//Turn off LED
static int channel_get_my_impl (const struct device *dev,
                                enum sensor_channel chan,
                                struct sensor_value *val )
{    
    const struct gpio_dt_spec *led = dev->config;

    return gpio_pin_set_dt(led, 0);
}

//Turn on LED
static int sample_fetch_my_impl (const struct device *dev,
				                 enum sensor_channel chan)
{
    const struct gpio_dt_spec *led = dev->config;
    our_driver_data *data = dev->data;

    int ret = gpio_pin_set_dt(led, 1);
    //Increment the counter
    if (!ret)
        data->counter++;
    
    return ret;
}

//Counter for turn on times
uint32_t our_driver_get_count (const struct device *dev)
{
    const our_driver_data *data = dev->data;

    return data->counter;
}

static DEVICE_API(sensor, our_driver_api) = {
    .channel_get = channel_get_my_impl,
    .sample_fetch = sample_fetch_my_impl,
};

//Init function
static int init (const struct device* dev) 
{
    const struct gpio_dt_spec *led = dev->config;

    if (!gpio_is_ready_dt(led)) {
        return -ENODEV;
    }

    return gpio_pin_configure_dt(led, GPIO_OUTPUT_INACTIVE);
}

static our_driver_data our_driver_data_0;
static const struct gpio_dt_spec our_driver_led_0 = GPIO_DT_SPEC_INST_GET(0, led_gpios);

DEVICE_DT_INST_DEFINE(0, init, NULL, &our_driver_data_0, &our_driver_led_0, POST_KERNEL, 80, &our_driver_api);

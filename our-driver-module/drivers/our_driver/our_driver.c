#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>

#define DT_DRV_COMPAT our_driver

typedef struct our_driver_config {
    struct gpio_dt_spec led;
}driver_cfg;

//Turn off LED
static int channel_get_my_impl (const struct device *dev,
                                enum sensor_channel chan,
                                struct sensor_value *val )
{    
    const driver_cfg *cfg = dev->config;

    return gpio_pin_set_dt(&cfg->led, 0);
}

//Turn on LED
static int sample_fetch_my_impl (const struct device *dev,
				                 enum sensor_channel chan)
{
    const driver_cfg *cfg = dev->config;

    return gpio_pin_set_dt(&cfg->led, 1);
}

static DEVICE_API(sensor, our_driver_api) = {
    .channel_get = channel_get_my_impl,
    .sample_fetch = sample_fetch_my_impl,
};

//Init function
static int init (const struct device* dev) 
{
    const driver_cfg *cfg = dev->config;

    if (!gpio_is_ready_dt(&cfg->led)) {
        return -ENODEV;
    }

    return gpio_pin_configure_dt(&cfg->led, GPIO_OUTPUT_INACTIVE);
}

static const driver_cfg our_driver_cfg_0 = {
    .led = GPIO_DT_SPEC_INST_GET(0, led_gpios),
};

DEVICE_DT_INST_DEFINE(0, init, NULL, NULL, &our_driver_cfg_0, POST_KERNEL, 80, &our_driver_api);

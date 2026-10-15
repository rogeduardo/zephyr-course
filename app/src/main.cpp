#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/sensor.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    const struct device *dev = DEVICE_DT_GET_ANY(our_driver);

    if (!device_is_ready(dev)) {
        return -ENODEV;
    }

    while (1) {
        //Turn on
        sensor_sample_fetch(dev);
        LOG_INF("Driver LED state: ON");
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
        sensor_channel_get(dev, SENSOR_CHAN_ALL, NULL);
        LOG_INF("Driver LED state: OFF");
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);        
    }
    return 0;
}

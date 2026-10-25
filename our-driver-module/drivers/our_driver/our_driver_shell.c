#include <zephyr/shell/shell.h>
#include <zephyr/drivers/sensor.h>
#include "our_driver.h"

//Handler for shell subcommand fetch
static int cmd_fetch_handler ( const struct shell *sh, int argc, char** argv )
{
    char *dev_name = argv[1];
    const struct device *dev = shell_device_get_binding (dev_name);
    if (!dev)
    {
        shell_error (sh, "Could not find device %s", dev_name);
        return -EFAULT;
    }

    int ret = sensor_sample_fetch (dev);
    if (ret != 0)
    {
        shell_error (sh, "Could not fetch channel, got %d", ret);
        return -EFAULT;
    }
    return 0;
}

//Handler for shell subcommand read
static int cmd_read_handler ( const struct shell *sh, int argc, char** argv )
{
    char *dev_name = argv[1];
    const struct device *dev = shell_device_get_binding (dev_name);
    if (!dev)
    {
        shell_error (sh, "Could not find device %s", dev_name);
        return -EFAULT;
    }

    int ret = sensor_channel_get (dev, 0, NULL);
    if (ret != 0)
    {
        shell_error (sh, "Could not fetch channel, got %d", ret);
        return -EFAULT;
    }
    shell_print (sh, "%d", ret );
    return 0;
}

//Handler for shell subcommand info
static int cmd_info_handler (const struct shell *sh, int argc, char** argv )
{
    char* dev_name = argv[1];
    const struct device *dev = shell_device_get_binding (dev_name);
    if (!dev)
    {
        shell_error (sh, "Could not find device %s", dev_name);
        return -EFAULT;
    }

    shell_print (sh, "Device Name: %s", dev->name);
    shell_print (sh, "State: %s", device_is_ready(dev) ? "READY" : "UNREADY" );
    return 0;
}

//Handler for shell subcommand set
static int cmd_set_handler (const struct shell *sh, int argc, char** argv )
{    
    long new_counter;
    int err = 0;

    //Check if the value passed is valid
    if (argv[1][0] == '-' )
    {
        shell_error ( sh, "The value informed must not be negative: %s", argv[1] );
        return -EINVAL;
    }
    
    new_counter = shell_strtoul ( argv[1], 10, &err );
    //More checks if the value passed is valid
    if (err)
    {
        shell_error ( sh, "The value informed is not valid: %s", argv[1] );
        return -EINVAL;
    }
    //This check is for platform where long is 64 bits
    if (new_counter > UINT32_MAX)
    {
        shell_error ( sh, "The value informed is out of range: %s", argv[1] );
        return -ERANGE;
    }

    char* dev_name = argv[2];
    const struct device *dev = shell_device_get_binding (dev_name);

    //Check if the device exist
    if (!dev)
    {
        shell_error (sh, "Could not find device %s", dev_name);
        return -EFAULT;
    }

    our_driver_data *data = dev->data;
    data->counter = (uint32_t) new_counter;

    return 0;
}

//Add fetch, read and info subcommands
SHELL_STATIC_SUBCMD_SET_CREATE (sensor_subcmd,
    SHELL_CMD_ARG (fetch, NULL, "Sample fetch of our driver", cmd_fetch_handler, 2, 0),
    SHELL_CMD_ARG (read, NULL, "Get channel of our driver", cmd_read_handler, 2, 0),
    SHELL_CMD_ARG (info, NULL, "Print the device name and ready state", cmd_info_handler, 2, 0),
    SHELL_CMD_ARG (set, NULL, "Set count (how many times LED blinked). sensor set <num> <device>", cmd_set_handler, 3, 0),
    SHELL_SUBCMD_SET_END,
);
//Add sensor root shell
SHELL_CMD_REGISTER(sensor, &sensor_subcmd, "Our Driver set of commands", NULL);
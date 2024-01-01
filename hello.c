#include <linux/miscdevice.h>
#include <linux/i2c.h>
#include "convenient.h"

#include <net/sock.h>
#include <linux/netlink.h>

#include "netlink.h"
// ****************************** METADATA *************************** //
MODULE_AUTHOR("Anton Khomich scphamster");
MODULE_DESCRIPTION("i2c test driver");
MODULE_LICENSE("Dual MIT/GPL");
MODULE_VERSION("0.1");
// declarations for quick reference
int  probe(struct i2c_client *client, const struct i2c_device_id *id);
void remove(struct i2c_client *client);
void shutdown(struct i2c_client *client);
void alert(struct i2c_client *client, enum i2c_alert_protocol protocol, unsigned int data);
int  command(struct i2c_client *client, unsigned int cmd, void *arg);
int  detect(struct i2c_client *client, struct i2c_board_info *info);

// ****************************** GLOBALS ***************************** //
// I2C
const struct i2c_device_id  test_device_i2c_match_table[]  = { { "test_i2c", 0 }, {} };
const struct of_device_id   test_device_of_match_table[]   = { {
                                                                 .name       = "test_i2c",
                                                                 .compatible = "scphamster, test_i2c",
                                                             // to be defined
                                                             //.type =
                                                             //.data =
                                                           },
                                                               {} };
const struct acpi_device_id test_device_acpi_match_table[] = { { "test_i2c", 12345 }, {} };
MODULE_DEVICE_TABLE(i2c, test_device_i2c_match_table);
MODULE_DEVICE_TABLE(of, test_device_of_match_table);
MODULE_DEVICE_TABLE(acpi, test_device_acpi_match_table);

struct i2c_driver g_test_driver = { .driver   = { .name             = test_device_i2c_match_table[0].name,
                                                  .of_match_table   = test_device_of_match_table,
                                                  .acpi_match_table = test_device_acpi_match_table },
                                    .probe    = probe,
                                    .remove   = remove,
                                    .shutdown = shutdown,
                                    .alert    = alert,
                                    .command  = command,
                                    .detect   = detect,
                                    .id_table = test_device_i2c_match_table };

static int __init
test_module_init(void)
{
    pr_info("Test module init invoked!");

    if (netlink_simple_intf_init()) {
        pr_err("Netlink setup failed!");
    }

    return i2c_add_driver(&g_test_driver);
}

static void __exit
test_module_deinit(void)
{
    pr_info("Test module exit invoked!");

    netlink_simple_intf_deinit();
    i2c_del_driver(&g_test_driver);
}

module_init(test_module_init);
module_exit(test_module_deinit);

/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////IMPLEMENTATION////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

int
probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    pr_info("Hello from i2c test module this is :: %s", __FUNCTION__);
    if (id) {
        pr_info("i2c device name: %s , data: %lu", id->name, id->driver_data);
    }
    else {
        pr_warn("id is null!");
    }

    if (client) {
        pr_info("i2c client name: %s, addr: %us", client->name, client->addr);
    }
    else {
        pr_warn("i2c client is null!");
    }

    const unsigned char test_buffer[3] = { 0x55, 0xa5, 0x5a };
    i2c_master_send(client, test_buffer, sizeof(test_buffer));

    return 0;
}

void
remove(struct i2c_client *client)
{
    pr_info("Hello from i2c test module this is :: %s", __FUNCTION__);
    if (client) {
        pr_info("i2c client name: %s, addr: %us", client->name, client->addr);
    }
    else {
        pr_warn("i2c client is null!");
    }
}

void
shutdown(struct i2c_client *client)
{
    pr_info("Hello from i2c test module this is :: %s", __FUNCTION__);
    if (client) {
        pr_info("i2c client name: %s, addr: %us", client->name, client->addr);
    }
    else {
        pr_warn("i2c client is null!");
    }
}

void
alert(struct i2c_client *client, enum i2c_alert_protocol protocol, unsigned int data)
{
    pr_info("Hello from i2c test module this is :: %s", __FUNCTION__);

    if (client) {
        pr_info("i2c client name: %s, addr: %us", client->name, client->addr);
    }
    else {
        pr_warn("i2c client is null!");
    }
}

int
command(struct i2c_client *client, unsigned int cmd, void *arg)
{
    pr_info("Hello from i2c test module this is :: %s", __FUNCTION__);
    if (client) {
        pr_info("i2c client name: %s, addr: %us", client->name, client->addr);
    }
    else {
        pr_warn("i2c client is null!");
    }

    return 0;
}

int
detect(struct i2c_client *client, struct i2c_board_info *info)
{
    pr_info("Hello from i2c test module this is :: %s", __FUNCTION__);

    if (client) {
        pr_info("i2c client name: %s, addr: %us", client->name, client->addr);
    }
    else {
        pr_warn("i2c client is null!");
    }

    if (info) {
        pr_info("i2c board dev name: %s, addr: %us", info->dev_name, info->addr);
    }
    else {
        pr_warn("i2c board info is null!");
    }

    return 0;
}

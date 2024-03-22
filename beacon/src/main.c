
#include <zephyr/random/rand32.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/types.h>
#include <stddef.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

/* Set Scan Response data */
static const struct bt_data sd[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static void bt_ready(int err)
{
    char addr_s[BT_ADDR_LE_STR_LEN];
    bt_addr_le_t addr = {0};
    size_t count = 1;
    double longtitude, latitude;
	float vbat;
    if (err) {
        printk("Bluetooth init failed (err %d)\n", err);
        return;
    }

    printk("Bluetooth initialized\n");

      vbat = 3.81;
	  int vbat_SEND = vbat*100; 
	  longtitude = 49.057571;
	  uint32_t longtitude_SEND = longtitude*1000000;
	  latitude = 20.312226;
	  uint32_t latitude_SEND = latitude*1000000;
      

printk("Sending\n");
printk("Vbat: %d\n", vbat_SEND);
printk("Longtitude_SEND: %u\n", longtitude_SEND);
printk("Latitude_SEND: %u\n", latitude_SEND);


    /* Custom Advertisement data to broadcast Temperature, Humidity, and CO2 */
    struct bt_data ad[] = {
        BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
        BT_DATA_BYTES(BT_DATA_UUID128_ALL, 0x12, 0x34, 0x67, 0x78), // Example custom service UUID (0x1234)
		BT_DATA_BYTES(BT_DATA_SVC_DATA128,
                      0x12, 0x34, 0x67, 0x78, // Custom service UUID again
                      vbat_SEND, 
                      longtitude_SEND,
					  latitude_SEND, 
                      vbat_SEND,					  
					  longtitude_SEND, 
                      latitude_SEND), // Splitting CO2 into 2 bytes
					  
    };

    /* Start advertising */
    err = bt_le_adv_start(BT_LE_ADV_NCONN_IDENTITY, ad, ARRAY_SIZE(ad),
                          sd, ARRAY_SIZE(sd));
    if (err) {
        printk("Advertising failed to start (err %d)\n", err);
        return;
    }

    bt_id_get(&addr, &count);
    bt_addr_le_to_str(&addr, addr_s, sizeof(addr_s));

    printk("Beacon started, advertising as %s\n", addr_s);
}

int main(void)
{
    int err;

    printk("Starting Sensor Beacon Demo\n");

    /* Initialize the Bluetooth Subsystem */
    err = bt_enable(bt_ready);
    if (err) {
        printk("Bluetooth init failed (err %d)\n", err);
    }
	k_sleep(K_SECONDS(15));
	sys_reboot(SYS_REBOOT_COLD);
    return 0;
}
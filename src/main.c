// SPDX-License-Identifier: GPL-2.0-or-later

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <sys/ioctl.h>

#include "pmic.h"


#define DEFAULT_I2C_PATH	"/dev/i2c-0"

struct ravpower_pmic_status {
	char device_status;
	uint8_t battery_volume;
};

static int i2c_open(const char *devpath)
{
	int fd = open(devpath, O_RDWR);

	if (fd == -1)
		perror(devpath);

	return fd;
}

static int i2c_close(int i2c_fd)
{
	int ret = close(i2c_fd);

	if (ret)
		perror("i2c_close");

	return ret;
}

static int i2c_set_address(int i2c_fd, long address)
{
	if (ioctl(i2c_fd, I2C_SLAVE, address) < 0) {
		perror("i2c_set_address");
		return -1;
	}
}

static int i2c_w(int i2c_fd, char addr)
{
	if (write(i2c_fd, &addr, 1) != 1) {
		perror("i2c_w");
		return -1;
	}

	return 0;
}

static int i2c_wr(int i2c_fd, char reg, char *buf)
{
	if (i2c_w(i2c_fd, reg))
		return -1;

	if (read(i2c_fd, buf, 1) != 1) {
		perror("i2c_rw");
		return -1;
	}

	return 0;
}

static int read_pmic_status(int i2c_fd, struct ravpower_pmic_status *buf)
{
	i2c_wr(i2c_fd, RAVPOWER_PMIC_DEVICE_STATUS_REG, &buf->device_status);
	i2c_wr(i2c_fd, RAVPOWER_PMIC_BATTERY_VOLUME_REG, (char *)&buf->battery_volume);
}

static int cmd_help(char *app_path)
{
	printf("Usage: %s [command]\n", app_path);
	printf("Avbailable commands:\n");
	printf("\tdump\t\t\tDump PMIC status\n");
	printf("\tpoweroff\t\tTurn off the device\n");
	printf("\tbattery-charge\t\tPrint battery charge in percent\n");
	printf("\tbattery-charging\tPrint battery charging status\n");
}

static int cmd_dump(int i2c_fd)
{
	struct ravpower_pmic_status pmic_status;

	read_pmic_status(i2c_fd, &pmic_status);

	printf("Charge:\t\t %d%%\n", pmic_status.battery_volume);
	printf("Charging:\t %s\n", RAVPOWER_PMIC_DEVICE_STATUS_CHARGING(pmic_status.device_status) ? "Yes" : "No");
}

static int cmd_poweroff(int i2c_fd)
{
	i2c_w(i2c_fd, RAVPOWER_PMIC_CMD_SHUTDOWN_REG);
	i2c_w(i2c_fd, RAVPOWER_PMIC_CMD_POWER_LED_REG);
}

static int cmd_battery_charge(int i2c_fd)
{
	struct ravpower_pmic_status pmic_status;

	read_pmic_status(i2c_fd, &pmic_status);

	printf("%d\n", pmic_status.battery_volume);
}

static int cmd_battery_charging(int i2c_fd)
{
	struct ravpower_pmic_status pmic_status;

	read_pmic_status(i2c_fd, &pmic_status);

	printf("%d\n", !!RAVPOWER_PMIC_DEVICE_STATUS_CHARGING(pmic_status.device_status));
}

int main (int argc, char* argv[])
{
	const char *device = DEFAULT_I2C_PATH;
	char *command = NULL;
	int i2c_fd;
	char buf;

	if (argc < 2) {
		cmd_help(argv[0]);
		return 1;
	}

	command = argv[1];

	i2c_fd = i2c_open(device);
	if (i2c_fd == -1)
		return 1;

	i2c_set_address(i2c_fd, RAVPOWER_PMIC_I2C_ADDRESS);

	if (!strcmp(command, "poweroff")) {
		cmd_poweroff(i2c_fd);
	} else if (!strcmp(command, "battery-charge")) {
		cmd_battery_charge(i2c_fd);
	} else if (!strcmp(command, "battery-charging")) {
		cmd_battery_charging(i2c_fd);
	} else if(!strcmp(command, "dump")) {
		cmd_dump(i2c_fd);
	} else {
		cmd_help(argv[0]);
	}

	i2c_close(i2c_fd);
	return 0;
}

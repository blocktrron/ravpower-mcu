// SPDX-License-Identifier: GPL-2.0-or-later

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
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

static int i2c_rw(int i2c_fd, char addr, char *buf)
{
	if (write(i2c_fd, &addr, 1) != 1) {
		perror("i2c_rw");
		return -1;
	}

	if (read(i2c_fd, buf, 1) != 1) {
		perror("i2c_rw");
		return -1;
	}

	return 0;
}

static int read_pmic_status(int i2c_fd, struct ravpower_pmic_status *buf)
{
	i2c_rw(i2c_fd, RAVPOWER_PMIC_DEVICE_STATUS_REG, &buf->device_status);
	i2c_rw(i2c_fd, RAVPOWER_PMIC_BATTERY_VOLUME_REG, (char *)&buf->battery_volume);
}

static int dump_pmic_status(int i2c_fd)
{
	struct ravpower_pmic_status pmic_status;

	read_pmic_status(i2c_fd, &pmic_status);

	printf("Charge:\t\t %d%%\n", pmic_status.battery_volume);
	printf("Charging:\t %s\n", RAVPOWER_PMIC_DEVICE_STATUS_CHARGING(pmic_status.device_status) ? "Yes" : "No");
}

int main (int argc, char* argv[])
{
	const char *device = DEFAULT_I2C_PATH;
	char *command = NULL;
	int i2c_fd;
	char buf;

	if (argc < 2) {
		printf("Usage: %s [command]\n", argv[0]);
		printf("Avbailable commands:\n");
		printf("\tdump\tDump PMIC status");
	}

	i2c_fd = i2c_open(device);
	if (i2c_fd == -1)
		return 1;

	i2c_set_address(i2c_fd, RAVPOWER_PMIC_I2C_ADDRESS);
	dump_pmic_status(i2c_fd);

	i2c_close(i2c_fd);
	return 0;
}
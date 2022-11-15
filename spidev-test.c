#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>

static const char *device = "/dev/spidev0.0";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 500000;

int spidev_fd;

static int spi_transfer(void)
{

	int ret, i;
	uint8_t tx[] = { 0x80, 0xff, 0xff };

	uint8_t rx[3] = { 0xaa, 0xbb, 0xcc };

	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long) tx,
		.rx_buf = (unsigned long) rx,
		.len = 3,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	ret = ioctl(spidev_fd, SPI_IOC_MESSAGE(1), &tr);

	if (ret < 1) {
		fprintf(stderr, "Failed to send SPI message\n");
		return -1;
	}

	for (i = 0; i < 3; i++) {
		fprintf(stdout, "%02x ", rx[i]);
	}
	fprintf(stdout, "\n");

	return 0;
}

int main(int arg, char *argv[])
{
	int ret = 0;

	spidev_fd = open(device, O_RDWR);
	if (spidev_fd < 0) {
		fprintf(stderr, "Failed to open spidev\n");
		return -1;
	}

	ret = ioctl(spidev_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1) {
		fprintf(stderr, "Failed to set bits per word\n");
		return -1;
	}

	ret = ioctl(spidev_fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1) {
		fprintf(stderr, "Failed to get bits per word\n");
		return -1;
	}

	/*
	 * max speed hz
	 */
	ret = ioctl(spidev_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1) {
		fprintf(stderr, "Failed to set set max speed Hz\n");
		return -1;
	}

	ret = ioctl(spidev_fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1) {
		fprintf(stderr, "Failed to get max speed hz");
		return -1;
	}

	fprintf(stdout, "Bits per word: %d\n", bits);
	fprintf(stdout, "max speed: %d Hz (%d KHz)\n", speed, speed/1000);

	return spi_transfer();
}

#include "PicoBorgRev.hpp"
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <inttypes.h>

///// Storage /////
uint8_t bufIn[BUFFER_MAX];
uint8_t bufOut[BUFFER_MAX];
char pathI2C[20];
int hI2C = HANDLE_UNINITALISED;

///// Public values /////
int busNumber = 1;

uint8_t pbrAddress;
bool pbrFound = false;

int PicoBorgRevInitialise(bool trOtherBus)
{
	int i;
	printf("Loading PicoBorgReverse on bus %d\n", busNumber);
	// Clean buffers
	for (i = 0; i < BUFFER_MAX; ++i) {
		bufIn[i] = 0xCC;
	        bufOut[i] = 0xCC;
	}

	// Open I2C device
	snprintf(pathI2C, sizeof(pathI2C)-1, "/dev/i2c-%d", busNumber);
	hI2C = open(pathI2C, O_RDWR);
	if (hI2C < 0) {
		printf("I2C ERROR: Failed to open bus %d, ", busNumber);
		if (trOtherBus) {
			// 1 or 0
			busNumber = 1 - busNumber;
			printf("Trying bus %d \n", busNumber);
			PicoBorgRevInitialise(false);
		} else {
			printf("are we root?\n");
			return I2C_ERROR_FAILED;
		}
	}
}

int main(int argc, char **argv)
{
	PicoBorgRevInitialise(true);
}

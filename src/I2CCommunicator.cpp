#include "I2CCommunicator.hpp"

//#include <cstdio>
#include <errno.h>
#include <fcntl.h>
#include "I2CException.hpp"
#include <inttypes.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

I2CCommunicator::I2CCommunicator(int busNumber, bool tryOtherBus)
{
	this->busNumber = busNumber;

	init(tryOtherBus);
}

int I2CCommunicator::init(bool tryOtherBus)
{
	printf("Open bus %d\n", busNumber);

	// Open I2C device
	char pathI2C[20];
	snprintf(pathI2C, sizeof(pathI2C)-1, "/dev/i2c-%d", busNumber);
	hI2C = open(pathI2C, O_RDWR);
	if (hI2C < 0) {
		printf("I2C ERROR: Failed to open bus %d, ", busNumber);
		if (tryOtherBus) {
			// 1 or 0
			busNumber = 1 - busNumber;
			printf("Trying bus %d \n", busNumber);
			init(false);
		} else {
			printf("are we root?\n");
			throw I2CException("Failed to open a I2C bus, are we root?");
		}
	}
	return 0;
}

int I2CCommunicator::setTarget(uint8_t targetAddress)
{
	 if (ioctl(hI2C, I2C_SLAVE, targetAddress) < 0) {
		char message[50];
		snprintf(message, sizeof(message) - 1, "Failed to set the target address to 0x%02x", targetAddress);
		throw I2CException(message);
	}
	return 0;
}

int I2CCommunicator::send(uint8_t *pData, int bytes)
{
	int rc = write(hI2C, pData, bytes);
	if (rc == bytes) {
		return I2C_ERROR_OK;
	} else if (rc < 0) {
		char message[50];
		snprintf(message, sizeof(message) - 1, "Failed to send %d bytes!\n", bytes);
		throw I2CException(message);
	} else {
		char message[50];
		snprintf(message, sizeof(message) - 1, "Only sent %d of %d bytes!", rc, bytes);
		throw I2CException(message);
	}
}

int I2CCommunicator::rec(uint8_t *pData, int bytes) {
	int rc = read(hI2C, pData, bytes);
	if (rc == bytes) {
		return I2C_ERROR_OK;
	} else if (rc < 0) {
		char message[50];
		snprintf(message, sizeof(message) - 1, "Failed to read %d bytes!", bytes);
		throw I2CException(message);
	} else {
		char message[50];
		snprintf(message, sizeof(message) - 1, "Only read %d of %d bytes!", rc, bytes);
		throw I2CException(message);
	}
}

int I2CCommunicator::getBusnumber()
{
	return busNumber;
}


#include "PicoBorgRev.hpp"
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <inttypes.h>


#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "I2CCommunicator.hpp"


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
	return PbrScanForAddress(0);
}

uint8_t PbrScanForAddress(uint8_t index)
{
	uint8_t found = 0;
	uint8_t oldAddress = pbrAddress;
	for (pbrAddress = PBR_MINIMUM_I2C_ADDRESS; pbrAddress <= PBR_MAXIMUM_I2C_ADDRESS; pbrAddress++) {
		if (PbrCheckId()) {
			if (index <= 0) {
				found = pbrAddress;
				break;
			} else {
				index--;
			}
		}
	}
	pbrAddress = oldAddress;
	return found;

}

bool PbrCheckId(void)
{
	SetTargetI2C(hI2C, pbrAddress);
	bufOut[0] = PBR_COMMAND_GET_ID;
	if (SendI2C(hI2C, 1, bufOut) == I2C_ERROR_OK) {
		printf("Send to address 0x%02x\n", pbrAddress);
	       if (RecI2C(hI2C, 2, bufIn) == I2C_ERROR_OK) {
		printf("Recieved message: 0x%02x\n", bufIn[1]);
		       return (bufIn[1] == PBR_I2C_ID_PICOBORG_REV);
	      }
	}
	printf("ERROR\n");
	return false;

}

int SetTargetI2C(int hI2C, uint8_t targetAddress)
{
	 if (ioctl(hI2C, I2C_SLAVE, targetAddress) < 0) {
		printf("I2C ERROR: Failed to set target address to %d!\n", targetAddress);
		return 1;
	}
	return 0;
}

int SendI2C(int hI2C, int bytes, uint8_t *pData)
{
	int rc = write(hI2C, pData, bytes);
	if (rc == bytes) {
		return I2C_ERROR_OK;
	} else if (rc < 0) {
		printf("I2C ERROR: Failed to send %d bytes!\n", bytes);
    printf("Error: %d", errno);
        printf("\n\n");
		return I2C_ERROR_FAILED;
	} else {
		printf("I2C ERROR: Only sent %d of %d bytes!\n", rc, bytes);
		return I2C_ERROR_PARTIAL;
	}
}

int RecI2C(int hI2C, int bytes, uint8_t *pData) {
	int rc = read(hI2C, pData, bytes);
	if (rc == bytes) {
		return I2C_ERROR_OK;
	} else if (rc < 0) {
		printf("I2C ERROR: Failed to read %d bytes!\n", bytes);
		return I2C_ERROR_FAILED;
	} else {
		printf("I2C ERROR: Only read %d of %d bytes!\n", rc, bytes);
		return I2C_ERROR_PARTIAL;
	}
}


int main(int argc, char **argv)
{
	//printf("PicoBorgRev found on: 0x%02x \n", PicoBorgRevInitialise(true));
	I2CCommunicator * com = new I2CCommunicator(); 
	delete com;
}

#include "PicoBorgRev.hpp"
#include <errno.h>
#include <fcntl.h>
#include "I2CCommunicator.hpp"
#include <inttypes.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include "PicoBorgRevException.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>





PicoBorgRev::PicoBorgRev(I2CCommunicator *communicator, uint8_t pbrAddress)
{
	if (PicoBorgRev::checkId(communicator, pbrAddress)) {
		printf("PBR board found\n");
		this->communicator = communicator;
		this->pbrAddress = pbrAddress;
		
		//clear buffers
		for (int i = 0; i < BUFFER_MAX; i++) {
			bufIn[i] = 0x0;
			bufOut[i] = 0x0;
		}

	} else {
		char message[70];
		snprintf(message, sizeof(message) - 1, "Board on address %d on bus %d is not a pbr!", pbrAddress, communicator->getBusnumber());
		throw PicoBorgRevException(message);
	}
}


uint8_t PicoBorgRev::scanForAddress(uint8_t index, I2CCommunicator *communicator)
{
	uint8_t found = 0;
	uint8_t pbrAddress;
	for (pbrAddress = PBR_MINIMUM_I2C_ADDRESS; pbrAddress <= PBR_MAXIMUM_I2C_ADDRESS; pbrAddress++) {
		if (PicoBorgRev::checkId(communicator, pbrAddress)) {
			if (index <= 0) {
				found = pbrAddress;
				break;
			} else {
				index--;
			}
		}
	}
	return found;
}

bool PicoBorgRev::checkId(I2CCommunicator *communicator, uint8_t pbrAddress)
{
	communicator->setTarget(pbrAddress);
	uint8_t bufOut[BUFFER_MAX];
	uint8_t bufIn[BUFFER_MAX];
	bufOut[0] = PBR_COMMAND_GET_ID;
	if (communicator->send(bufOut, 1)  == I2C_ERROR_OK) {
		printf("Send to address 0x%02x\n", pbrAddress);
		if (communicator->rec(bufIn, 2) == I2C_ERROR_OK) {
			printf("Recieved message: 0x%02x\n", bufIn[1]);
			return (bufIn[1] == PBR_I2C_ID_PICOBORG_REV);
	      }
	}
	printf("Board not a pbr\n");
	return false;

}

int main(int argc, char **argv)
{
	//printf("PicoBorgRev found on: 0x%02x \n", PicoBorgRevInitialise(true));
	I2CCommunicator * com = new I2CCommunicator(); 
	uint8_t address = PicoBorgRev::scanForAddress(1, com);
	PicoBorgRev *pbr = new PicoBorgRev(com, address);
	delete pbr;
	delete com;
}

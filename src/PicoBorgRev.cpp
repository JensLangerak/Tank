#include "PicoBorgRev.hpp"
#include <errno.h>
#include <fcntl.h>
#include "I2CCommunicator.hpp"
#include "I2CException.hpp"
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
		printf("PBR board found on 0x%02x\n", pbrAddress);
		this->communicator = communicator;
		this->pbrAddress = pbrAddress;
		
		//clear buffers
		for (int i = 0; i < BUFFER_MAX; i++) {
			bufIn[i] = 0x0;
			bufOut[i] = 0x0;
		}

	} else {
		char message[70];
		snprintf(message, sizeof(message) - 1, "Board on address 0x%02x on bus %d is not a pbr!", pbrAddress, communicator->getBusnumber());
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
	try {
		if (communicator->send(bufOut, 1)  == I2C_ERROR_OK) {
			printf("Send to address 0x%02x\n", pbrAddress);
			if (communicator->rec(bufIn, 2) == I2C_ERROR_OK) {
				printf("Recieved message: 0x%02x\n", bufIn[1]);
				return (bufIn[1] == PBR_I2C_ID_PICOBORG_REV);
		      }
		}
	} catch (const I2CException& e) {	

	}
	printf("No pbr on 0x%02x\n", pbrAddress);
	return false;

}

void PicoBorgRev::setMotor1(uint8_t power)
{
	if (power < 0) {
		power = power * -1;
		this->bufOut[0] = PBR_COMMAND_SET_A_REV;
	} else {
		this->bufOut[0] = PBR_COMMAND_SET_A_FWD;
	}

	communicator->setTarget(pbrAddress);
	communicator->send(this->bufOut, 1);
	power = (power > PBR_PWM_MAX) ? PBR_PWM_MAX : power;
	this->bufOut[0] = (uint8_t) power;
	communicator->send(this->bufOut, 1);
}

void PicoBorgRev::setMotor2(uint8_t power)
{
	if (power < 0) {
		power = power * -1;
		this->bufOut[0] = PBR_COMMAND_SET_B_REV;
	} else {
		this->bufOut[0] = PBR_COMMAND_SET_B_FWD;
	}

	communicator->setTarget(pbrAddress);
	communicator->send(this->bufOut, 1);
	power = (power > PBR_PWM_MAX) ? PBR_PWM_MAX : power;
	this->bufOut[0] = (uint8_t) power;
	communicator->send(this->bufOut, 1);
}

uint8_t PicoBorgRev::getMotor(uint8_t command)
{
	this->bufOut[0] = command;
	communicator->setTarget(pbrAddress);
	communicator->send(this->bufOut, 1);
	communicator->rec(this->bufIn, 3);
	int res = bufIn[2];

	if (bufIn[1] == PBR_COMMAND_VALUE_FWD) {
		return res;
	} else if(bufIn[1] == PBR_COMMAND_VALUE_REV) {
		return res * -1;
	} else {
		char message[70];
		if (command == PBR_COMMAND_GET_A) {
			snprintf(message, sizeof(message) - 1, "Failed to read the status of motor 1");
		} else if (command == PBR_COMMAND_GET_B) {
			snprintf(message, sizeof(message) - 1, "Failed to read the status of motor 2");
		} else {
			snprintf(message, sizeof(message) - 1, "Send wrong command");
		}


		throw PicoBorgRevException(message);
	}
}

uint8_t PicoBorgRev::getMotor1()
{
	return getMotor(PBR_COMMAND_GET_A);
}

uint8_t PicoBorgRev::getMotor2()
{
	return getMotor(PBR_COMMAND_GET_B);
}


int main(int argc, char **argv)
{
	//printf("PicoBorgRev found on: 0x%02x \n", PicoBorgRevInitialise(true));
	I2CCommunicator * com = new I2CCommunicator(); 
	uint8_t address = PicoBorgRev::scanForAddress(0, com);
	printf("Found 0x%02x", address);
	PicoBorgRev *pbr = new PicoBorgRev(com, address);
	pbr->setMotor1(100);
	printf("Set motor 1");
	sleep(10);
	printf("Motor 1 running at %i", pbr->getMotor1());
	pbr->setMotor2(100);
	printf("Set motor 2");
	sleep(10);
	printf("Motor 2 running at %i", pbr->getMotor2());
	pbr->setMotor1(0);
	pbr->setMotor2(0);
	delete pbr;
	delete com;
}

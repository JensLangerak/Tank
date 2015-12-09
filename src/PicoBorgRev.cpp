#include "PicoBorgRev.hpp"
#include <math.h>
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

void PicoBorgRev::send(uint8_t size)
{
	communicator->setTarget(pbrAddress);
	communicator->send(this->bufOut, size);
}

void PicoBorgRev::rec(uint8_t send, uint8_t rec)
{
	communicator->setTarget(pbrAddress);
	communicator->send(this->bufOut, send);
	communicator->rec(this->bufIn, rec);
}

void PicoBorgRev::setMotor(uint8_t command, float power)
{
	power *= PBR_PWM_MAX;

	power = (power > PBR_PWM_MAX) ? PBR_PWM_MAX : power;
	this->bufOut[0] = command;
	this->bufOut[1] = (uint8_t) round(power);
	this->send(2);
}

void PicoBorgRev::setMotor1(float power)
{
	if (power < 0) {
		power = power * -1;
		setMotor(PBR_COMMAND_SET_A_REV, power);
	} else {
		setMotor(PBR_COMMAND_SET_A_FWD,power);
	}
}

void PicoBorgRev::setMotor2(float power)
{
	if (power < 0) {
		power = power * -1;
		setMotor(PBR_COMMAND_SET_B_REV, power);
	} else {
		setMotor(PBR_COMMAND_SET_B_FWD,power);
	}
}

float PicoBorgRev::getMotor(uint8_t command)
{
	this->bufOut[0] = command;
	this->rec(1, 3);
	float res = (float) bufIn[2] / (float) PBR_PWM_MAX;
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

float PicoBorgRev::getMotor1()
{
	return getMotor(PBR_COMMAND_GET_A);
}

float PicoBorgRev::getMotor2()
{
	return getMotor(PBR_COMMAND_GET_B);
}

void PicoBorgRev::setMotors(float power)
{
	if (power < 0) {
		power = power * -1;
		setMotor(PBR_COMMAND_SET_ALL_REV, power);
	} else {
		setMotor(PBR_COMMAND_SET_ALL_FWD,power);
	}
}

void PicoBorgRev::motorsOff(void)
{	
	this->bufOut[0] = PBR_COMMAND_ALL_OFF;
	this->send(1);
}


int main(int argc, char **argv)
{
	//printf("PicoBorgRev found on: 0x%02x \n", PicoBorgRevInitialise(true));
	I2CCommunicator * com = new I2CCommunicator(); 
	uint8_t address = PicoBorgRev::scanForAddress(0, com);
	printf("Found 0x%02x\n", address);
	PicoBorgRev *pbr = new PicoBorgRev(com, address);
	pbr->setMotor1(-0.5);
	sleep(3);
	pbr->setMotor2(.5);
	sleep(3);
	printf("motor 1: %f \n", pbr->getMotor1());
	printf("motor 2: %f \n", pbr->getMotor2());
	pbr->setMotors(1);
	sleep(3);
	printf("motor 1: %f \n", pbr->getMotor1());
	printf("motor 2: %f \n", pbr->getMotor1());
	pbr->motorsOff();
	printf("motor 1: %f \n", pbr->getMotor1());
	printf("motor 2: %f \n", pbr->getMotor2());
	delete pbr;
	delete com;
}

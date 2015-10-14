#ifndef I2CCOMMUNICATOR_HPP
#define I2CCOMMUNICATOR_HPP

#include <stdint.h>
#include <unistd.h>

#define BUFFER_MAX		(32)

#define I2C_ERROR_OK		(0)
#define I2C_ERROR_FAILED	(1)
#define I2C_ERROR_PARTIAL	(2)
#define HANDLE_UNINITALISED	(-1)

class I2CCommunicator {
	protected:
		int hI2C = HANDLE_UNINITALISED;

		int busNumber;

		/**
		 * Open a i2c bus and clear the buffers
		 *
		 * @parem tryOtherBus true if other bus must be tried when init fails to open the first bus
		 *
		 * @return 0 if succeded
		 * @throws I2CException thrown when failed to initialize the i2c object.
		 */
		int init(bool tryOtherBus);
	public:
		/* Construct a object that is able to communicate with I2C.
		 * @param busNumber number of the bus that must be connected
		 * @param tryOtherBus true if the other bus must be tried when this bus fails.
		 * @throws I2CException when it fails to create this object.
		 */
		I2CCommunicator(int busNumber=1, bool tryOtherBus = true);

		/**
		 * Return the selected busnumber.
		 */
		int getBusnumber(void);

		/**
		 * Set a target to communicate with.
		 *
		 * @param targetAddress address of the targer
		 *
		 * @return 0 if succeded.
		 * @throws I2CException when it fails to set the target.
		 */
		int setTarget(uint8_t targetAddress);	
		
		/**
		 * Send a message to the selected target (set the target with setTarget).
		 *
		 * @param pData a pointer to the data that must be send
		 * @param bytes number of bytes to send
		 *
		 * @return 0 if succeded.
		 * @throws I2CException when it fails to send the data.
		 */
		int send(uint8_t *pData, int bytes);

		/**
		 * Read a recieved message.
		 *
		 * @param pData a pointer to the location where the data must be stored.
		 * @param bytes number of bytes to read.
		 *
		 * @return 0 if succeded.
		 * @throws I2CException when it fails to recieve the data.
		 */
		int rec(uint8_t *pData, int bytes);
};
#endif /* I2CCOMUNICATOR_HPP */

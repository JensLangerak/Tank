#ifndef PICOBORGREV_HPP
#define PICOBORGREV_HPP
#include <stdint.h>
#include <unistd.h>
#include "I2CCommunicator.hpp"

/**
 * Based on the arduino file for PicoBorgRev:
 * https://www.piborg.org/downloads/picoborgrev/PicoBorgRevArduino.zip
 */

// Commands
// GET commands sent should be followed by a read for the result
// All other commands are send only (no reply)
#define PBR_COMMAND_SET_LED         (1)     // Set the LED status
#define PBR_COMMAND_GET_LED         (2)     // Get the LED status
#define PBR_COMMAND_SET_A_FWD       (3)     // Set motor 2 PWM rate in a forwards direction
#define PBR_COMMAND_SET_A_REV       (4)     // Set motor 2 PWM rate in a reverse direction
#define PBR_COMMAND_GET_A           (5)     // Get motor 2 direction and PWM rate
#define PBR_COMMAND_SET_B_FWD       (6)     // Set motor 1 PWM rate in a forwards direction
#define PBR_COMMAND_SET_B_REV       (7)     // Set motor 1 PWM rate in a reverse direction
#define PBR_COMMAND_GET_B           (8)     // Get motor 1 direction and PWM rate
#define PBR_COMMAND_ALL_OFF         (9)     // Switch everything off
#define PBR_COMMAND_RESET_EPO       (10)    // Resets the EPO flag, use after EPO has been tripped and switch is now clear
#define PBR_COMMAND_GET_EPO         (11)    // Get the EPO latched flag
#define PBR_COMMAND_SET_EPO_IGNORE  (12)    // Set the EPO ignored flag, allows the system to run without an EPO
#define PBR_COMMAND_GET_EPO_IGNORE  (13)    // Get the EPO ignored flag
#define PBR_COMMAND_GET_DRIVE_FAULT (14)    // Get the drive fault flag, indicates faults such as short-circuits and under voltage
#define PBR_COMMAND_SET_ALL_FWD     (15)    // Set all motors PWM rate in a forwards direction
#define PBR_COMMAND_SET_ALL_REV     (16)    // Set all motors PWM rate in a reverse direction
#define PBR_COMMAND_SET_FAILSAFE    (17)    // Set the failsafe flag, turns the motors off if communication is interrupted
#define PBR_COMMAND_GET_FAILSAFE    (18)    // Get the failsafe flag
#define PBR_COMMAND_SET_ENC_MODE    (19)    // Set the board into encoder or speed mode
#define PBR_COMMAND_GET_ENC_MODE    (20)    // Get the boards current mode, encoder or speed
#define PBR_COMMAND_MOVE_A_FWD      (21)    // Move motor 2 forward by n encoder ticks
#define PBR_COMMAND_MOVE_A_REV      (22)    // Move motor 2 reverse by n encoder ticks
#define PBR_COMMAND_MOVE_B_FWD      (23)    // Move motor 1 forward by n encoder ticks
#define PBR_COMMAND_MOVE_B_REV      (24)    // Move motor 1 reverse by n encoder ticks
#define PBR_COMMAND_MOVE_ALL_FWD    (25)    // Move all motors forward by n encoder ticks
#define PBR_COMMAND_MOVE_ALL_REV    (26)    // Move all motors reverse by n encoder ticks
#define PBR_COMMAND_GET_ENC_MOVING  (27)    // Get the status of encoders moving
#define PBR_COMMAND_SET_ENC_SPEED   (28)    // Set the maximum PWM rate in encoder mode
#define PBR_COMMAND_GET_ENC_SPEED   (29)    // Get the maximum PWM rate in encoder mode

#define PBR_COMMAND_GET_ID          (0x99)  // Get the board identifier
#define PBR_COMMAND_SET_I2C_ADD     (0xAA)  // Set a new I2C address

// Values
// These are the corresponding numbers for states used by the above commands
#define PBR_COMMAND_VALUE_FWD       (1)     // I2C value representing forward
#define PBR_COMMAND_VALUE_REV       (2)     // I2C value representing reverse
#define PBR_COMMAND_VALUE_ON        (1)     // I2C value representing on
#define PBR_COMMAND_VALUE_OFF       (0)     // I2C value representing off
#define PBR_I2C_ID_PICOBORG_REV     (0x15)  // I2C values returned when calling the GET_ID command
#define PBR_DEFAULT_I2C_ADDRESS     (0x44)  // I2C address set by default (before using SET_I2C_ADD)
#define PBR_ERROR_READING           (888)   // Returned from GetMotor commands when value failed to read

// Limits
// These define the maximums that the PicoBorg Reverse will accept
#define PBR_I2C_MAX_LEN             (4)     // Maximum number of bytes in an I2C message
#define PBR_PWM_MAX                 (255)   // Maximum I2C value for speed settings (represents 100% drive)
#define PBR_MINIMUM_I2C_ADDRESS     (0x03)  // Minimum allowed value for the I2C address
#define PBR_MAXIMUM_I2C_ADDRESS     (0x77)  // Maximum allowed value for the I2C address


class PicoBorgRev {
	protected:
		I2CCommunicator *communicator;
		uint8_t pbrAddress;
		uint8_t bufIn[BUFFER_MAX];
		uint8_t bufOut[BUFFER_MAX];

		/**
		 * Get the motor speed
		 *
		 * @param command command for the speed of motor 1 or 2
		 */
		int16_t getMotor(uint8_t command);

	public:
		PicoBorgRev(I2CCommunicator *communicator, uint8_t pbrAddress);

		/**************************************/
		/***** PicoBorg Reverse Functions *****/
		/**************************************/
		/**
		 * All motor drive levels are from +PBR_PWM_MAX to -PBR_PWM_MAX
		 * Positive values indicate forwards motion
		 * Negative values indicate reverse motion
		 * 0 indicates stationary
		 * Values outside PBR_PWM_MAX will be capped to PBR_PWM_MAX (100%)
		 */


		/***** Motor functions *****/

		// Sets the drive level for motor 2
		void setMotor2(uint8_t power);

		// Gets the drive level for motor 2
		int16_t getMotor2(void);

		// Sets the drive level for motor 1
		void setMotor1(uint8_t power);

		// Gets the drive level for motor 1
		int16_t getMotor1(void);

		// Sets the drive level for all motors
		void PbrSetMotors(uint8_t power);

		// Sets all motors to stopped, useful when ending a program
		void PbrMotorsOff(void);

		/***** General functions *****/

		// Reads the board identifier and checks it is a PicoBorg Reverse, false for incorrect, true for correct
		static bool checkId(I2CCommunicator *communicator, uint8_t pbrAddress);

		// Sets the current state of the LED, false for off, true for on
		void PbrSetLed(bool state);

		// Reads the current state of the LED, false for off, true for on
		bool PbrGetLed(void);


		// Resets the EPO latch state, use to allow movement again after the EPO has been tripped
		void PbrResetEpo(void);

		// Reads the system EPO latch state.
		// If false the EPO has not been tripped, and movement is allowed.
		// If true the EPO has been tripped, movement is disabled if the EPO is not ignored (see PbrSetEpoIgnore)
		//     Movement can be re-enabled by calling PbrResetEpo. 
		bool PbrGetEpo(void);

		// Sets the system to ignore or use the EPO latch, set to false if you have an EPO switch, true if you do not
		void PbrSetEpoIgnore(bool state);

		// Reads the system EPO ignore state, False for using the EPO latch, True for ignoring the EPO latch
		bool PbrGetEpoIgnore(void);

		// Sets the system to enable or disable the communications failsafe
		// The failsafe will turn the motors off unless it is commanded at least once every 1/4 of a second
		// Set to True to enable this failsafe, set to False to disable this failsafe
		// The failsafe is disabled at power on
		void PbrSetCommsFailsafe(bool state);

		// Read the current system state of the communications failsafe, true for enabled, false for disabled
		// The failsafe will turn the motors off unless it is commanded at least once every 1/4 of a second
		bool PbrGetCommsFailsafe(void);

		// Reads the system drive fault state, False for no problems, True for a fault has been detected
		// Faults may indicate power problems, such as under-voltage (not enough power), and may be cleared by setting a lower drive power
		// If a fault is persistent, it repeatably occurs when trying to control the board, this may indicate a wiring problem such as:
		//     * The supply is not powerful enough for the motors
		//         The board has a bare minimum requirement of 6V to operate correctly
		//         A recommended minimum supply of 7.2V should be sufficient for smaller motors
		//     * The + and - connections for either motor are connected to each other
		//     * Either + or - is connected to ground (GND, also known as 0V or earth)
		//     * Either + or - is connected to the power supply (V+, directly to the battery or power pack)
		//     * One of the motors may be damaged
		// Faults will self-clear, they do not need to be reset, however some faults require both motors to be moving at less than 100% to clear
		// The easiest way to check is to put both motors at a low power setting which is high enough for them to rotate easily, such as 30%
		// Note that the fault state may be true at power up, this is normal and should clear when both motors have been driven
		// If there are no faults but you cannot make your motors move check PbrGetEpo to see if the safety switch has been tripped
		// For more details check the website at www.piborg.org/picoborgrev and double check the wiring instructions
		bool PbrGetDriveFault(void);

		/***** Advanced functions *****/

		// Scans the I2C bus for PicoBorg Reverse boards and returns a count of all the boards found
		static uint8_t PbrScanForCount(void);

		// Scans the I2C bus for a PicoBorg Reverse board, index is which address to return (from 0 to count - 1)
		// Returns address 0 if no board is found for that index
		static uint8_t scanForAddress(uint8_t index, I2CCommunicator *communicator);

		// Sets the PicoBorg Reverse at the current address to newAddress
		// Warning, this new I²C address will still be used after resetting the power on the device
		// If successful returns true and updates pbrAddress, otherwise returns false
		bool PbrSetNewAddress(uint8_t newAddress);

};

#endif /* PICOBORGREV_HPP */

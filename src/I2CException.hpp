#ifndef I2CEXCEPTION_HPP
#define I2CEXCEPTION_HPP

#include <iostream>
#include <exception>
#include <string>

class I2CException : public std::exception
{;
	private:
		std::string errMsg;
	public:
		I2CException(const char *msg) : errMsg(msg) {};
		I2CException(void) :
			I2CException("Error") {};
		~I2CException(void) throw() {};
		const char *what() const throw() { 
			std::string preMessage = "I2C ERROR: ";
			preMessage.append(errMsg);
			return preMessage.c_str();
		};
};

#endif /* I2CEXCEPTION_HPP */

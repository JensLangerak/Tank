#ifndef PICOBORGREVEXCEPTION_HPP
#define PICOBORGREVEXCEPTION_HPP

#include <iostream>
#include <exception>
#include <string>

class PicoBorgRevException : public std::exception
{
	private:
		std::string errMsg;
	public:
		PicoBorgRevException(const char *msg) : errMsg(msg) {};
		PicoBorgRevException(void) :
			PicoBorgRevException("Error") {};
		~PicoBorgRevException(void) throw() {};
		const char *what() const throw() {
			std::string preMessage = "PBR ERROR: ";
			preMessage.append(errMsg);
			return preMessage.c_str();
		};
};

#endif /* PICOBORGREVEXCEPTION_HPP */

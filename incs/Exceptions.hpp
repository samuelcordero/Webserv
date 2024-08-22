#pragma once

#include <exception>
#include <cstring>

//class that contains exceptions used in the webserv to control parsing errors for config etc

class ParseErrorException : public std::exception {
private:
	char message[256];

public:
	// Constructor que toma un mensaje personalizado
	explicit ParseErrorException(const char* message) {
		strncpy(this->message, message, sizeof(this->message) - 1);
		this->message[sizeof(this->message) - 1] = '\0'; // Asegura la terminación nula
	}

	// Sobrescribir el método what() para devolver el mensaje de error
	virtual const char* what() const throw() {
		return message;
	}
};

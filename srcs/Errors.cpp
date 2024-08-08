#include "Errors.hpp"

Errors::Errors() {}

Errors::Errors(std::map<std::string, std::string> errorMap) {
	for (std::map<std::string, std::string>::iterator it = errorMap.begin(); it != errorMap.end(); ++it) {
		std::string key = it->first;
		std::string value = it->second;

		int keyAsInt = std::atoi(key.c_str());

		if (access(value.c_str(), R_OK) == 0) {
			customErrors[keyAsInt] = value;
			std::cout << "Custom error " << key << " with file " << value << " added." << std::endl;
		} else {
			std::cerr << "Custom error " << key << " - cannot read file " << value << ", default error will be used." << std::endl;
		}
	}
}

Errors::~Errors() {}

Response Errors::getError(int error_code) {
	std::cerr << "oopsieee " << error_code << " error detected\n";
	if (customErrors.find(error_code) != customErrors.end())
		return customErrorResponse(error_code);
	return ourErrorResponse(error_code);
}

Response Errors::customErrorResponse(int error_code) {
	std::string filePath = customErrors[error_code];
	std::ifstream file(filePath.c_str());

	if (file.is_open()) {
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string fileContent = buffer.str();
		file.close();
	
		return Response(error_code, fileContent, true);
	} else { // defaulting if error opening file
		return ourErrorResponse(error_code);
	}
}

Response Errors::ourErrorResponse(int error_code) {
	int final_code = error_code;
	std::string message;
	switch (error_code)
    {
    case 200:
        message = "OK";
        break;
    case 201:
        message = "Created";
        break;
    case 204:
        message = "No Content";
        break;
    case 400:
        message = "Bad Request";
        break;
    case 401:
        message = "Unauthorized";
        break;
    case 403:
        message = "Forbidden";
        break;
    case 404:
        message = "Not Found";
        break;
    case 405:
        message = "Method Not Allowed";
        break;
    case 406:
        message = "Not Acceptable";
        break;
    case 408:
        message = "Request Timeout";
        break;
    case 409:
        message = "Conflict";
        break;
    default:
        final_code = 500;
        message = "Internal Server Error";
        break;
    }
	return Response(final_code, message, true);
}

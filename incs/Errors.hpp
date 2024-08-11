#pragma once

#include <fstream>
#include <map>
#include <cstdlib>
#include <unistd.h>

#include "Response.hpp"

class Errors {
	private:
		std::map<int, std::string>	customErrors;

		Response customErrorResponse(int error_code);
		Response ourErrorResponse(int error_code);
	public:
		Errors();
		Errors(std::map<std::string, std::string>);
		~Errors();
		Response getError(int error_code);
};
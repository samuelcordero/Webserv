#pragma once
#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

class Response {
	private:
		std::string message;

		std::string _get_current_date();
	public:
		Response();
		Response(int status_code, const std::string& status_message, const std::string& body, bool include_body);
		Response(const Response &other);
		~Response();

		const std::string &getMessage() const;

		Response &operator=(const Response &other);
};

std::ostream &operator<<(std::ostream &out, const Response &resp);
std::string _int_to_string(int value);
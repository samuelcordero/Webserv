#pragma once
#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

class Response
{
private:
	std::string message;
	int status_code;
	std::string status_message;
	std::string body;

	std::string _get_current_date();

public:
	Response();
	Response(int status_code, const std::string &body, bool include_body);
	Response(const Response &other);
	~Response();

	const std::string &getMessage() const;
	const std::string &getBody() const;
	const std::string &getStatusMessage() const;
	const int &getStatusCode() const;

	void setMessage(const std::string &message);
	void setBody(const std::string &body);
	void setStatusMessage(const std::string &status_message);
	void setStatusCode(const int &status_code);

	Response &operator=(const Response &other);
};

std::ostream &operator<<(std::ostream &out, const Response &resp);
std::string _int_to_string(int value);
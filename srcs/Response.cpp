#include "Response.hpp"

std::string Response::_get_current_date() {
    std::time_t now = std::time(NULL);
    std::tm *tm = std::gmtime(&now);

    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", tm);

    return std::string(buffer);
}

std::string _int_to_string(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

Response::Response() {
	std::string body;
	body = "Hello world! This is test message!";
	message += "HTTP/1.1 " + _int_to_string(200) + " " + "OK" + "\r\n";
    message += "Date: " + _get_current_date() + "\r\n";
    message += "Server: CustomC++Server/1.0\r\n";
    message += "Content-Type: text/html; charset=UTF-8\r\n";
    message += "Content-Length: " + _int_to_string(body.length()) + "\r\n";
    message += "Connection: keep-alive\r\n";
    message += "\r\n";
    message += body;
}

Response::Response(int status_code, const std::string& status_message, const std::string& body, bool include_body) {
	message += "HTTP/1.1 " + _int_to_string(status_code) + " " + status_message + "\r\n";
    message += "Date: " + _get_current_date() + "\r\n";
    message += "Server: CustomC++Server/1.0\r\n";
    message += "Content-Type: text/html; charset=UTF-8\r\n";
    message += "Content-Length: " + _int_to_string(body.length()) + "\r\n";
    message += "Connection: keep-alive\r\n";
    message += "\r\n";
	if (include_body)
    	message += body;
}

Response::Response(const Response &other) {
	*this = other;
}

Response::~Response() {}

const std::string &Response::getMessage() const {
	return message;
}

Response &Response::operator=(const Response &other) {
	message = other.message;
	return (*this);
}

std::ostream &operator<<(std::ostream &out, const Response &resp) {
	out << "---------RESPONSE: ---------\n";
	out << resp.getMessage();
	out << "\n---------RESPONSE END---------\n";
	return out;
}

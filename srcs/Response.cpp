#include "Response.hpp"

std::string Response::get_current_date()
{
    std::time_t now = std::time(NULL);
    std::tm *tm = std::gmtime(&now);

    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", tm);

    return std::string(buffer);
}

std::string _int_to_string(int value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

Response::Response()
{
    std::string body;
    body = "Hello world! This is test message!";
    message += "HTTP/1.1 " + _int_to_string(200) + " " + "OK" + "\r\n";
    message += "Date: " + get_current_date() + "\r\n";
    message += "Server: CustomC++Server/1.0\r\n";
    message += "Content-Type: text/html; charset=UTF-8\r\n";
    message += "Content-Length: " + _int_to_string(body.length()) + "\r\n";
    message += "Connection: keep-alive\r\n";
    message += "\r\n";
    message += body;
}

Response::Response(int status_code, const std::string &body, bool include_body)
{

    this->status_code = status_code;
    // this->status_message = status_message;
    // based on status code, set status message
    switch (status_code)
    {
    case 200:
        this->status_message = "OK";
        break;
    case 201:
        this->status_message = "Created";
        break;
    case 204:
        this->status_message = "No Content";
        break;
    case 400:
        this->status_message = "Bad Request";
        break;
    case 401:
        this->status_message = "Unauthorized";
        break;
    case 403:
        this->status_message = "Forbidden";
        break;
    case 404:
        this->status_message = "Not Found";
        break;
    case 405:
        this->status_message = "Method Not Allowed";
        break;
    case 406:
        this->status_message = "Not Acceptable";
        break;
    case 408:
        this->status_message = "Request Timeout";
        break;
    case 409:
        this->status_message = "Conflict";
        break;
    default:
        this->status_code = 500;
        this->status_message = "Internal Server Error";
        break;
    }
    message += "HTTP/1.1 " + _int_to_string(status_code) + " " + status_message + "\r\n";
    message += "Date: " + get_current_date() + "\r\n";
    message += "Server: CustomC++Server/1.0\r\n";
    message += "Content-Type: text/html; charset=UTF-8\r\n";
    message += "Content-Length: " + _int_to_string(body.length()) + "\r\n";
    message += "Connection: keep-alive\r\n";
    message += "\r\n";
    if (include_body)
        message += body;
}

Response::Response(const Response &other)
{
    *this = other;
}

Response::~Response() {}

const std::string &Response::getMessage() const
{
    return message;
}

const std::string &Response::getBody() const
{
    return body;
}

const std::string &Response::getStatusMessage() const
{
    return status_message;
}

const int &Response::getStatusCode() const
{
    return status_code;
}

void Response::setMessage(const std::string &message)
{
    this->message = message;
}

void Response::setBody(const std::string &body)
{
    this->body = body;
}

void Response::setStatusMessage(const std::string &status_message)
{
    this->status_message = status_message;
}

void Response::setStatusCode(const int &status_code)
{
    this->status_code = status_code;
}

Response &Response::operator=(const Response &other)
{
    message = other.message;
    return (*this);
}

std::ostream &operator<<(std::ostream &out, const Response &resp)
{
    out << "---------RESPONSE: ---------\n";
    out << resp.getMessage();
    out << "\n---------RESPONSE END---------\n";
    return out;
}

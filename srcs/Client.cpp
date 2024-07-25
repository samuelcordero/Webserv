#include "Client.hpp"

Client::Client() {
	last_conn = __LONG_LONG_MAX__;
	request_buffer.clear();
	response.first = NULL;
	response.second = false;
	request.first = NULL;
	request.second = false;
	cgi = NULL;
}

Client::Client(const Client &other) {
	last_conn = other.last_conn;
	request_buffer = other.request_buffer;
	response.first = NULL;
	if (other.response.first)
		response.first = new Response(*other.response.first);
	response.second = other.response.second;
	request.first = NULL;
	if (other.request.first)
		request.first = new Request(*other.request.first);
	request.second = other.request.second;
	cgi = other.cgi;
}

Client::~Client() {
	if (response.first)
		delete response.first;
	if (request.first)
		delete request.first;
}

Client &Client::operator=(const Client &other) {
	if (this != &other) {
		last_conn = other.last_conn;
		request_buffer = other.request_buffer;
		if (response.first)
			delete response.first;
		response.first = NULL;
		if (other.response.first)
			response.first = new Response(*other.response.first);
		response.second = other.response.second;
		if (request.first)
			delete request.first;
		request.first = NULL;
		if (other.request.first)
			request.first = new Request(*other.request.first);
		request.second = other.request.second;
		cgi = other.cgi;
	}
	return (*this);
}

bool Client::responseReady() {
	return response.second;
}

bool Client::requestReady() {
	return request.second;
}

void Client::setLastConn(long long etime) {
	last_conn = etime;
}

const std::string &Client::getResponseMessage() {
	return response.first->getMessage();
}

const Request &Client::getRequest() {
	return *request.first;
}

long long	Client::getLastConn() {
	return last_conn;
}

void Client::clearRequest() {
	if (request.first)
		delete request.first;
	request.first = NULL;
	request.second = false;
}

void Client::clearResponse() {
	if (response.first)
		delete response.first;
	response.first = NULL;
	response.second = false;
}

void Client::addToRequestBuffer(const std::string &b) {
	request_buffer += b;
}

std::string &Client::getRequestBuffer() {
	return request_buffer;
}

void Client::setRequest(const Request &r) {
	request.first = new Request(r);
	request.second = true;
}

void Client::setResponse(const Response &r) {
	response.first = new Response(r);
	response.second = true;
}

void Client::disconnect(int fd) {
	std::cerr << "[ " << Response::get_current_date() << " ] : Closing connection with " << fd << std::endl;
	close(fd);
	request_buffer.clear();
	last_conn = __LONG_LONG_MAX__;
}

void	Client::setCGI(CGIHandler *handler) {
	if (cgi)
		delete cgi;
	cgi = handler;
}

CGIHandler	*Client::getCGI() {
	return cgi;
}

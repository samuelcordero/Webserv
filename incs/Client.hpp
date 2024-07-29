#pragma once
#ifndef __CLIENT_H__
# define __CLIENT_H__

# include "Response.hpp"
# include "Request.hpp"
# include <string>
# include <utility>
# include <unistd.h>
# include "CgiHandler.hpp"

class Response;
class Request;

class Client {
	private:
		std::string 				request_buffer;
		//std::string				response_buffer;
		std::pair<Response *, bool>	response;
		std::pair<Request *, bool>	request;
		long long					last_conn;
		CGIHandler					*cgi;
		long long					cgi_start;
	public:
		Client();
		//Client();
		Client(const Client &other);
		~Client();
		Client				&operator=(const Client &other);
		bool				responseReady();
		bool				requestReady();
		//void				createResponse();
		void				setLastConn(long long etime);
		const std::string	&getResponseMessage();
		const Request		&getRequest();
		long long			getLastConn();
		void				clearResponse();
		void				clearRequest();
		void				disconnect(int fd);
		void				addToRequestBuffer(const std::string &b);
		std::string			&getRequestBuffer();
		void				setRequest(const Request &r);
		void				setResponse(const Response &r);
		void				setCGI(CGIHandler *handler);
		CGIHandler			*getCGI();
		void				setCgiStartTime(long long now);
		long long			getCgiStartTime();
};

#endif
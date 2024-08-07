// A class encapsulating the request header of an HTTP request.
// The request header is the first part of an HTTP request, and contains
// information about the request, such as the method, the URI, and the
// version of the HTTP protocol being used. 
// The request header is followed by the request body, which contains
// the data being sent to the server.

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <cstdlib>


class Request
{
    private:
        std::string method;
        std::string uri;
        std::map<std::string, std::string> headers;
        std::string body;
		size_t		content_len;
    public:
        Request(std::string &);
        Request(const Request&);
        Request& operator=(const Request&);
        ~Request();

        std::string getMethod() const;
        std::string getUri() const;
        std::map<std::string, std::string> getHeaders() const;
        const std::string &getBody() const;
		int			getNumMethod() const;
		size_t		getContentLen() const;
};

std::ostream& operator<<(std::ostream&, const Request&);

#endif
// Implements request.hpp

#include "Request.hpp"
#include "Response.hpp"

// Request constructor that takes a string as an argument
// The string is the raw HTTP request
// The constructor parses the request and stores the method, URI, headers, and body
Request::Request(std::string &request)
{
    size_t pos = 0, end_pos = 0;

    content_len = 0;
    // Parse the request line (e.g., "GET / HTTP/1.1")
    end_pos = request.find("\r\n");
    std::string request_line = request.substr(pos, end_pos - pos);
    pos = end_pos + 2;

    size_t method_end = request_line.find(' ');
    method = request_line.substr(0, method_end);

    size_t uri_end = request_line.find(' ', method_end + 1);
    uri = request_line.substr(method_end + 1, uri_end - method_end - 1);

    // Parse headers
    while ((end_pos = request.find("\r\n", pos)) != std::string::npos)
    {
        std::string line = request.substr(pos, end_pos - pos);
        pos = end_pos + 2;

        if (line.empty())
        {
            break; // End of headers
        }

        size_t delimiter_pos = line.find(": ");
        if (delimiter_pos != std::string::npos)
        {
            std::string key = line.substr(0, delimiter_pos);
            std::string value = line.substr(delimiter_pos + 2);
            headers[key] = value;
        }
    }

    // Parse body
    body = "";
    if (headers.find("Content-Length") != headers.end()) 
    { 
        // Generate body with content-length
        char *end;
        content_len = std::strtol(headers["Content-Length"].c_str(), &end, 10);
        if (*end != '\0') {
            std::cerr << "Invalid Content-Length!\n";
        } else {
            if (request.size() - pos >= content_len) {
                body = request.substr(pos, content_len);
                pos += content_len; // Advance position after body
				request = request.substr(pos);
            } else {
                body = ""; // Incomplete body, handle it accordingly
            }
        }
    }
    else if (headers.find("Transfer-Encoding") != headers.end() && headers["Transfer-Encoding"] == "chunked")
    {
        // Handle chunked transfer encoding
		std::cerr << "parsing chunks!\n";
		// skip the total size of the chunks (already checked)
        end_pos = request.find("\r\n", pos);

        std::string chunk_size_str = request.substr(pos, end_pos - pos);
        pos = end_pos + 2;

    	// Convert the chunk size from hex to a numeric value
        size_t total_chunk_size = std::strtoul(chunk_size_str.c_str(), NULL, 16);
		if (total_chunk_size != 0) {
			while (true)
			{
				// Find the size of the next chunk
				end_pos = request.find("\r\n", pos);
				if (end_pos == std::string::npos)
					break;

				std::string chunk_size_str = request.substr(pos, end_pos - pos);
				pos = end_pos + 2;

				// Convert the chunk size from hex to a numeric value
				size_t chunk_size = std::strtoul(chunk_size_str.c_str(), NULL, 16);
				if (chunk_size == 0) {
					request = request.substr(pos);
					headers["Content-Length"] = _int_to_string(body.size());
					content_len = body.size();
					break; // Last chunk (size 0) indicates the end of the body
				}

				// Extract the chunk data
				if (pos + chunk_size <= request.size())
				{
					body += request.substr(pos, chunk_size);
					pos += chunk_size + 2; // Skip over the chunk and the trailing \r\n
				}
				else
				{
					break; // Incomplete chunk, handle it accordingly
				}
			}
		} else {
			request = request.substr(pos);
			headers["Content-Length"] = _int_to_string(0);
			content_len = body.size();
		}
    }
    else
    {
        // No Content-Length or Transfer-Encoding specified
        // In many cases, this means no body
        body = "";
		request = request.substr(pos);
    }

}


Request::Request(const Request &other)
{
    method = other.method;
    uri = other.uri;
    headers = other.headers;
    body = other.body;
	content_len = other.content_len;
}

Request &Request::operator=(const Request &other)
{
    method = other.method;
    uri = other.uri;
    headers = other.headers;
    body = other.body;
	content_len = other.content_len;
    return *this;
}

Request::~Request()
{
}

std::string Request::getMethod() const
{
    return method;
}

std::string Request::getUri() const
{
    return uri;
}

std::map<std::string, std::string> Request::getHeaders() const
{
    return headers;
}

const std::string &Request::getBody() const
{
    return body;
}

std::ostream &operator<<(std::ostream &os, const Request &r)
{
    os << "Method: " << r.getMethod() << std::endl;
    os << "URI: " << r.getUri() << std::endl;
    os << "Headers:" << std::endl;
    std::map<std::string, std::string> headers = r.getHeaders();
    // print all headers
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }
    os << "Body: " << r.getBody() << std::endl;
    return os;
}

int	Request::getNumMethod() const
{
	if (this->method == "POST")
		return (1);
	if (this->method == "GET" || this->method == "HEAD")
		return (2);
	if (this->method == "DELETE")
		return (4);
	return (8);
}


size_t	Request::getContentLen() const
{
	return (content_len);
}
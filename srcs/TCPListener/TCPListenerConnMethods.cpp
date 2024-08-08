#include "TCPListener.hpp"

//calls the GET, POST or HEAD method requested, and returns the response
Response TCPListener::analizer(const Request &request)
{
	std::vector<Location> &locations = this->server->getLocations();

	std::pair<std::string, std::string> uri_pair = splitUri(request.getUri());

	std::cerr << "Building response for resource " << uri_pair.second << " at location " << uri_pair.first << std::endl;
	for (size_t i = 0; i < locations.size(); i++)
	{
		if (locations[i].getUri() == uri_pair.first)
		{
			if ((locations[i].getMethods() & request.getNumMethod()) == request.getNumMethod())
			{
				if (uri_pair.second == "" && locations[i].hasAutoIndex())
					return (Response(200, locations[i].getAutoIndex(), true));
				else if (uri_pair.second == "")
					uri_pair.second = locations[i].getIndex().front();
				else if (!locations[i].hasAutoIndex())
				{
					bool	flag = false;
					std::vector<std::string>	tmp = locations[i].getIndex();
					for (size_t j = 0; j < tmp.size(); j++)
					{
						if (uri_pair.second == tmp[j])
						{
							flag = true;
							break ;
						}
					}
					if (flag == false)
					{
						return (server->error(404));
					}
				}
				/* std::cerr << "requested method: " << request.getMethod()
					<< " for uri " << uri_pair.first
					<< " resource " << uri_pair.second << std::endl;
				std::cerr << "location method " << locations[i].getMethods() << std::endl; */

				if (request.getNumMethod() == 1)
					return (Post(uri_pair, locations[i], request));
				if (request.getNumMethod() == 2 && request.getMethod() == "GET")
					return (Get(uri_pair, locations[i]));
				if (request.getNumMethod() == 2 && request.getMethod() == "HEAD")
					return (Head(uri_pair, locations[i]));
				if (request.getNumMethod() == 4)
					return (Delete(uri_pair, locations[i]));
			}
			else
			{
				return (server->error(403));
			}
			break;
		}
	}
	return (server->error(404));
}

// GET method handler
Response TCPListener::Get(std::pair<std::string, std::string> uri_pair, Location &location)
{
	std::string file_path = location.getRoot() + "/" + uri_pair.second;
	//std::cerr << "opening file " << file_path << std::endl;
	std::ifstream file(file_path.c_str());

	if (file.is_open())
	{
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string file_contents = buffer.str();
		file.close();
		return Response(200, file_contents, true, file_path);
	}
	else
	{
		return server->error(500);
	}
}

// HEAD method handler
Response TCPListener::Head(std::pair<std::string, std::string> uri_pair, Location &location)
{
	std::string file_path = location.getRoot() + "/" + uri_pair.second;
	//std::cerr << "opening file " << file_path << std::endl;
	std::ifstream file(file_path.c_str());

	if (file.is_open())
	{
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string file_contents = buffer.str();
		file.close();
		return Response(200, file_contents, false);
	}
	else
	{
		return server->error(500);
	}
}

// DELETE method handler
Response TCPListener::Delete(std::pair<std::string, std::string> uri_pair, Location &location)
{
	std::string file_path = location.getRoot() + "/" + uri_pair.second;
	if (remove(file_path.c_str()) == 0)
		return Response(204, "", false);
	else
		return server->error(500);
}

// POST method handler
//nginx returns 405 if posting to a static file (no cgi)
Response TCPListener::Post(std::pair<std::string, std::string> uri_pair, Location &location, const Request &request)
{
	(void) uri_pair;
	(void) location;
	(void) request;
	return server->error(405);
}

//event handler for sending information to cgi through its input fd
std::pair<int, int>	TCPListener::Client2CGI(int fd) {
	std::cerr << "on client2cgi\n";
	int client_fd = cgi_handlers[fd]->getClientFd();
	int	cgi_stdin = cgi_handlers[fd]->getWriteEnd();

	Request r = clients[client_fd].getRequest();
	const char* data_ptr = r.getBody().c_str();
	size_t data_left = r.getBody().size();

	while (data_left > 0) {
		ssize_t bytes_written = write(cgi_stdin, data_ptr, data_left);
		if (bytes_written < 0) {
			perror("write");
			return std::pair<int, int>(0, 0);
		}
		data_ptr += bytes_written;
		data_left -= bytes_written;
	}

	eventManager->removeFromMonitoring(cgi_stdin);
	close(cgi_stdin);
	cgi_handlers[fd] = NULL;
	return std::pair<int, int>(0, 0);
}

//event handler for reading information to cgi through its output fd
//then creates a response and stores it in the client that made the original request
std::pair<int, int>	TCPListener::CGI2Client(int fd) {
	std::cerr << "on cgi2client\n";
	std::string response_buffer;
	char		read_buffer[4096];
	size_t		bytes_read;
	int client_fd = cgi_handlers[fd]->getClientFd();
	int	cgi_stdout = cgi_handlers[fd]->getReadEnd();


	while ((bytes_read = read(cgi_stdout, read_buffer, sizeof(read_buffer))) > 0) {
		response_buffer.append(read_buffer, bytes_read);
		std::cerr << "read " << bytes_read << " bytes from cgi\n";
	}
	clients[client_fd].setResponse(Response(200, response_buffer, true));
	eventManager->removeFromMonitoring(cgi_stdout);
	close(cgi_stdout);
	//maybe check kill (avoid zombie process)
	clients[client_fd].setCGI(NULL);
	cgi_handlers[fd] = NULL;
	std::cerr << "cgi response ready\n";
	return std::pair<int, int>(0,0);
}

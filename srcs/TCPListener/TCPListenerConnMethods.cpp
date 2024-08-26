#include "TCPListener.hpp"

//calls the GET, POST or HEAD method requested, and returns the response
Response TCPListener::analizer(const Request &request, Server *s)
{
	std::vector<Location> &locations = s->getLocations();

	std::string decoded = urlDecoder(request.getUri());

	//std::cerr << "Uri before split: " << request.getUri() << std::endl;

	std::pair<std::string, std::string> uri_pair = splitUrl(decoded, locations);

	std::cerr << "Building response for resource " << uri_pair.second << " at location " << uri_pair.first << std::endl;
	for (size_t i = 0; i < locations.size(); i++)
	{
		if (locations[i].getUri() == uri_pair.first)
		{
			if ((locations[i].getMethods() & request.getNumMethod()) == request.getNumMethod())
			{
				if (locations[i].getRedirect().first > 300 && locations[i].getRedirect().first < 308) 
					return (Response(locations[i].getRedirect().first, locations[i].getRedirect().second, ""));
				if (uri_pair.second == "" && locations[i].hasAutoIndex())
					return (Response(200, locations[i].getAutoIndex(), true, true));
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
						return (s->error(404));
					}
				}
				/* std::cerr << "requested method: " << request.getMethod()
					<< " for uri " << uri_pair.first
					<< " resource " << uri_pair.second << std::endl;
				std::cerr << "location method " << locations[i].getMethods() << std::endl; */

				if (request.getNumMethod() == 1)
					return (Post(uri_pair, locations[i], s, request));
				if (request.getNumMethod() == 2 && request.getMethod() == "GET")
					return (Get(uri_pair, locations[i], s));
				if (request.getNumMethod() == 2 && request.getMethod() == "HEAD")
					return (Head(uri_pair, locations[i], s));
				if (request.getNumMethod() == 4)
					return (Delete(uri_pair, locations[i], s));
			}
			else
			{
				return (s->error(403));
			}
			break;
		}
	}
	return (s->error(404));
}

// GET method handler
Response TCPListener::Get(std::pair<std::string, std::string> uri_pair, Location &location, Server *s)
{
	std::string file_path = location.getRoot() + "/" + uri_pair.second;
	
	if (access(file_path.c_str(), F_OK))
		return s->error(404);
	if (access(file_path.c_str(), R_OK))
		return s->error(403);
	if (isDirectory(file_path)) {
		Indexer index(file_path);
		return (Response(200, index.getHtml(), true, true));
	}

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
		return s->error(500);
	}
}

// HEAD method handler
Response TCPListener::Head(std::pair<std::string, std::string> uri_pair, Location &location, Server *s)
{
	std::string file_path = location.getRoot() + "/" + uri_pair.second;
	

	if (access(file_path.c_str(), F_OK))
		return s->error(404);
	if (access(file_path.c_str(), R_OK))
		return s->error(403);
	if (isDirectory(file_path)) {
		Indexer index(file_path);
		return (Response(200, index.getHtml(), false, true));
	}
	//std::cerr << "opening file " << file_path << std::endl;
	std::ifstream file(file_path.c_str());

	if (file.is_open())
	{
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string file_contents = buffer.str();
		file.close();
		return Response(200, file_contents, false, file_path);
	}
	else
	{
		return s->error(500);
	}
}

// DELETE method handler
Response TCPListener::Delete(std::pair<std::string, std::string> uri_pair, Location &location, Server *s)
{
	std::string file_path = location.getRoot() + "/" + uri_pair.second;

	std::string directory = file_path.substr(0, file_path.find_last_of('/'));
	if (access(directory.c_str(), W_OK) != 0)
		return s->error(403);

	if (access(file_path.c_str(), F_OK))
		return s->error(404);
	if (access(file_path.c_str(), R_OK) || access(file_path.c_str(), W_OK))
		return s->error(403);

	if (remove(file_path.c_str()) == 0)
		return Response(204, "", false, true);
	else
		return s->error(500);
}

// POST method handler
//nginx returns 405 if posting to a static file (no cgi), but we write/rewrite to allow easy upload
Response TCPListener::Post(std::pair<std::string, std::string> uri_pair, Location &location, Server *s, const Request &request)
{
	std::string file_path = location.getRoot() + "/" + uri_pair.second;

	if (access(file_path.c_str(), F_OK))
		return s->error(404);
	if (access(file_path.c_str(), R_OK))
		return s->error(403);
	if (access(file_path.c_str(), W_OK))
		return s->error(403);
	std::ofstream outfile;
	std::cerr << "opening file " << file_path << std::endl;
	outfile.open(file_path.c_str());
	if (!outfile.is_open())
		return s->error(500);
	outfile << request.getBody();
	outfile.close();
	return Response(200, request.getBody(), true, uri_pair.second);
}

//event handler for sending information to cgi through its input fd
std::pair<int, int>	TCPListener::Client2CGI(int fd) {
	int client_fd = cgi_handlers[fd]->getClientFd();
	int	cgi_stdin = cgi_handlers[fd]->getWriteEnd();
	const size_t CHUNK_SIZE = 65536;

	Request r = clients[client_fd].getRequest();
	if (!clients[client_fd].data_ptr) {
		std::cerr << "starting client2cgi\n";
		clients[client_fd].data_ptr = r.getBody().c_str();
		clients[client_fd].data_left = r.getBody().size();
		cgi_handlers[fd]->handleRequest();
	}

	if (clients[client_fd].data_left > 0) {
		clients[client_fd].setLastConn(getCurrentEpochMillis());
		ssize_t bytes_to_write = (clients[client_fd].data_left < CHUNK_SIZE) ? clients[client_fd].data_left : CHUNK_SIZE;
        ssize_t bytes_written = write(cgi_stdin, clients[client_fd].data_ptr, bytes_to_write);
		if (bytes_written < 0) {
			perror("write");
			return std::pair<int, int>(0, 0);
		}
		clients[client_fd].data_ptr += bytes_written;
		clients[client_fd].data_left -= bytes_written;
		//std::cerr << "Wrote " << bytes_written << " bytes\n";
		if (clients[client_fd].data_left > 0)
			return std::pair<int, int>(0, 0);
	}
	std::cerr << "End of client2cgi\n";
	eventManager->removeFromMonitoring(cgi_stdin);
	close(cgi_stdin);
	cgi_handlers[fd] = NULL;
	clients[client_fd].data_ptr = NULL;
	return std::pair<int, int>(0, 0);
}

//event handler for reading information to cgi through its output fd
//then creates a response and stores it in the client that made the original request
std::pair<int, int>	TCPListener::CGI2Client(int fd) {
	//std::cerr << "on cgi2client\n";
	std::string response_buffer;
	char		read_buffer[4096];
	size_t		bytes_read;
	int client_fd = cgi_handlers[fd]->getClientFd();
	int	cgi_stdout = cgi_handlers[fd]->getReadEnd();


	while ((bytes_read = read(cgi_stdout, read_buffer, sizeof(read_buffer))) > 0) {
		response_buffer.append(read_buffer, bytes_read);
		//std::cerr << "read " << bytes_read << " bytes from cgi\n";
	}
	//std::cerr << "Response cgi: {" << response_buffer << "}\n";
	if (cgi_handlers[fd]->getExitCode()) {
		clients[client_fd].setResponse(Response(500, response_buffer, true, false));
	} else {
		clients[client_fd].setResponse(Response(200, response_buffer, true, false));
	}
	eventManager->removeFromMonitoring(cgi_stdout);
	close(cgi_stdout);
	clients[client_fd].setCGI(NULL);
	cgi_handlers[fd] = NULL;
	std::cerr << "cgi response ready\n";
	return std::pair<int, int>(0,0);
}

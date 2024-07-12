/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TCPListener.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agserran <agserran@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 13:02:58 by sacorder          #+#    #+#             */
/*   Updated: 2024/07/01 16:58:42 by agserran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TCPListener.hpp"
#include <cstdlib>
#include <iterator>
#include <sys/epoll.h>
#include <sys/socket.h>

TCPListener::TCPListener(int port, Server *server) : port(port)
{
	this->server = server;
	socket_fd = -1;
	this->eventManager = NULL;
}

int TCPListener::start()
{
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	int reuse = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse)) < 0)
	{
		perror("setsockopt(SO_REUSEADDR)");
		exit(EXIT_FAILURE);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}
	// start listening through the socket
	if (listen(socket_fd, 16) == -1)
	{
		perror("Listening failed");
		exit(EXIT_FAILURE);
	}
	std::cout << "Server listening on port " << port << "...\n";
	// return socket to add to epoll
	return socket_fd;
}

int TCPListener::checkEvent(epoll_event ev) {
	if (ev.data.fd == socket_fd && ev.events & EPOLLIN) {
		return newClient();
	} else if (clients[ev.data.fd].responseReady() && matcher[ev.data.fd] == CLIENT && ev.events & EPOLLOUT){
		return sendData(ev.data.fd);
	} else if (matcher[ev.data.fd] == CLIENT && ev.events & EPOLLIN){
		return readData(ev.data.fd);
	} /* else if (matcher[fd] == CGIIN) {
		// send request through
	} else if (matcher[fd] == CGIOUT) {
		// get cgi response from pipe
	} */
	return 0;
}

void TCPListener::createResponse(size_t i) {
	if (clients[i].requestReady())
	{
		clients[i].setResponse(analizer(clients[i].getRequest()));
	}
}

int	TCPListener::newClient() {
	// handle new con
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	std::cerr << "Accepting with fd " << socket_fd << "...\n";
	int client_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);
	if (client_fd == -1)
	{
		perror("Accept failed");
		return 0;
	}
	clients[client_fd].setLastConn(getCurrentEpochMillis());
	matcher[client_fd] = CLIENT;
	std::cerr << "[ " << Response::get_current_date()
		<< " ] : Connection accepted from " << inet_ntoa(client_addr.sin_addr)
		<< ":" << ntohs(client_addr.sin_port) << " through fd " << client_fd
		<< std::endl;
	return client_fd;
}

// reads data from the client in fd and stores it in buffers[fd]
//tries to create a full request after reading, and stores the full request if it is succesfully created
int TCPListener::readData(int fd) 
{
	char buffer[16384];
	int bytesRead;
	bytesRead = recv(fd, buffer, sizeof(buffer), 0);
	//static int ctr;
	//std::cerr << "read (" << buffer << ")\n";
	//std::cerr << "Read " << bytesRead << " bytes\n";
	if (bytesRead == -1)
	{
		perror("recv");
		disconnectClient(fd);
		return 0;
	}
	if (bytesRead == 0) {
		if (isTimeout(clients[fd].getLastConn(), getCurrentEpochMillis(), CONN_TIMEOUT)) {
			std::cerr << "Timeout met: ";
			disconnectClient(fd);
		}
	} else
		clients[fd].setLastConn(getCurrentEpochMillis());
	if (bytesRead > 0) {
		clients[fd].addToRequestBuffer(std::string(buffer, bytesRead));
		//std::cerr << "Trying to create request from " << buffers[fd].size() << " chunks\n";
		Request r = Request(clients[fd].getRequestBuffer());
		if (r.getContentLen() != r.getBody().length()) // if body not complete, skip
			return 0;
		//std::cerr << "---- PARSED REQUEST ----\n" << r << std::endl << "---- PARSED REQUEST END ----\n";
		clients[fd].setRequest(r);
		createResponse(fd);
	}
	return 0;
}

int TCPListener::sendData(int fd) { // sends data from responses[fd] to the client in that socket fd
	int bytes;

	std::string message = clients[fd].getResponseMessage();
	bytes = send(fd, message.c_str(), message.length(), 0);
	clients[fd].clearResponse();

	const Request &r = clients[fd].getRequest();
	if (bytes == -1 || //if send fails, or request sends Connection close
		(r.getHeaders().find("Connection") != r.getHeaders().end()
		&& r.getHeaders()["Connection"] == "close")) {
		disconnectClient(fd); //disconnect
	} else
		clients[fd].setLastConn(getCurrentEpochMillis());
	clients[fd].clearRequest();
	return 0;
}

//disconnects a client
void TCPListener::disconnectClient(int fd) {
	//send epoll ctl delete!!!!
	eventManager->removeFromMonitoring(fd);
	clients[fd].disconnect(fd);
	matcher[fd] = 0;
}

Response TCPListener::analizer(const Request &request)
{
	std::vector<Location> &locations = this->server->getLocations();

	std::pair<std::string, std::string> uri_pair = splitUri(request.getUri());

	//std::cerr << "Building response for resource " << uri_pair.second << " at location " << uri_pair.first << std::endl;
	for (size_t i = 0; i < locations.size(); i++)
	{
		if (locations[i].getUri() == uri_pair.first)
		{
			if (uri_pair.second == "")
				uri_pair.second = locations[i].getIndex();
			//std::cerr << "requested method: " << request.getMethod() << std::endl;
			if ((locations[i].getMethods() & request.getNumMethod()) == request.getNumMethod())
			{
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
				return (Response(405, "405 Error\nThe requested method isn't allowed", true));
			}
			break;
		}
	}
	return (Response(404, "404 Error\nWe tried, but couldn't find :(", true));
}

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
		return Response(200, file_contents, true);
	}
	else
	{
		return Response(500, "500 Error\nCould not open the requested file.", true);
	}
}

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
		return Response(500, "500 Error\nCould not open the requested file.", true);
	}
}

Response TCPListener::Delete(std::pair<std::string, std::string> uri_pair, Location &location)
{
	std::string file_path = location.getRoot() + "/" + uri_pair.second;
	if (remove(file_path.c_str()) == 0)
		return Response(204, "", false);
	else
		return Response(500, "500 Error\nCould not open the requested file.", true);
}

Response TCPListener::Post(std::pair<std::string, std::string> uri_pair, Location &location, const Request &request)
{
	std::string file_path = location.getRoot() + "/" + uri_pair.second;
	std::ofstream outfile;
	std::cerr << "opening file " << file_path << std::endl;
	outfile.open(file_path.c_str());
	if (!outfile.is_open())
		return Response(500, "500 Error\nCould not open the requested file.", true);
	outfile << request.getBody();
	outfile.close();
	return Response(200, request.getBody(), true);
}

std::pair<std::string, std::string> TCPListener::splitUri(std::string uri)
{
	std::size_t pos = uri.find_last_of('/');

	if (pos == std::string::npos)
		return std::make_pair("", uri);
	else if (pos == uri.length())
		return std::make_pair(uri, "");
	else
		return std::make_pair(uri.substr(0, pos + 1), uri.substr(pos + 1));
}

// Get duration since epoch in milliseconds
long long TCPListener::getCurrentEpochMillis() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return static_cast<long long>(tv.tv_sec) * 1000 + tv.tv_usec / 1000;
}

// Returns true once the threshold is greater or equal
bool TCPListener::isTimeout(long long startMillis, long long endMillis, int thresholdSeconds) {
    long long diffMillis = endMillis - startMillis;
    long long diffSeconds = diffMillis / 1000;
    return diffSeconds >= thresholdSeconds;
}

int	TCPListener::getSocketFd() {
	return socket_fd;
}

TCPListener &TCPListener::operator=(const TCPListener &copy)
{
	// std::cerr << "called equal op tcp listener\n";
	this->port = copy.port;
	this->server = copy.server;
	socket_fd = -1;
	eventManager = copy.eventManager;
	return (*this);
}

TCPListener::TCPListener(const TCPListener &copy, Server *s) : server(copy.server)
{
	// std::cerr << "called copy cons tcp listener\n";
	*this = copy;
	server = s;
}

TCPListener::~TCPListener()
{
	if (socket_fd > 0)
		close(socket_fd);
}

void	TCPListener::setEventManager(EventManager *eventManager) {
	this->eventManager = eventManager;
}

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
	epoll_fd = -1;
	socket_fd = -1;
	events = NULL;
}

TCPListener &TCPListener::operator=(const TCPListener &copy)
{
	// std::cerr << "called equal op tcp listener\n";
	this->port = copy.port;
	this->server = copy.server;
	epoll_fd = -1;
	socket_fd = -1;
	events = NULL;
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
	if (epoll_fd > 0)
		close(epoll_fd);
	if (events)
		delete events;
}

void TCPListener::start()
{
	memset(responses, 0, sizeof(responses));
	for (size_t i = 0; i < 4096; ++i) {
		requests[i].first = NULL;
		requests[i].second = false;
		last_conn[i] = __LONG_LONG_MAX__;
	}
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
	// create a new epoll
	epoll_fd = epoll_create(1024);
	if (epoll_fd == -1)
	{
		perror("epoll_create");
		exit(EXIT_FAILURE);
	}

	// assign listening socket to epoll
	event.events = EPOLLIN;
	event.data.fd = socket_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &event) == -1)
	{
		perror("epoll_ctl");
		exit(EXIT_FAILURE);
	}

	// start listening through the socket
	if (listen(socket_fd, 10) == -1)
	{
		perror("Listening failed");
		exit(EXIT_FAILURE);
	}
	events = new std::vector<epoll_event>(MAX_EVENTS);
	std::cout << "Server listening on port " << port << "...\n";
}

void TCPListener::run()
{
	// std::cout << "Server listening on port " << port << "...\n";

	// wait for epoll notification for fd ready
	int nbr_fds = epoll_wait(epoll_fd, events->data(), events->size(), 0);
	if (nbr_fds == -1)
	{
		perror("epoll_wait");
		return;
	}

	// loop through fds that are ready
	for (int i = 0; i < nbr_fds; ++i)
	{
		if ((*events)[i].data.fd == socket_fd)
		{
			// handle new con
			struct sockaddr_in client_addr;
			socklen_t client_addr_len = sizeof(client_addr);
			int client_socket_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);
			if (client_socket_fd == -1)
			{
				perror("Accept failed");
				continue;
			}
			last_conn[client_socket_fd] = __LONG_LONG_MAX__;

			// Add to epoll ctl
			epoll_event client_event;
			client_event.events = EPOLLIN | EPOLLOUT;
			client_event.data.fd = client_socket_fd;
			if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket_fd, &client_event) == -1)
			{
				perror("epoll ctl");
				close(client_socket_fd);
				exit(EXIT_FAILURE);
			}

			std::cerr << "[ " << Response::get_current_date() << " ] : Connection accepted from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;
		}
		else if (responses[(*events)[i].data.fd] && (*events)[i].events & EPOLLOUT) { //send client response if ready
			sendData(i);
		} else if ((*events)[i].events & EPOLLIN) { // read client data
			readData(i);
		}
	}
}

void TCPListener::createResponse(size_t i) {
	if (requests[i].second == true) {
		responses[i] = new Response(analizer(*requests[i].first));
		requests[i].second = false;
	}
}

void TCPListener::sendData(int pos) { // sends data from responses[client_socket_fd] to the client in (*events)[pos]
	int client_socket_fd = (*events)[pos].data.fd;
	//std::cerr << "Response built succesfully\n";
	//std::cerr << *responses[client_socket_fd];
	int bytes;

	std::string message = responses[client_socket_fd]->getMessage();
	bytes = send(client_socket_fd, message.c_str(), message.length(), 0);
	delete responses[client_socket_fd];
	responses[client_socket_fd] = NULL;
	if (bytes == -1 || //if send fails, or request sends Connection close
		(requests[client_socket_fd].first->getHeaders().find("Connection") != requests[client_socket_fd].first->getHeaders().end()
		&& requests[client_socket_fd].first->getHeaders()["Connection"] == "close")) {
		// remove from epoll and close connection
		disconnectClient(pos);
	} else
		last_conn[client_socket_fd] = getCurrentEpochMillis();
	delete requests[client_socket_fd].first;
	requests[client_socket_fd].first = NULL;
}

// reads data from the client in (*events)[pos] and stores it in buffers[client_socket_fd]
//tries to create a full request after reading, and stores the full request if it is succesfully created
void TCPListener::readData(int pos) 
{
	int client_socket_fd = (*events)[pos].data.fd;
	char buffer[16384];
	int bytesRead;
	bytesRead = recv(client_socket_fd, buffer, sizeof(buffer), 0);
	//static int ctr;
	//std::cerr << "read (" << buffer << ")\n";
	//std::cerr << "Read " << bytesRead << " bytes\n";
	if (bytesRead == -1)
	{
		perror("recv");
		disconnectClient(pos);
		return ;
	}
	if (bytesRead == 0) {
		if (isTimeout(last_conn[client_socket_fd], getCurrentEpochMillis(), CONN_TIMEOUT)) {
			std::cerr << "Timeout met: ";
			disconnectClient(pos);
		}
	} else
		last_conn[client_socket_fd] = getCurrentEpochMillis();
	if (bytesRead > 0) {
		buffers[client_socket_fd].push_back(std::string(buffer, bytesRead));
		//std::cerr << "Trying to create request from " << buffers[client_socket_fd].size() << " chunks\n";
		Request r = Request(buffers[client_socket_fd]);
		if (r.getContentLen() != r.getBody().length()) // if body not complete, skip
			return ;
		//std::cerr << "---- PARSED REQUEST ----\n"
		//		<< r << std::endl
		//		<< "---- PARSED REQUEST END ----\n";
		
		requests[client_socket_fd].first = new Request(r);
		requests[client_socket_fd].second = true;
		createResponse(client_socket_fd);
	}
}

//disconnects a client
void TCPListener::disconnectClient(int pos) {
	int client_socket_fd = (*events)[pos].data.fd;

	std::cerr << "[ " << Response::get_current_date() << " ] : Closing connection with " << client_socket_fd << std::endl;
	// remove from epoll and close connection
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, (*events)[pos].data.fd, NULL) == -1) {
		perror("epoll_ctl");
	}
	close(client_socket_fd);
	buffers[client_socket_fd].clear();
	last_conn[client_socket_fd] = __LONG_LONG_MAX__;
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
		// TODO: change it to forbidden instead of internal server error
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
		// TODO: change it to forbidden instead of internal server error
		return Response(500, "500 Error\nCould not open the requested file.", true);
	}
}

Response TCPListener::Delete(std::pair<std::string, std::string> uri_pair, Location &location)
{
	std::string file_path = location.getRoot() + "/" + uri_pair.second;
	if (remove(file_path.c_str()) == 0)
		return Response(204, "", false);
	else
		// TODO: change it to forbidden instead of internal server error
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

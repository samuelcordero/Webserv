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

TCPListener::TCPListener(int port, Server *server) : port(port) {
	this->server = server;
	epoll_fd = -1;
	socket_fd = -1;
	events = NULL;
}

TCPListener& TCPListener::operator=(const TCPListener& copy)
{
	//std::cerr << "called equal op tcp listener\n";
	this->port = copy.port;
	this->server = copy.server;
	epoll_fd = -1;
	socket_fd = -1;
	events = NULL;
	return (*this);
}

TCPListener::TCPListener(const TCPListener& copy, Server *s) : server(copy.server)
{
	//std::cerr << "called copy cons tcp listener\n";
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

void TCPListener::start() {
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

	memset(&server_addr, 0, sizeof(server_addr)); // forbidden function
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

			// Add to epoll ctl
			epoll_event client_event;
			client_event.events = EPOLLIN | EPOLLET; // Use edge-triggered mode
			client_event.data.fd = client_socket_fd;
			if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket_fd, &client_event) == -1)
			{
				perror("epoll ctl");
				close(client_socket_fd);
				exit(EXIT_FAILURE);
			}

			std::cout << "Connection accepted from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;
		}
		else
		{
			// handle client response
			std::cout << "Handling response..." << std::endl;
			mock_handler((*events)[i].data.fd);

			// delete from epoll ctr monitoring
			if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, (*events)[i].data.fd, NULL) == -1)
			{
				perror("epoll_ctl");
			}
			close((*events)[i].data.fd);
		}
	}
}

void TCPListener::mock_handler(int client_socket_fd)
{
	char buffer[1024];

	int bytesRead = recv(client_socket_fd, buffer, sizeof(buffer), 0);
	if (bytesRead == -1)
	{
		perror("recv");
		return;
	}
	if (bytesRead == 0)
		return ; //wtf
	// Process the received data (parse the HTTP request and generate a response)
	std::string request(buffer, bytesRead);
	//std::cout << "---- RECEIVED REQUEST ----\n"
	//		  << request << "---- REQUEST END ----\n";
	Request r = Request(request);
	std::cout << "---- PARSED REQUEST ----\n"
			  << r << std::endl
			  << "---- PARSED REQUEST END ----\n";
	//logica
	Response resp = analizer(r);

	std::cout << "Response built succesfully\n";

	std::cout << resp;

	// Send a response back to the client (THIS IS OBVIOUSLY A MOCK!)
	std::string message = resp.getMessage();
	send(client_socket_fd, message.c_str(), message.length(), 0);
}

std::pair<std::string, std::string>	splitUri(std::string uri) {
	std::size_t pos = uri.find_last_of('/');

    if (pos == std::string::npos)
        return std::make_pair("", uri);
    else if (pos == uri.length())
        return std::make_pair(uri, "");
    else
        return std::make_pair(uri.substr(0, pos + 1), uri.substr(pos + 1));
}

static Response Get(std::pair<std::string, std::string> uri_pair, Location& location)
{
	std::string file_path = location.getRoot() + "/" + uri_pair.second;
	std::cerr << "opening file " << file_path << std::endl;
	std::ifstream file(file_path.c_str());
				
    if (file.is_open()) {
        std::stringstream buffer;
        buffer << file.rdbuf();
    	std::string file_contents = buffer.str();
        file.close();
        return Response(200, "OK", file_contents);
    } else {
        return Response(500, "Internal Server Error", "500 Error\nCould not open the requested file.");
    }
}

static Response	Delete(std::pair<std::string, std::string> uri_pair, Location &location)
{
	std::string file_path = location.getRoot() + "/" + uri_pair.second;
	if (remove(file_path.c_str()) == 0)
		return Response(204, "No content", "");
	else
		return Response(500, "Internal Server Error", "500 Error\nCould not open the requested file.");
}

static Response Post(std::pair<std::string, std::string> uri_pair, Location& location, const Request& request)
{
	std::string file_path = location.getRoot() + "/" + uri_pair.second;
	std::ofstream	outfile;

	outfile.open(file_path.c_str());
	if (!outfile.is_open())
		return Response(500, "Internal Server Error", "500 Error\nCould not open the requested file.");
	outfile << request.getBody();
	outfile.close();
	return Response(200, "OK" ,request.getBody());
}

Response TCPListener::analizer(const Request& request)
{
	std::vector<Location> &locations = this->server->getLocations();

	std::pair<std::string, std::string> uri_pair = splitUri(request.getUri());

	std::cerr << "Building response for resource " << uri_pair.second << " at location " << uri_pair.first << std::endl;
	for (size_t i = 0; i < locations.size(); i++)
	{
		if (locations[i].getUri() == uri_pair.first)
		{
			if (uri_pair.second == "")
				uri_pair.second = locations[i].getIndex();
			std::cerr << "requested method: " << request.getNumMethod() << std::endl;
			if ((locations[i].getMethods() & request.getNumMethod()) == request.getNumMethod())
			{
        if (request.getNumMethod() == 2)
					return (Get(uri_pair, locations[i]));
				if (request.getNumMethod() == 1)
					return (Post(uri_pair, locations[i], request));
				if (request.getNumMethod() == 4)
					return (Delete(uri_pair, locations[i]));
			}
			else
			{
				return (Response(405, "Method Not Allowed", "405 Error\nThe requested method isn't allowed"));
			}
			break;
		}
	}
	return (Response(404, "Not Found", "404 Error\nWe tried, but couldn't find :("));
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TCPListener.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agserran <agserran@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 13:02:58 by sacorder          #+#    #+#             */
/*   Updated: 2024/06/26 17:18:59 by agserran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TCPListener.hpp"
#include <cstdlib>
#include <iterator>
#include <sys/epoll.h>
#include <sys/socket.h>
#include "Request.hpp"
#include "Response.hpp"

TCPListener::TCPListener(int port, Server *server) : port(port) {
	this->server = server;
	epoll_fd = -1;
	socket_fd = -1;
	events = NULL;
}

TCPListener& TCPListener::operator=(const TCPListener& copy)
{
	std::cerr << "called equal op tcp listener\n";
	this->port = copy.port;
	this->server = copy.server;
	epoll_fd = -1;
	socket_fd = -1;
	events = NULL;
	return (*this);
}

TCPListener::TCPListener(const TCPListener& copy) : server(copy.server)
{
	std::cerr << "called copy cons tcp listener\n";
	*this = copy;
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

	// Process the received data (parse the HTTP request and generate a response)
	std::string request(buffer, bytesRead);
	//logica
	std::cout << "---- RECEIVED REQUEST ----\n"
			  << request << "---- REQUEST END ----\n";
	Request r = Request(request);
	std::cout << "---- PARSED REQUEST ----\n"
			  << r << std::endl
			  << "---- PARSED REQUEST END ----\n";

	// Send a response back to the client (THIS IS OBVIOUSLY A MOCK!)
	Response response(404, "Not found", "404: couldn't find requested resource");
	std::cout << response;
	std::string message = response.getMessage();
	send(client_socket_fd, message.c_str(), message.length(), 0);
}
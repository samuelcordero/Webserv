/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TCPListener.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agserran <agserran@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 13:02:58 by sacorder          #+#    #+#             */
/*   Updated: 2024/07/15 13:02:33 by agserran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TCPListener.hpp"

TCPListener::TCPListener(int port, Server *server) : port(port)
{
	this->server = server;
	socket_fd = -1;
	this->eventManager = NULL;
}

//destructor
TCPListener::~TCPListener()
{
	if (socket_fd > 0)
		close(socket_fd);
}

//equal op overrride
TCPListener &TCPListener::operator=(const TCPListener &copy)
{
	// std::cerr << "called equal op tcp listener\n";
	this->port = copy.port;
	this->server = copy.server;
	socket_fd = -1;
	eventManager = copy.eventManager;
	return (*this);
}

//copy cons
TCPListener::TCPListener(const TCPListener &copy, Server *s) : server(copy.server)
{
	// std::cerr << "called copy cons tcp listener\n";
	*this = copy;
	server = s;
}

//starts the listener, cretes a socket and binds it to requeted port
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

//socket_fd getter
int	TCPListener::getSocketFd() {
	return socket_fd;
}

//event manager setter, latter used to remove events from monitoring
void	TCPListener::setEventManager(EventManager *eventManager) {
	this->eventManager = eventManager;
}

//gets an event passed originally from the controller
//checks which type of event is and then executes corresponding handler
//returns up to two new fds to add to the event monitor through the pair<int, int>
std::pair<int, int> TCPListener::checkEvent(epoll_event ev) {
	if (ev.data.fd == socket_fd && ev.events & EPOLLIN) {
		return newClient();
	} else if (clients[ev.data.fd].responseReady() && matcher[ev.data.fd] == CLIENT && ev.events & EPOLLOUT){
		return sendData(ev.data.fd);
	} else if (matcher[ev.data.fd] == CLIENT && ev.events & EPOLLIN){
		return readData(ev.data.fd);
	} else if (matcher[ev.data.fd] == CGI_WRITE) {
		return Client2CGI(ev.data.fd);
	} else if (matcher[ev.data.fd] == CGI_READ && cgi_handlers[ev.data.fd]->executionDone()) {
		return CGI2Client(ev.data.fd);
	} else if (matcher[ev.data.fd] == CGI_READ) {
		if (isTimeout(clients[cgi_handlers[ev.data.fd]->getClientFd()].getCgiStartTime(), getCurrentEpochMillis(), CGI_TIMEOUT)) {
			std::cerr << "Cgi timeout met\n";
			clients[cgi_handlers[ev.data.fd]->getClientFd()].setResponse(Response(500, "500 Error\nCouldn't generate cgi response", true));
			killCGI(ev.data.fd);
		}
	}
	return std::pair<int, int>(0, 0);
}

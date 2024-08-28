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

TCPListener::TCPListener(int port, const std::string &listenTo) : port(port)
{
	server_ctr = 0;
	socket_fd = -1;
	this->eventManager = NULL;
	start(listenTo);
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
	for (int i = 0; i < copy.server_ctr; ++i)
		servers[i] = copy.servers[i];
	server_ctr = copy.server_ctr;
	socket_fd = -1;
	eventManager = copy.eventManager;
	return (*this);
}

//copy cons
TCPListener::TCPListener(const TCPListener &copy)
{
	// std::cerr << "called copy cons tcp listener\n";
	*this = copy;
}

//starts the listener, cretes a socket and binds it to requeted port
int TCPListener::start(const std::string &listenTo)
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
    if (inet_aton(listenTo.c_str(), &server_addr.sin_addr) == 0) {
        std::cerr << "Invalid address/Address not supported: " << listenTo << std::endl;
        exit(EXIT_FAILURE);
    }

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
	std::cout << "Listening on port " << port << "...\n";
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
	} else if (matcher[ev.data.fd] == CGI_READ && !cgi_handlers[ev.data.fd]->executionDone()) {
		if (isTimeout(clients[cgi_handlers[ev.data.fd]->getClientFd()].getCgiStartTime(), getCurrentEpochMillis(), CGI_TIMEOUT)) {
			usleep(5000);
			if (!cgi_handlers[ev.data.fd]->executionDone()) {
				std::cerr << "Cgi timeout met\n";
				Server *s = servers[0];
				for (int j = 0; j < server_ctr; ++j) {
					if (servers[j]->getName() == clients[cgi_handlers[ev.data.fd]->getClientFd()].getRequest().getHeaders()["Referer"]) {
						s = servers[j];
					}
				}	
				clients[cgi_handlers[ev.data.fd]->getClientFd()].setResponse(s->error(504)); //check which server from request, send error
				killCGI(ev.data.fd);
			}
		}
	}
	return std::pair<int, int>(0, 0);
}

bool	TCPListener::attachServer(Server *s) {
	if (server_ctr + 1 < MAX_SERVERS) {
		servers[server_ctr++] = s;
		return true;
	}

	return false;
}

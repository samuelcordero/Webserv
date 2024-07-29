#include "TCPListener.hpp"

//creates a new client and returns it socket fd
std::pair<int, int>	TCPListener::newClient() {
	// handle new con
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	std::cerr << "Accepting with fd " << socket_fd << "...\n";
	int client_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);
	if (client_fd == -1)
	{
		perror("Accept failed");
		return std::pair<int, int>(0,0);
	}
	clients[client_fd].setLastConn(getCurrentEpochMillis());
	matcher[client_fd] = CLIENT;
	std::cerr << "[ " << Response::get_current_date()
		<< " ] : Connection accepted from " << inet_ntoa(client_addr.sin_addr)
		<< ":" << ntohs(client_addr.sin_port) << " through fd " << client_fd
		<< std::endl;
	return std::pair<int, int>(client_fd, 0);
}

// reads data from the client in fd and stores it in buffers[fd]
//tries to create a full request after reading, and stores the full request if it is succesfully created
std::pair<int, int> TCPListener::readData(int fd) 
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
		return std::pair<int, int>(0,0);
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
			return std::pair<int, int>(0,0);
		//std::cerr << "---- PARSED REQUEST ----\n" << r << std::endl << "---- PARSED REQUEST END ----\n";
		clients[fd].setRequest(r);
		return createResponse(fd);
	}
	return std::pair<int, int>(0,0);
}

// sends data from responses[fd] to the client in that socket fd
std::pair<int, int> TCPListener::sendData(int fd) {
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
	return std::pair<int, int>(0,0);
}

//disconnects a client
void TCPListener::disconnectClient(int fd) {
	CGIHandler *cgi = clients[fd].getCGI();
	if (cgi) {
		if (!cgi->executionDone()){
			eventManager->removeFromMonitoring(cgi->getReadEnd());
			close(cgi->getReadEnd());
			cgi->checkAndKill();
		}
		clients[fd].setCGI(NULL);
	}
	eventManager->removeFromMonitoring(fd);
	clients[fd].disconnect(fd);
	matcher[fd] = 0;
	std::cerr << "Connection closed with " << fd << std::endl;
}

//creates a response
//response can be generated on the go if it refers two a static file
//if it requires cgi, cretes a handler and returns an input and output fd for the handler through the pair
std::pair<int, int> TCPListener::createResponse(size_t i) {
	if (clients[i].requestReady())
	{
		if (checkCgiRequest(i))
			return createCgiHandler(i);
		else
			clients[i].setResponse(analizer(clients[i].getRequest()));
	}
	return std::pair<int, int>(0,0);
}

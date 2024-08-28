#include "TCPListener.hpp"

//checks if a request is a valid cgi requests
bool	TCPListener::checkCgiRequest(int fd, Server *s) {
	Request r = clients[fd].getRequest();
	std::vector<Location> &locations = s->getLocations();

	std::pair<std::string, std::string> uri_pair = splitUrl(r.getUri(), locations);
	if (uri_pair.second == "")
		return false;

	std::string::size_type pos = uri_pair.second.rfind('.');

    if (pos == std::string::npos) {
        return false;
    }

	std::string file_ext = uri_pair.second.substr(pos);

	std::cerr << "File ext: " << file_ext << std::endl;
	if (file_ext == ".py" || file_ext == ".php") {
		for (size_t i = 0; i < locations.size(); ++i) {
			if (locations[i].getUri() == uri_pair.first) {
				if (locations[i].getCgi().first == file_ext
					&& ((locations[i].getMethods() & r.getNumMethod()) == POST || (locations[i].getMethods() & r.getNumMethod()) == GET)) //allow both get and post methods for cgi
						return true;
				return false;
			}
		}
	}

    return false;
}

//for valid cgi requests only (see TCPListener::checkCgiRequest)
//creates a cgi handler
//returns a pair of fds, read(first) and write(second) for the forked process
std::pair<int, int>	TCPListener::createCgiHandler(int fd, Server *s) {
	size_t	i = 0;
	std::vector<Location> &locations = s->getLocations();
	Request r = clients[fd].getRequest();

	std::pair<std::string, std::string> uri_pair = splitUrl(r.getUri(), locations);

	for (i = 0; i < locations.size(); ++i) {
		if (locations[i].getUri() == uri_pair.first)
			break ;
	}

	std::string scriptPath = locations[i].getRoot() + "/" + uri_pair.second;
	
	std::cerr << "Building cgi response for resource " << uri_pair.second << " at location " << scriptPath
		<< " for client " << fd << std::endl;

	CGIHandler *handler = new CGIHandler(scriptPath, locations[i].getCgi().second, fd, clients[fd].getRequest());
	clients[fd].setCGI(handler);
	clients[fd].setCgiStartTime(getCurrentEpochMillis());
	int writeEnd = handler->getWriteEnd();
	if (clients[fd].getRequest().getNumMethod() == POST) {
		cgi_handlers[writeEnd] = handler;
		std::cerr << "Adding write end with fd " << handler->getWriteEnd() << "...\n";
		matcher[writeEnd] = CGI_WRITE;
	} else {
		close(writeEnd);
		writeEnd = -1;
		handler->handleRequest();
	}
	cgi_handlers[handler->getReadEnd()] = handler;
	std::cerr << "Adding read end with fd " << handler->getReadEnd() << "...\n";
	matcher[handler->getReadEnd()] = CGI_READ;
	return std::pair<int, int>(writeEnd, handler->getReadEnd());
}

//kills cgi when it timesout
void	TCPListener::killCGI(int fd) {
	CGIHandler *cgi = cgi_handlers[fd];
	if (cgi_handlers[cgi->getWriteEnd()]) { //close input if still open
		eventManager->removeFromMonitoring(cgi->getWriteEnd());
		cgi_handlers[cgi->getWriteEnd()] = NULL;
		close(cgi->getWriteEnd());
	}
	if (cgi_handlers[cgi->getReadEnd()]) { //close output (shopuld be open but check anyway)
		eventManager->removeFromMonitoring(cgi->getReadEnd());
		cgi_handlers[cgi->getReadEnd()] = NULL;
		close(cgi->getReadEnd());
	}
	clients[cgi->getClientFd()].setCGI(NULL); //deletes before setting to NULL, no leaks
}

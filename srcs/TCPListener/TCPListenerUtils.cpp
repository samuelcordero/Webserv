#include "TCPListener.hpp"

//splits an uri into path(first), filename(second)
//result is returned in a pair of strings
std::pair<std::string, std::string> TCPListener::splitUri(std::string uri)
{
	std::string tmp = uri;
    std::string::size_type pos = uri.find('?');

    if (pos != std::string::npos) {
        tmp = uri.substr(0, pos);
    }

	pos = tmp.find_last_of('/');

	if (pos == std::string::npos)
		return std::make_pair("", tmp);
	else if (pos == uri.length())
		return std::make_pair(tmp, "");
	else
		return std::make_pair(tmp.substr(0, pos + 1), tmp.substr(pos + 1));
}

//returns time since epoch in milliseconds
long long TCPListener::getCurrentEpochMillis() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return static_cast<long long>(tv.tv_sec) * 1000 + tv.tv_usec / 1000;
}

//returns true once the threshold is greater or equal to the time diff
bool TCPListener::isTimeout(long long startMillis, long long endMillis, int thresholdSeconds) {
    long long diffMillis = endMillis - startMillis;
    long long diffSeconds = diffMillis / 1000;
    return diffSeconds >= thresholdSeconds;
}


//checks if a request is a valid cgi requests
bool	TCPListener::checkCgiRequest(int fd, Server *s) {
	Request r = clients[fd].getRequest();
	std::vector<Location> &locations = s->getLocations();

	std::pair<std::string, std::string> uri_pair = splitUri(r.getUri());
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

	std::pair<std::string, std::string> uri_pair = splitUri(r.getUri());

	for (i = 0; i < locations.size(); ++i) {
		if (locations[i].getUri() == uri_pair.first)
			break ;
	}

	std::string scriptPath = locations[i].getRoot() + "/" + uri_pair.second;
	
	std::cerr << "Building cgi response for resource " << uri_pair.second << " at location " << uri_pair.first
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

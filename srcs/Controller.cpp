#include "Controller.hpp"

Controller::Controller() {}

Controller::~Controller() {
	//delete listeners
	for (std::map<int, TCPListener*>::iterator it = listeners.begin(); it != listeners.end(); ++it) {
		delete it->second;
	}
}

bool	Controller::parse(std::string config_file_path) {
	Parser p(config_file_path);
	memset(listener_matcher, 0, sizeof(TCPListener *) * 4096);
	if (p.noErrors()) {
		servers = p.getServers();
		return true;
	}
	return false;
}

void	Controller::solveEvent(epoll_event ev) {
	TCPListener *listener = listener_matcher[ev.data.fd];
	std::pair<int, int> fd_pair = listener->checkEvent(ev);

	if (fd_pair.first != 0) {
		if (fd_pair.second == 0) { //if new client
			listener_matcher[fd_pair.first] = listener;
			event_manager.addToMonitoring(fd_pair.first, EPOLLIN | EPOLLOUT);
		} else { //else cgi
			if (fd_pair.first > 0) {
				listener_matcher[fd_pair.first] = listener;
				event_manager.addToMonitoring(fd_pair.first, EPOLLOUT);
			}
			listener_matcher[fd_pair.second] = listener;
			event_manager.addToMonitoring(fd_pair.second, EPOLLIN);
		}
	}
}

void	Controller::run() {
	//initialize listeners and attach servers
	for (size_t i = 0; i < servers.size(); ++i) {
		if (listeners.find(servers[i].getPort()) == listeners.end()) {
			listeners[servers[i].getPort()] = new TCPListener(servers[i].getPort());
			listeners[servers[i].getPort()]->setEventManager(&event_manager);
			listener_matcher[listeners[servers[i].getPort()]->getSocketFd()] = listeners[servers[i].getPort()];
			event_manager.addToMonitoring(listeners[servers[i].getPort()]->getSocketFd(), EPOLLIN);
			if (!listeners[servers[i].getPort()]->attachServer(&servers.at(i)))
				exit(1);
		} else {
			if (!listeners[servers[i].getPort()]->attachServer(&servers.at(i)))
				exit(1);
		}
	}

	//main execution loop
	while (true) {
		events = event_manager.getNonblockingEvents();

		for (size_t i = 0; i < events.first; ++i) {
			//std::cerr << "solving event for " << events.second->at(i).data.fd << std::endl;
			solveEvent(events.second->at(i));
		}
	}
}

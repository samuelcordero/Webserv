#include "Controller.hpp"

Controller::Controller() {}

Controller::~Controller() {}

bool	Controller::parse(std::string config_file_path) {
	Parser p(config_file_path);
	memset(matcher, 0, sizeof(Server *) * 4096);
	if (p.noErrors()) {
		servers = p.getServers();
		return true;
	}
	return false;
}

void	Controller::solveEvent(epoll_event ev) {
	Server *s = matcher[ev.data.fd];
	std::pair<int, int> fd_pair = s->event(ev);

	if (fd_pair.first > 0) {
		if (fd_pair.second == 0) { //if new client
			matcher[fd_pair.first] = s;
			event_manager.addToMonitoring(fd_pair.first, EPOLLIN | EPOLLOUT);
		} else { //else cgi
			matcher[fd_pair.first] = s;
			event_manager.addToMonitoring(fd_pair.first, EPOLLIN);
			matcher[fd_pair.second] = s;
			event_manager.addToMonitoring(fd_pair.second, EPOLLOUT);
		}
	}
}

void	Controller::run() {
	for (size_t i = 0; i < servers.size(); ++i) {
		event_manager.addToMonitoring(servers.at(i).start(&event_manager), EPOLLIN);
		matcher[servers.at(i).getSocketFd()] = &servers.at(i);
	}
	while (true) {
		events = event_manager.getNonblockingEvents();

		for (size_t i = 0; i < events.first; ++i) {
			//std::cerr << "solving event for " << events.second->at(i).data.fd << std::endl;
			solveEvent(events.second->at(i));
		}
	}
}

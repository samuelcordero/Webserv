#include "EventManager.hpp"

EventManager::EventManager() {
	epoll_fd = epoll_create(1024);
	if (epoll_fd == -1)
	{
		perror("epoll_create");
		exit(EXIT_FAILURE);
	}
	events = new std::vector<epoll_event>(MAX_EVENTS);
}

EventManager::~EventManager() {
	close(epoll_fd);
	delete events;
}

void	EventManager::addToMonitoring(int fd, uint32_t events) {
	epoll_event event;

	event.data.fd = fd;
	event.events = events;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1) {
		perror("epoll ctl");
		exit(EXIT_FAILURE);
	}
}

void	EventManager::removeFromMonitoring(int fd) {
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) {
		perror("epoll_ctl");
		exit(EXIT_FAILURE);
	}
}

std::pair<size_t, std::vector<epoll_event> *>	EventManager::getNonblockingEvents() {
	size_t nbr_fds = epoll_wait(epoll_fd, events->data(), events->size(), 0);

	std::pair<int, std::vector<epoll_event> *> p;
	p.first = nbr_fds;
	p.second = events;
	return p;
}

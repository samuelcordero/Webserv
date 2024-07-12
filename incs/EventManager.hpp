#pragma once
# include <sys/epoll.h>
# include <unistd.h>
# include <cstdio>
# include <vector>
# include <utility>
# include <cstdlib>

# define MAX_EVENTS 128

class EventManager {
	private:
		int							epoll_fd;
		epoll_event					event;
		std::vector<epoll_event>	*events;
	public:
		EventManager();
		//EventManager(const EventManager &other);
		~EventManager();
		void			addToMonitoring(int fd, uint32_t events);
		void			removeFromMonitoring(int fd);
		//EventManager	&operator=(const EventManager &other);

		std::pair<size_t, std::vector<epoll_event> *>	getNonblockingEvents();
};
#pragma once

# include "EventManager.hpp"
# include "config_parser.hpp"
# include "Server.hpp"
# include "TCPListener.hpp"

class Controller {
	private:
		EventManager				event_manager;
		std::vector<Server>			servers;
		std::map<int, TCPListener*>	listeners;
		TCPListener					*listener_matcher[4096];

		std::pair<size_t, std::vector<epoll_event> *>	events;
		void	solveEvent(epoll_event ev);
	public:
		Controller();
		~Controller();

		bool	parse(std::string config_file_path);
		void	run();
};
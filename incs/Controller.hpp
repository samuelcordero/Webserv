#pragma once

# include "EventManager.hpp"
# include "config_parser.hpp"
# include "Server.hpp"
# include "TCPListener.hpp"

# define TBCHECKS 8

class Controller {
	private:
		EventManager				event_manager;
		std::vector<Server>			servers;
		std::map<int, TCPListener*>	listeners;
		TCPListener					*listener_matcher[4096];
		long long					last_check;

		std::pair<size_t, std::vector<epoll_event> *>	events;
		void	solveEvent(epoll_event ev);
		void	checkServers();
	public:
		Controller();
		~Controller();

		bool	parse(std::string config_file_path);
		void	run();
};
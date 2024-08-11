#pragma once

#include "config_parser.hpp"
#include "TCPListener.hpp"
#include "EventManager.hpp"
#include "Errors.hpp"
#include "Response.hpp"

class Location;

class TCPListener;

class Server {
	private:
		std::string 												name;
		int 														port;
		std::string													listen;
		std::vector<Location> 										locations;
		std::string													block;
		std::vector<std::string>									words;
		unsigned long												maxBodySize;
		TCPListener													*listener;
		Errors														*errorHandler;
		std::map<std::string, std::string>							errorMap;

	public:
		Server();
		Server(std::string serverBlock);
		~Server();
		Server& operator=(const Server& copy);
		Server(const Server& copy);
		std::string getName();
		int getPort();
		std::vector<Location> &getLocations();
		void		splitBlock();
		int			setAuto();
		int			getStatus(std::string word, int prevStatus);
		void		fillValues();
		void		setName(size_t i);
		void		setListen(size_t i);
		void		createLocation(size_t i);
		void		setMaxBodySize(size_t i);
		void		addToErrorMap(size_t i);
		int			start(EventManager *eventManager);
		int			getSocketFd();
		std::pair<int, int>	event(epoll_event ev);
		Response	error(int error_code);
};
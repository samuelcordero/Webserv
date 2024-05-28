#pragma once

#include "config_parser.hpp"
#include "TCPListener.hpp"

class Location;

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
	//	std::vector<std::pair <std::string, std::string>>			cgi;

	public:
		Server(std::string serverBlock);
		~Server();
		std::string getName();
		int getPort();
		//std::vector<Location> getLocations();
		void		splitBlock();
		int			setAuto();
		int			getStatus(std::string word, int prevStatus);
		void		fillValues();
		void		setName(size_t i);
		void		setListen(size_t i);
		void		createLocation(size_t i);
		void		setMaxBodySize(size_t i);
		void		serverRun();
};
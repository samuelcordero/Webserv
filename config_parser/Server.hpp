#pragma once

#include "config_parser.hpp"

class Server {
	private:
		std::string 			name;
		int 					port;
		std::vector<Location> 	locations;

	public:
		Server(std::string serverBlock);
		~Server();
		std::string getName();
		int getPort();
		std::vector<Location> getLocations();
		void	parseBlock(std::string serverBlock);
};
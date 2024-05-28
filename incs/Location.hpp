#pragma once

#include "config_parser.hpp"

#define POST 1
#define GET  2
#define DELETE 4

class Location {
	private:
		std::string							index;
		std::string 						uri;
		int									flagsMethods;
		std::pair<std::string, std::string>	cgi;
	
	public:
		Location(std::vector<std::string> locationBlock);
		~Location();
		std::string					getPath();
		std::string					getRoot();
		std::vector<std::string>	getMethods();	
		void						setMethods(size_t i, const std::vector<std::string> &locationBlock);
		void						setIndex(size_t i, std::vector<std::string> &locationBlock);
		void						setCgi(size_t i, std::vector<std::string> &locationBlock);
};

#pragma once

#include "config_parser.hpp"

enum Methods {
	POST = 1,
	GET = 2,
	DELETE = 4
};

class Location {
	private:
		std::string							index;
		std::string 						uri;
		int									flagsMethods;
		std::pair<std::string, std::string>	cgi;
		std::string							root;
	
	public:
		Location(std::vector<std::string> locationBlock);
		~Location();
		std::string					getUri();
		std::string					getRoot();
		int							getMethods();	
		void						setMethods(size_t i, const std::vector<std::string> &locationBlock);
		void						setIndex(size_t i, std::vector<std::string> &locationBlock);
		void						setCgi(size_t i, std::vector<std::string> &locationBlock);
		void						setRoot(size_t i, std::vector<std::string> &locationBlock);
};

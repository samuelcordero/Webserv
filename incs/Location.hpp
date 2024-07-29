#pragma once

#include "config_parser.hpp"

enum Methods {
	POST = 1,
	GET = 2,
	DELETE = 4
};

class Location {
	private:
		std::vector<std::string>			index;
		std::string 						uri;
		std::string							root;
		int									flagsMethods;
		std::pair<std::string, std::string>	cgi;
	public:
		Location(std::vector<std::string> locationBlock);
		Location(const Location &other);
		~Location();
		std::string					getUri();
		std::string					getRoot();
		std::vector<std::string>	getIndex();
		int							getMethods();
		std::pair<std::string, std::string>	getCgi();
	
		size_t						setMethods(size_t i, const std::vector<std::string> &locationBlock);
		size_t						setIndex(size_t i, std::vector<std::string> &locationBlock);
		size_t						setCgi(size_t i, std::vector<std::string> &locationBlock);
		size_t						setRoot(size_t i, std::vector<std::string> &locationBlock);
		Location					&operator=(const Location &other);
};

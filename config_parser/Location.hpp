#pragma once

#include "config_parser.hpp"

enum	states{
	START,
	ERROR,
	SERVER, //0
	SERVER_OPEN, //0
	SERVER_CLOSE, // 9
	KEYWORD, // 2
	VALUE, // 3
	SEMICOLON, // 4
	LOCATION, // 5
	LOCATION_URI, // 8
	LOCATION_OPEN, // 6
	LOCATION_CLOSE // 7
};

class Location {
	private:
		std::string index;
		std::string root;
		std::vector<std::string> methods;
	
	public:
		Location();
		Location(std::string path, std::string root, std::vector<std::string> methods);
		~Location();
		std::string getPath();
		std::string getRoot();
		std::vector<std::string> getMethods();		
};


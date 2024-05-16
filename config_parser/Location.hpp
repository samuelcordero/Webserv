#pragma once

#include "config_parser.hpp"

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
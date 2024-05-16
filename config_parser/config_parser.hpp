#pragma once

#include <string>
#include <iostream>
#include <vector>

#define default_name "default_name"
#define default_port 4242

class Location {
	private:
		std::string path;
		std::string root;
		std::vector<std::string> methods;
	
	public:
		Location();
		Location(std::string path, std::string root, std::vector<std::string> methods);
		~Location();
		std::string getPath();
		std::string getRoot();
		std::vector<std::string> getMethods();
}
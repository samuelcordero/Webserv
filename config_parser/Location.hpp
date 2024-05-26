#pragma once

#include "config_parser.hpp"

class Location {
	private:
		std::vector<std::string>	index;
		std::string 				uri;
		std::vector<std::string>	methods;
	
	public:
		Location(std::vector<std::string> locationBlock);
		~Location();
		std::string					getPath();
		std::string					getRoot();
		std::vector<std::string>	getMethods();	
		void						setMethods(size_t i, const std::vector<std::string> &locationBlock);
		void						setIndex(size_t i, const std::vector<std::string> &locationBlock);
		void						setCgi(size_t i, const std::vector<std::string> &locationBlock);
};

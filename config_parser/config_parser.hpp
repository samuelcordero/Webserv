#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
//#include "Location.hpp"
//#include "Server.hpp"

enum	States{
	START,
	ERROR,
	SERVER,
	SERVER_OPEN,
	SERVER_CLOSE,
	KEYWORD,
	VALUE,
	SEMICOLON,
	LOCATION,
	LOCATION_URI,
	LOCATION_OPEN,
	LOCATION_CLOSE
};

const	std::string keywords[] = {
	"name",
	"listen",
	"cgi",
	"root",
	"methods",
	"index",
};

class Parser{
	private:
		std::vector<std::string>	words;
		std::string					trimedFile;

	public:
		Parser();
		~Parser();
		int		autoStatus(int x, int y);
		void	trimComments(std::string filePath);
		void	splitWords();
		int		setValues();
		int		getStatus(std::string word, int prevStatus);
};

std::vector<std::string> splitConfigFile(std::string configFile);
std::string readConfigFile(std::string filePath);

#endif
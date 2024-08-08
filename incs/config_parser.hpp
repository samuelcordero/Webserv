#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "Location.hpp"
#include "Server.hpp"
#include <cstdlib>

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

class Server;

const	std::string keywords[] = {
	"name",
	"listen",
	"cgi",
	"root",
	"methods",
	"index",
	"Max_Body_Size",
	"autoindex",
	"error",
};

class Parser{
	private: 
		std::vector<Server>			servers;
		std::vector<std::string>	words;
		std::string					trimedFile;
		std::string					filePath;
		bool						no_error;

	public:
		Parser(std::string FilePath);
		~Parser();
		int		autoStatus(int x, int y);
		void	trimComments();
		void	splitWords();
		int		setValues();
		int		getStatus(std::string word, int prevStatus);
		void	createServers();
		//void	run();
		std::vector<Server> getServers();
		bool	noErrors();
};

std::vector<std::string> splitConfigFile(std::string configFile);
std::string readConfigFile(std::string filePath);

#endif
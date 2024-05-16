#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
//#include "Location.hpp"
//#include "Server.hpp"


std::vector<std::string> parseConfigFile(std::string configFile);
std::string readConfigFile(std::string filePath);

#endif
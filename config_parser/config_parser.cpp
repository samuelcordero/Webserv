#include "config_parser.hpp"

std::string readConfigFile(std::string filePath){
	std::ifstream file(filePath);

	if (!file.is_open()) {
		std::cerr << "Error: could not open file" << std::endl;
		return "";
	}
	std::ostringstream oss;
	oss << file.rdbuf();
	return (oss.str());	
}

std::vector<std::string> parseConfigFile(std::string configFile) {
	std::vector<std::string>	servers;
	std::string					delimiter = "server";
	size_t						pos = 0;

	while ((pos = configFile.find(delimiter, pos)) != std::string::npos)
	{
		int		openBrackets = 0;
		int		closeBrackets = 0;
		size_t	startPos = pos;
		for (size_t i = pos + delimiter.length(); i < configFile.size(); i++)
		{
			if (configFile[i] == '{')
				openBrackets++;
			else if (configFile[i] == '}')
				closeBrackets++;
			if (openBrackets > 0 && openBrackets == closeBrackets)
			{
				servers.push_back(configFile.substr(startPos, i - startPos + 1));
				pos = i + 1;
				break;
			}
		}
		if (openBrackets > 0 && openBrackets == closeBrackets)
			continue;
		
		pos += delimiter.length();
	}
	return (servers);
}

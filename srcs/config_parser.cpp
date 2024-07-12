#include "config_parser.hpp"

Parser::Parser(std::string FilePath){
	no_error = false;
	this->filePath = FilePath;
	this->trimComments();
	this->splitWords();
	if (this->setValues())
		no_error = true;
}

std::string readConfigFile(std::string filePath){ //read the config file
	std::ifstream file(filePath.c_str());

	if (!file.is_open()) {
		std::cerr << "Error: could not open file" << std::endl;
		return "";
	}
	std::ostringstream oss;
	oss << file.rdbuf();
	return (oss.str());	
}

std::vector<std::string> splitConfigFile(std::string configFile) { //split the server config into blocks
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
			if (configFile[i] == '[')
				openBrackets++;
			else if (configFile[i] == ']')
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

Parser::~Parser(){}

int	Parser::autoStatus(int x, int y)
{
	int status[][12] = {
	{1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // START 0
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // ERROR 1
	{1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1}, //SERVER 2
	{1, 1, 1, 1, 1, 5, 1, 1, 8, 1, 1, 1}, // SERVER_OPEN 3
	{1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // SERVER_CLOSE 4
	{1, 1, 1, 1, 1, 1, 6, 1, 1, 1, 1, 1}, // KEYWORD 5
	{1, 1, 1, 1, 1, 1, 6, 7, 1, 1, 1, 1}, // VALUE 6
	{1, 1, 1, 1, 4, 5, 1, 1, 8, 1, 1, 11}, // SEMICOLON 7
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 1, 1}, // LOCATION 8
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 10, 1}, // LOCATION_URI 9
	{1, 1, 1, 1, 1, 5, 1, 1, 1, 1, 1, 1}, // LOCATION_OPEN 10
	{1, 1, 1, 1, 4, 5, 1, 1, 8, 1, 1, 1} // LOCATION_CLOSE 11
	};

	return (status[x][y]);
}

static void	trimLine(std::string &line)
{
	size_t endPos = line.find_last_not_of(" \t\r\n\f\v");
    if (endPos != std::string::npos) {
        line.erase(endPos + 1);
    } else {
        line.clear();
    }
}

void	Parser::trimComments()
{
	std::ifstream	inputFile(this->filePath.c_str());

	if (!inputFile.is_open())
	{
		std::cerr << "Couldn´t open the file" << std::endl;
		return;
	}

	std::string	line;
	while (getline(inputFile, line))
	{
		if (line.empty())
			this->trimedFile += "\n";
		size_t commentPos = line.find('#');
		if (commentPos != std::string::npos)
			line = line.substr(0, commentPos);
		

		trimLine(line);
		
		if (!line.empty())
			this->trimedFile += line + "\n";
	}

	//std::cout << this->trimedFile;
	inputFile.close();
}

void	Parser::splitWords()
{
	std::istringstream	splited(this->trimedFile);
	std::string			word;
	while (splited >> word)
	{
		if (word == ";")
			this->words.push_back(";");
		else if (word[word.length() - 1] == ';')
		{
			this->words.push_back(word.substr(0, word.length() - 1));
			this->words.push_back(";");
		}
		else
			this->words.push_back(word);
	}

	//for (size_t i = 0; i < this->words.size(); i++)
		//std::cout << this->words[i] << std::endl;
}

int	Parser::setValues()
{
	size_t	i = 0;
	int		prevStatus = 0;

	while (i < this->words.size())
	{
		prevStatus = this->getStatus(this->words[i], prevStatus);
		if (prevStatus == 1)
		{
			std::cerr << "Error non valid config file." << std::endl;
			return (0);
		}
		i++;
	}
	this->createServers();
	return (1);
}

static	bool finder(std::string word)
{
	for (size_t i = 0; i < 6; i++)
	{
		if (keywords[i] == word)
			return true;
	}
	return false;
}

int	Parser::getStatus(std::string word, int prevStatus)
{
	int	status;
	if (word == "server")
		status = 2;
	else if (word == "[")
		status = 3;
	else if (word == "]")
		status = 4;
	else if (finder(word) == true)
		status = 5;
	else if (word == ";")
		status = 7;
	else if (word == "location")
		status = 8;
	else if (prevStatus == 8)
		status = 9;
	else if (word == "{")
		status = 10;
	else if (word == "}")
		status = 11;
	else
		status = 6;
	//std::cout << word << " [" << status << "]" << std::endl;
	//std::cout << "---------------------------" << std::endl;
	//std::cout << this->autoStatus(prevStatus, status) << std::endl;
	return (this->autoStatus(prevStatus, status));
}

void	Parser::createServers()
{
	std::string	File = readConfigFile(this->filePath);
	std::vector<std::string> Blocks = splitConfigFile(File);

	for (size_t i = 0; i < Blocks.size(); i++)
	{
		this->servers.push_back(Server(Blocks[i]));
		//std::cout << i << std::endl;
	}
}

/* void	Parser::run()
{
	std::cerr << "Starting servers...\n";
	for (size_t i = 0; i < this->Servers.size(); i++)
		this->Servers[i].start();
	std::cerr << "Started " << this->Servers.size() << " servers!\n";
	while (true) {
		for (size_t i = 0; i < this->Servers.size(); i++)
		{
			this->Servers[i].serverRun();
		}
	}
} */

std::vector<Server> Parser::getServers() {
	return servers;
}

bool Parser::noErrors() { return no_error; }

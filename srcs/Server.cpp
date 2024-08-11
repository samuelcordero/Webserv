#include "Server.hpp"

Server::Server() {
	listener = NULL;
	errorHandler = NULL;
}

Server::Server(std::string serverBlock)
{
	this->block = serverBlock;
	//std::cout << serverBlock << std::endl;
	this->maxBodySize = 1000000;
	this->splitBlock();
	this->fillValues();
	listener = new TCPListener(port, this);
	errorHandler = new Errors(errorMap);
}

Server& Server::operator=(const Server& copy)
{
	//std::cerr << "called equal op server\n";
	this->name = copy.name;
	this->port = copy.port;
	this->listen = copy.listen;
	for (size_t i = 0; i < copy.locations.size(); ++i) {
		this->locations.push_back(Location(copy.locations[i]));
	}
	this->block = copy.block;
	this->words = copy.words;
	this->maxBodySize = copy.maxBodySize;
	this->listener = new TCPListener(*copy.listener, this);
	//delete errorHandler;
	errorMap = copy.errorMap;
	this->errorHandler = new Errors(copy.errorMap);
	return (*this);
}

Server::Server(const Server& copy)
{
	//std::cerr << "called copy cons server\n";
	*this = copy;
}

Server::~Server() {
	delete listener;
	delete errorHandler;
}


void	Server::splitBlock()
{
	std::istringstream	splited(this->block);
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

void	Server::fillValues()
{
	for (size_t i = 0; i < this->words.size(); i++)
	{
		if (this->words[i] == "name")
			this->setName(i + 1);
		else if (this->words[i] == "listen")
			this->setListen(i);
		else if (this->words[i] == "location")
			this->createLocation(i);
		else if (this->words[i] == "Max_Body_Size")
			this->setMaxBodySize(i);
		else if (this->words[i] == "error")
			this->addToErrorMap(i);
	}
}

void	Server::setName(size_t i)
{
	while (i < this->words.size())
	{
		if (this->words[i + 1] == ";")
		{
			this->name = this->words[i];
			break;
		}
		i++;
	}
}

static	int	extractPort(const std::string &address)
{
	size_t colonPos = address.find(':');

	if (colonPos == std::string::npos)
	{
		std::cerr << "Couldn´t find port" << std::endl;
		return (-1);
	}

	std::string	strPort = address.substr(colonPos + 1);

	int	port = std::atoi(strPort.c_str());

	if (port == 0 && strPort != "0")
	{
		std::cerr << "Non valid port" << std::endl;
		return (-1);
	}

	return (port);
}

void	Server::setListen(size_t i)
{
	while (i < this->words.size())
	{
		if (this->words[i + 1] == ";")
		{
			this->listen = this->words[i];
			this->port = extractPort(this->words[i]);
			break;
		}
		i++;
	}
}


void	Server::createLocation(size_t i)
{
	std::vector<std::string>	aux;
	
	i++;
	while (this->words[i] != "}")
	{
		aux.push_back(this->words[i]);
		i++;	
	}
	this->locations.push_back(Location(aux));
}

std::vector<Location> &Server::getLocations()
{
	return (this->locations);
}

void	Server::setMaxBodySize(size_t i)
{
	while (i < this->words.size())
	{
		if (this->words[i + 1] == ";")
		{	
			char *endPtr;
			this->maxBodySize = std::strtoul(this->words[i].c_str(), &endPtr, 10);
			if (*endPtr != '\0' || endPtr == this->words[i].c_str())
			{
				std::cerr << "Error non valid body size, set to default" << std::endl;
				this->maxBodySize = 1000000;
				return ;
			}
			break;
		}
		i++;
	}
}

void	Server::addToErrorMap(size_t i)
{
	if (words[i + 3] == ";")
	{	
		errorMap[words[i + 1]] = words[i + 2];
	}
}

int	Server::start(EventManager *eventManager) {
	listener->setEventManager(eventManager);
	return listener->start();
}

std::pair<int, int>	Server::event(epoll_event ev) {
	return listener->checkEvent(ev);
}

int Server::getSocketFd() {
	return listener->getSocketFd();
}

Response Server::error(int error_code) {
	return errorHandler->getError(error_code);
}

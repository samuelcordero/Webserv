#include "Location.hpp"

Location::Location(std::vector<std::string> locationBlock)
{
	flagsMethods = 0;
	autoindex = false;
	for (size_t i = 0; i < locationBlock.size(); i++)
	{
		if (locationBlock[i][0] == '/')
			this->uri = locationBlock[i];
		else if (locationBlock[i] == "index")
			i = this->setIndex(i + 1, locationBlock);
		else if (locationBlock[i] == "methods")
			i = this->setMethods(i + 1, locationBlock);
		else if (locationBlock[i] == "cgi")
			i = this->setCgi(i + 1, locationBlock);
		else if (locationBlock[i] == "root")
			i = this->setRoot(i + 1, locationBlock);
		else if (locationBlock[i] == "autoindex")
			i = this->setAutoIndex(i + 1, locationBlock);
	}
	if (this->uri[this->uri.size() - 1] != '/')
		this->uri += "/";
	std::cout << "\n new location with uri " << uri << std::endl;
	std::cout << "methods: " <<  this->flagsMethods << std::endl;
	std::cout << "cgi: " << this->cgi.first << " " << this->cgi.second << std::endl;
	std::cout << "Index:" << std::endl;
	std::cout << "Autoindex: " << autoindex << std::endl;
	for (size_t c = 0; c < index.size(); ++c) {
		std::cout << index[c] << std::endl;
	}
	std::cout << "Index end" << std::endl;
}

Location::Location(const Location &other) {
	*this = other;
}

Location &Location::operator=(const Location &other) {
	this->uri = other.uri;
	this->root = other.root;
	this->index.clear();
	this->index = other.index;
	this->flagsMethods = other.flagsMethods;
	this->cgi = other.cgi;
	this->autoindex = other.autoindex;
	return *this;
}

Location::~Location(){}

size_t	Location::setRoot(size_t i, std::vector<std::string> &locationBlock)
{
	while (i < locationBlock.size())
	{
		if (locationBlock[i + 1] == ";")
		{
			this->root = locationBlock[i];
			break;
		}
		i++;
	}
	return (i + 1);
}

size_t	Location::setIndex(size_t i, std::vector<std::string> &locationBlock)
{
	while (i < locationBlock.size() && locationBlock[i] != ";")
	{
		this->index.push_back(locationBlock[i]);
		i++;
	}
	return (i);
}

size_t	Location::setMethods(size_t i, const std::vector<std::string> &locationBlock)
{
	while (locationBlock[i] != ";")
	{
		if (locationBlock[i] == "POST")
			this->flagsMethods |= 1;
		else if (locationBlock[i] == "GET")
			this->flagsMethods |= 2;
		else if (locationBlock[i] == "DELETE")
			this->flagsMethods |= 4;
		else
			std::cerr << "Method not suported: " << locationBlock[i] << std::endl;
		i++;
	}
	return (i);
}

size_t	Location::setCgi(size_t i, std::vector<std::string> &locationBlock)
{
	while (i < locationBlock.size())
	{
		if (locationBlock[i] == ";" && (locationBlock[i - 2] == ".py" || locationBlock[i - 2] == ".php"))
		{
			this->cgi.first = locationBlock[i - 2];
			this->cgi.second = locationBlock[i - 1];
			break ;
		}
		++i;
	}
	return (i);
}

size_t	Location::setAutoIndex(size_t i, std::vector<std::string> &locationBlock) {
	while (locationBlock[i] != ";")
	{
		if (locationBlock[i] == "on" || locationBlock[i] == "true")
			this->autoindex = true;
		else
			this->autoindex = false;
		++i;
	}
	return (i);
}


int	Location::getMethods()
{
	return (this->flagsMethods);
}

bool	Location::hasAutoIndex() {
	return autoindex;
}

std::string	Location::getAutoIndex() {
	Indexer i(root);
	return i.getHtml();
}

std::string	Location::getUri()
{
	return (this->uri);
}

std::string Location::getRoot()
{
	return (this->root);
}

std::vector<std::string> Location::getIndex()
{
	return (this->index);
}

std::pair<std::string, std::string>	Location::getCgi() {
	return cgi;
}

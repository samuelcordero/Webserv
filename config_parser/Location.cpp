#include "Location.hpp"

Location::Location(std::vector<std::string> locationBlock)
{	
	for (size_t i = 0;i < locationBlock.size(); i++)
	{
		if (locationBlock[i][0] == '/')
			this->uri = locationBlock[i];
		else if (locationBlock[i] == "index")
			this->setIndex(i + 1, locationBlock);
		else if (locationBlock[i] == "accepted_methods")
			this->setMethods(i + 1, locationBlock);
		else if (locationBlock[i] == "cgi")
			this->setCgi(i + 1, locationBlock);
	}
	std::cout << this->cgi.first << " " << this->cgi.second << std::endl;
	std::cout << this->uri << std::endl;
	std::cout << this->flagsMethods << std::endl;
	std::cout << this->index << std::endl;
}

Location::~Location(){}

void	Location::setIndex(size_t i, std::vector<std::string> &locationBlock)
{
	while (i < locationBlock.size())
	{
		if (locationBlock[i + 1] == ";")
		{
			this->index = locationBlock[i];
			break;
		}
		i++;
	}
}

void	Location::setMethods(size_t i, const std::vector<std::string> &locationBlock)
{
	while (i < locationBlock.size() && locationBlock[i] != ";")
	{
		if (locationBlock[i] == "POST")
			this->flagsMethods |= 1;
		else if (locationBlock[i] == "GET")
			this->flagsMethods |= 2;
		else if (locationBlock[i] == "DELETE")
			this->flagsMethods |= 4;
		else
			{
				std::cerr << "Method not suported: " << locationBlock[i] << std::endl;
				i++;
			}
	}
}

void	Location::setCgi(size_t i, std::vector<std::string> &locationBlock)
{
	while (i < locationBlock.size())
	{
		if (locationBlock[i] == ";" && (locationBlock[i - 2] == ".py" || locationBlock[i - 2] == ".php"))
		{
			this->cgi.first = locationBlock[i - 2];
			this->cgi.second = locationBlock[i - 1];
		}		
	}
}

#include "Location.hpp"

Location::Location(std::vector<std::string> locationBlock)
{
	size_t		i = 0;
	
	while (i < locationBlock.size())
	{
		if (locationBlock[i][0] == '/')
			this->uri = locationBlock[i];
		else if (locationBlock[i] == "index")
			this->setIndex(i, locationBlock);
		else if (locationBlock[i] == "accepted_methods")
			this->setMethods(i, locationBlock);
		else if (locationBlock[i] == "cgi")
			this->setCgi(i, locationBlock);
	}
}

void	

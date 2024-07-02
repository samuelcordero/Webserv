#include "config_parser.hpp"
#include "CgiHandler.hpp"

int main()
{
	Parser parse("config/default.conf");

	// test cgi
	std::cout << "CGI TEST" << std::endl;
	CGIHandler cgiHandler("CGI/test.py");
	cgiHandler.handleRequest();
	std::cout << "CGI TEST: Begining of output data -> " << cgiHandler.getOutputData() << "<- End of output data." << std::endl;
	std::cout << "CGI TEST END" << std::endl;
	// end of cgi test

	parse.run();
}
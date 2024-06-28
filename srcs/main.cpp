#include "config_parser.hpp"
#include "CGIHandler.hpp"

int main()
{
	Parser parse("config/default.conf");

	// test cgi
	std::cout << "CGI TEST" << std::endl;
	CGIHandler cgiHandler("CGI/test.py");
	cgiHandler.handleRequest();
	std::cout << "CGI TEST END" << std::endl;
	// end of cgi test

	parse.run();
}
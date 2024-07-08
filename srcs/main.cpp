#include "config_parser.hpp"
#include "CgiHandler.hpp"

int main()
{
	Parser parse("config/default.conf");

	/* // test cgi
	std::cout << "CGI TEST" << std::endl;
	CGIHandler cgiHandler1("CGI/test.py");
	cgiHandler1.handleRequest();
	std::cout << "CGI TEST: Begining of output data 1 -> " << cgiHandler1.getOutputData() << "<- End of output data 1." << std::endl;

	CGIHandler cgiHandler2("CGI/long_test.py");
	cgiHandler2.handleRequest();
	std::cout << "CGI TEST: Begining of output data 2 -> " << cgiHandler2.getOutputData() << "<- End of output data 2." << std::endl;
	std::cout << "CGI TEST END" << std::endl;
	// end of cgi test */

	parse.run();
}
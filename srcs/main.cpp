#include "Controller.hpp"
#include "CgiHandler.hpp"
#include "Indexer.hpp"
#include "extraFunctions.hpp"

int main(int argc, char **argv)
{
	Controller c;
	std::string config_file_path;

	if (argc > 2)
	{
		std::cerr << "Arguments should be one config file\n";
		return (1);
	}
	if (argc == 1)
		config_file_path = "config/default.conf";
	else
		config_file_path = argv[1];

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
	/*
		// directory listing test
		Indexer indexer("/");
		std::cout << "Indexer path: " << indexer.getPath() << std::endl;
		std::cout << "Indexer html: " << indexer.getHtml() << std::endl;
		// end of directory listing test
	 */

	// MIME type test
	std::string filename;
	std::string mimeType;

	filename = "index.html";
	mimeType = getMimeType(filename);
	std::cout << "The MIME type of " << filename << " is: " << mimeType << std::endl;

	filename = "photo.jpg";
	mimeType = getMimeType(filename);
	std::cout << "The MIME type of " << filename << " is: " << mimeType << std::endl;

	filename = "song.mp3";
	mimeType = getMimeType(filename);
	std::cout << "The MIME type of " << filename << " is: " << mimeType << std::endl;

	filename = "document.pdf";
	mimeType = getMimeType(filename);
	std::cout << "The MIME type of " << filename << " is: " << mimeType << std::endl;

	filename = "archive.zip";
	mimeType = getMimeType(filename);
	std::cout << "The MIME type of " << filename << " is: " << mimeType << std::endl;
	// end of MIME type test

	if (c.parse(config_file_path))
		c.run();
	else
	{
		std::cerr << "Config file contains errors. Exiting...\n";
		exit(EXIT_FAILURE);
	}
}
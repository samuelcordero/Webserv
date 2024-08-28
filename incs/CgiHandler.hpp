#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include <string>
#include <sys/types.h>
#include <fstream>

#include "Request.hpp"

class CGIHandler
{
public:
    CGIHandler(const std::string &scriptPath, const std::string &interpreter, int client_fd, const Request &r);
    ~CGIHandler();
	void handleRequest();
    std::string getOutputData();
	int	getWriteEnd();
	int getReadEnd();
	int getClientFd();
	bool	executionDone();
	void	checkAndKill();
	int 	getExitCode();

private:
    std::string scriptPath;
    std::string postData;
    std::string outputData;
	std::string interpreter;
    int pipein[2];
	int pipeout[2];
	int status;
	int	exitCode;
	const Request 	&request;
	int	client_fd;
	pid_t pid;

    // Environment variables
    std::string requestMethod;
    std::string queryString;
    std::string contentType;
    std::string contentLength;

    void readEnvironmentVariables();
    void readPostData();
    void executeCGIScript();
    void setCGIEnvironment();
    bool waitForChildProcess();

	std::string extractQueryStr(const std::string& url);
};

#endif // CGI_HANDLER_HPP

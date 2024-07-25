#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include <string>
#include <sys/types.h>

class CGIHandler
{
public:
    CGIHandler(const std::string &scriptPath, const std::string &interpreter, int client_fd);
    void handleRequest();
    std::string getOutputData();
	int	getWriteEnd();
	int getReadEnd();
	int getClientFd();

private:
    std::string scriptPath;
    std::string postData;
    std::string outputData;
	std::string interpreter;
    int pipein[2];
	int pipeout[2];
	int	client_fd;

    // Environment variables
    std::string requestMethod;
    std::string queryString;
    std::string contentType;
    std::string contentLength;

    void readEnvironmentVariables();
    void readPostData();
    void executeCGIScript();
    void setCGIEnvironment();
    void waitForChildProcess(pid_t pid, int pipefd[]);
};

#endif // CGI_HANDLER_HPP

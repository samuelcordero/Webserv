#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include <string>

class CGIHandler
{
public:
    CGIHandler(const std::string &scriptPath);
    void handleRequest();
    std::string getOutputData();

private:
    std::string scriptPath;
    std::string postData;
    std::string outputData;
    int pipefd[2];

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
    int *getPipeFd();
};

#endif // CGI_HANDLER_HPP

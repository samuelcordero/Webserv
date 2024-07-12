#include "CgiHandler.hpp"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstring>
#include <vector>

CGIHandler::CGIHandler(const std::string &scriptPath)
    : scriptPath(scriptPath) {}

void CGIHandler::handleRequest()
{
    readEnvironmentVariables();
    readPostData();
    executeCGIScript();
}

void CGIHandler::readEnvironmentVariables()
{
    if (const char *method = std::getenv("REQUEST_METHOD"))
        requestMethod = method;
    if (const char *query = std::getenv("QUERY_STRING"))
        queryString = query;
    if (const char *type = std::getenv("CONTENT_TYPE"))
        contentType = type;
    if (const char *length = std::getenv("CONTENT_LENGTH"))
        contentLength = length;
}

void CGIHandler::readPostData()
{
    if (requestMethod == "POST" && !contentLength.empty())
    {
        int len = std::atoi(contentLength.c_str());
        postData.resize(len);
        std::cin.read(&postData[0], len);
    }
}

void CGIHandler::executeCGIScript()
{
    if (pipe(pipefd) == -1)
    {
        std::cerr << "Failed to create pipe\n";
        return;
    }

    pid_t pid = fork();
    if (pid == 0)
    {
        // Child process
        close(pipefd[0]);               // Close read end of pipe
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
        dup2(pipefd[1], STDERR_FILENO); // Redirect stderr to pipe
        close(pipefd[1]);

        setCGIEnvironment();

        // Prepare arguments for execve
        std::vector<char *> argv;
        argv.push_back(const_cast<char *>(scriptPath.c_str()));
        argv.push_back(NULL); // The last element of argv must be NULL

        // Prepare environment variables for execve
        std::vector<char *> envp;
        if (!requestMethod.empty())
            envp.push_back(const_cast<char *>(("REQUEST_METHOD=" + requestMethod).c_str()));
        if (!queryString.empty())
            envp.push_back(const_cast<char *>(("QUERY_STRING=" + queryString).c_str()));
        if (!contentType.empty())
            envp.push_back(const_cast<char *>(("CONTENT_TYPE=" + contentType).c_str()));
        if (!contentLength.empty())
            envp.push_back(const_cast<char *>(("CONTENT_LENGTH=" + contentLength).c_str()));
        if (!postData.empty())
            envp.push_back(const_cast<char *>(("POST_DATA=" + postData).c_str()));
        envp.push_back(NULL); // The last element of envp must be NULL

        execve(scriptPath.c_str(), argv.data(), envp.data());

        // If execve fails, the following lines will execute
        std::cerr << "Failed to execute CGI script\n";
        std::exit(1);
    }
    else if (pid > 0)
    {
        // Parent process
        close(pipefd[1]); // Close write end of pipe
        waitForChildProcess(pid, pipefd);
        close(pipefd[0]);
    }
    else
    {
        // Fork failed
        std::cerr << "Failed to fork process\n";
    }
}

void CGIHandler::setCGIEnvironment()
{
    if (!requestMethod.empty())
        setenv("REQUEST_METHOD", requestMethod.c_str(), 1);
    if (!queryString.empty())
        setenv("QUERY_STRING", queryString.c_str(), 1);
    if (!contentType.empty())
        setenv("CONTENT_TYPE", contentType.c_str(), 1);
    if (!contentLength.empty())
        setenv("CONTENT_LENGTH", contentLength.c_str(), 1);
    if (!postData.empty())
        setenv("POST_DATA", postData.c_str(), 1);
}

void CGIHandler::waitForChildProcess(pid_t pid, int pipefd[])
{
    int status;
    pid_t result = waitpid(pid, &status, WNOHANG);

    if (result == 0)
    {
        // Child process is still running
        sleep(1); // Wait for 1 seconds
        result = waitpid(pid, &status, WNOHANG);
        if (result == 0)
        {
            // Child process is still running after timeout
            kill(pid, SIGKILL); // Kill the child process
            outputData = "Error: Timeout";
            return;
        }
    }

    if (WIFEXITED(status))
    {
        char buffer[1024];
        ssize_t bytesRead;
        std::cout << "Output of execution:\n"
                  << std::endl;
        std::cout << "Content-Type: text/html\r\n\r\n";
        while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0)
        {
            buffer[bytesRead] = '\0';
            // comment the stdout to avoid debug printing
            // std::cout << buffer;
            // append the output to outputData member
            outputData.append(buffer, bytesRead);
        }
    }
    else
    {
        std::cerr << "CGI script execution failed\n";
    }
}

std::string CGIHandler::getOutputData()
{
    return outputData;
}

int *CGIHandler::getPipeFd()
{
    return pipefd;
}

#include "CgiHandler.hpp"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstring>
#include <vector>
#include <ext/stdio_filebuf.h>

CGIHandler::CGIHandler(const std::string &scriptPath, const std::string &interpreter, int client_fd, const Request &r)
    : scriptPath(scriptPath), interpreter(interpreter), request(r), client_fd(client_fd) {
	//create pipes
	if (pipe(pipein) == -1)
    {
        std::cerr << "Failed to create pipe\n";
        return;
    }

	if (pipe(pipeout) == -1)
    {
        std::cerr << "Failed to create pipe\n";
		close(pipein[0]);
        close(pipein[1]);
        return;
    }
	//handleRequest();
}

CGIHandler::~CGIHandler() {}

void	CGIHandler::checkAndKill() {
	if (!executionDone())
		kill(pid, SIGKILL);
}


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
	requestMethod = request.getMethod();
	queryString = request.getHeaders()["Query-String"];
	contentType = request.getHeaders()["Content-Type"];
	contentLength = request.getHeaders()["Content-Length"];
	postData = request.getBody();
	/* //here we should read from pipein[0]
	__gnu_cxx::stdio_filebuf<char> filebuf( pipein[0], std::ios_base::in );
    std::istream inputStream( &filebuf );
    if (requestMethod == "POST" && !contentLength.empty())
    {
        int len = std::atoi(contentLength.c_str());
        postData.resize(len);
        inputStream.read(&postData[0], len);
    } */
}

void CGIHandler::executeCGIScript()
{
    /* if (pipe(pipein) == -1)
    {
        std::cerr << "Failed to create pipe\n";
        return;
    }

	if (pipe(pipeout) == -1)
    {
        std::cerr << "Failed to create pipe\n";
		close(pipein[0]);
        close(pipein[1]);
        return;
    } */

    pid = fork();
    if (pid == 0)
    {
        // Child process
        close(pipeout[0]);               // Close read end of pipe
        dup2(pipeout[1], STDOUT_FILENO); // Redirect stdout to pipe
        dup2(pipeout[1], STDERR_FILENO); // Redirect stderr to pipe
		close(pipeout[1]);

		close(pipein[1]);               // Close write end of pipe
        dup2(pipein[0], STDIN_FILENO); // Redirect stdin to pipe
		close(pipein[0]);

        setCGIEnvironment();

        // Prepare arguments for execve
        std::vector<char *> argv;
		argv.push_back(const_cast<char *>(interpreter.c_str()));
        argv.push_back(const_cast<char *>(scriptPath.c_str()));
        argv.push_back(NULL); // The last element of argv must be NULL

		/* for (int i = 0; argv[i]; ++i)
			std::cout << argv[i] << std::endl; */

        // Prepare environment variables for execve
        std::vector<std::string> env_strings;
		std::vector<char *> envp;

		if (!requestMethod.empty())
			env_strings.push_back("REQUEST_METHOD=" + requestMethod);
		if (!queryString.empty())
			env_strings.push_back("QUERY_STRING=" + queryString);
		if (!contentType.empty())
			env_strings.push_back("CONTENT_TYPE=" + contentType);
		if (!contentLength.empty())
			env_strings.push_back("CONTENT_LENGTH=" + contentLength);
		if (!postData.empty())
			env_strings.push_back("POST_DATA=" + postData);

		// Here we extract the c_str() pointers from the strings, ensuring that the memory remains valid.
		for (std::vector<std::string>::size_type i = 0; i < env_strings.size(); ++i) {
    		envp.push_back(const_cast<char*>(env_strings[i].c_str()));
		}

		envp.push_back(NULL); // The last element of envp must be NULL

        execve(interpreter.c_str(), argv.data(), envp.data());

        // If execve fails, the following lines will execute
        std::cerr << "Failed to execute CGI script\n";
        std::exit(1);
    }
    else if (pid > 0)
    {
        // Parent process
        close(pipeout[1]); // Close write end of pipe
        //waitForChildProcess(pid, pipefd);
        close(pipein[0]);
    }
    else
    {
        // Fork failed
        std::cerr << "Failed to fork process\n";
		close(pipein[0]);
        close(pipein[1]);
		close(pipeout[0]);
        close(pipeout[1]);
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

bool CGIHandler::waitForChildProcess()
{
    int status;
    pid_t result = waitpid(pid, &status, WNOHANG);

	if (result == 0) // process not done
		return false;
	return true;

    /* if (result == 0)
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
    } */
}

std::string CGIHandler::getOutputData()
{
    return outputData;
}

int CGIHandler::getReadEnd()
{
    return pipeout[0];
}

int CGIHandler::getWriteEnd()
{
    return pipein[1];
}

int CGIHandler::getClientFd()
{
    return client_fd;
}

bool	CGIHandler::executionDone() {
	return waitForChildProcess();
}

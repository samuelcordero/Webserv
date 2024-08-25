#ifndef EXTRA_FUNCTIONS_HPP
#define EXTRA_FUNCTIONS_HPP

#include <iostream>
#include <string>
#include <cctype>
#include <ctime>
#include <sys/stat.h>
#include <sys/time.h>

std::string getMimeType(const std::string &filename);
std::string	urlDecoder(const std::string &url);
std::string getHost(const std::string& hostport);
bool isDirectory(const std::string& path);
long long			getCurrentEpochMillis();
bool 				isTimeout(long long startMillis, long long endMillis, int thresholdSeconds);

#endif // EXTRA_FUNCTIONS_HPP
#ifndef EXTRA_FUNCTIONS_HPP
#define EXTRA_FUNCTIONS_HPP

#include <iostream>
#include <string>
#include <cctype>
#include <sys/stat.h>

std::string getMimeType(const std::string &filename);
std::string	urlDecoder(const std::string &url);
std::string getHost(const std::string& hostport);
bool isDirectory(const std::string& path);


#endif // EXTRA_FUNCTIONS_HPP
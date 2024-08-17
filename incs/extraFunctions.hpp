#ifndef EXTRA_FUNCTIONS_HPP
#define EXTRA_FUNCTIONS_HPP

#include <iostream>
#include <string>
#include <cctype>

std::string getMimeType(const std::string &filename);
std::string	urlDecoder(const std::string &url);
std::string getHost(const std::string& hostport);


#endif // EXTRA_FUNCTIONS_HPP
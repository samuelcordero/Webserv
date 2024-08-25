#include "extraFunctions.hpp"

#include <iostream>
#include <string>

std::string getMimeType(const std::string &filename)
{
    // Find the position of the last dot in the filename
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos)
    {
        return "application/octet-stream"; // Default binary stream
    }

    // Extract the file extension
    std::string extension = filename.substr(dotPos + 1);

    // Define common MIME types
    if (extension == "html" || extension == "htm")
        return "text/html";
    if (extension == "css")
        return "text/css";
    if (extension == "js")
        return "application/javascript";
    if (extension == "json")
        return "application/json";
    if (extension == "xml")
        return "application/xml";
    if (extension == "txt")
        return "text/plain";
    if (extension == "csv")
        return "text/csv";

    // Images
    if (extension == "png")
        return "image/png";
    if (extension == "jpg" || extension == "jpeg")
        return "image/jpeg";
    if (extension == "gif")
        return "image/gif";
    if (extension == "bmp")
        return "image/bmp";
    if (extension == "webp")
        return "image/webp";
    if (extension == "ico")
        return "image/vnd.microsoft.icon";
    if (extension == "svg")
        return "image/svg+xml";
    if (extension == "tiff" || extension == "tif")
        return "image/tiff";

    // Audio/Video
    if (extension == "mp3")
        return "audio/mpeg";
    if (extension == "wav")
        return "audio/wav";
    if (extension == "ogg")
        return "audio/ogg";
    if (extension == "mp4")
        return "video/mp4";
    if (extension == "webm")
        return "video/webm";
    if (extension == "avi")
        return "video/x-msvideo";

    // Application types
    if (extension == "pdf")
        return "application/pdf";
    if (extension == "zip")
        return "application/zip";
    if (extension == "rar")
        return "application/vnd.rar";
    if (extension == "gz")
        return "application/gzip";
    if (extension == "tar")
        return "application/x-tar";
    if (extension == "7z")
        return "application/x-7z-compressed";

    // Microsoft Office types
    if (extension == "doc")
        return "application/msword";
    if (extension == "docx")
        return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    if (extension == "xls")
        return "application/vnd.ms-excel";
    if (extension == "xlsx")
        return "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    if (extension == "ppt")
        return "application/vnd.ms-powerpoint";
    if (extension == "pptx")
        return "application/vnd.openxmlformats-officedocument.presentationml.presentation";

    // If the extension is not recognized, return the default MIME type
    return "application/octet-stream";
}

static int hexToDecimal(char first, char second) {
    int hexValue = 0;
    
    if (first >= '0' && first <= '9') {
        hexValue += (first - '0') * 16;
    } else if (first >= 'a' && first <= 'f') {
        hexValue += (first - 'a' + 10) * 16;
    } else if (first >= 'A' && first <= 'F') {
        hexValue += (first - 'A' + 10) * 16;
    }

    if (second >= '0' && second <= '9') {
        hexValue += (second - '0');
    } else if (second >= 'a' && second <= 'f') {
        hexValue += (second - 'a' + 10);
    } else if (second >= 'A' && second <= 'F') {
        hexValue += (second - 'A' + 10);
    }

    return hexValue;
}


std::string	urlDecoder(const std::string &url)
{
	std::string	ret;
	for (size_t i = 0; i < url.size(); i++)
	{
		if(url[i] == '%' && std::isxdigit(url[i + 1]) && std::isxdigit(url[i + 2]))
		{
			int dec = hexToDecimal(url[i + 1], url[i + 2]);
			ret += static_cast<char>(dec);
			i += 2;
		}
		else
			ret += url[i];
	}
	return (ret);
}

std::string getHost(const std::string& hostport) {
    // Buscar el carácter ':' en el string
    size_t posicion = hostport.find(':');
    
    // Si encuentra el carácter ':', devuelve la parte antes de él (el host)
    if (posicion != std::string::npos) {
        return hostport.substr(0, posicion);
    }
    
    // Si no encuentra ':', se asume que todo el string es el host
    return hostport;
}

bool isDirectory(const std::string& path) {
    struct stat statbuf;
    // Check if the stat call is successful
    if (stat(path.c_str(), &statbuf) != 0) {
        // Error occurred (file doesn't exist, or we don't have permission)
        return false;
    }
    // Use S_ISDIR macro to check if it's a directory
    return S_ISDIR(statbuf.st_mode);
}

//returns time since epoch in milliseconds
long long getCurrentEpochMillis() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return static_cast<long long>(tv.tv_sec) * 1000 + tv.tv_usec / 1000;
}

//returns true once the threshold is greater or equal to the time diff
bool isTimeout(long long startMillis, long long endMillis, int thresholdSeconds) {
    long long diffMillis = endMillis - startMillis;
    long long diffSeconds = diffMillis / 1000;
    return diffSeconds >= thresholdSeconds;
}

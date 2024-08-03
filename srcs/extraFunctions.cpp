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
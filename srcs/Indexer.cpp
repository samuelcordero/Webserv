#include "Indexer.hpp"

Indexer::Indexer(std::string path)
{
    this->path = path;
    // lists all the files on the directory specivied by the variable path with readdir
    // fliters all the folders and all the files
    // and creates an html file with the list of files and links to each one of them, stored in the variable html

    this->html = "<!DOCTYPE html>\n<html>\n<head>\n<title>Index of " + path + "</title>\n</head>\n<body>\n<h1>Index of " + path + "</h1>\n<hr>\n<ul>\n";
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (ent->d_type == DT_DIR)
            {
                this->html += "<li><a href=\"" + std::string(ent->d_name) + "/\">" + std::string(ent->d_name) + "/</a></li>\n";
            }
            else if (ent->d_type == DT_REG)
            {
                this->html += "<li><a href=\"" + std::string(ent->d_name) + "\">" + std::string(ent->d_name) + "</a></li>\n";
            }
        }
        closedir(dir);
    }
    this->html += "</ul>\n<hr>\n</body>\n</html>";
}

Indexer::Indexer(const Indexer &other)
{
    *this = other;
}

Indexer &Indexer::operator=(const Indexer &other)
{
    if (this != &other)
    {
        this->path = other.path;
        this->html = other.html;
    }
    return *this;
}

Indexer::~Indexer()
{
}

std::string Indexer::getPath() const
{
    return this->path;
}

std::string Indexer::getHtml() const
{
    return this->html;
}
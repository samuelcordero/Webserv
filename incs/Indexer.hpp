#ifndef INDEXER_HPP
#define INDEXER_HPP

#include <string>
#include <dirent.h>

class Indexer
{
private:
    std::string path;
    std::string html;

public:
    Indexer(std::string path);
    Indexer(const Indexer &);
    Indexer &operator=(const Indexer &);
    ~Indexer();

    std::string getPath() const;
    std::string getHtml() const;
};

#endif // INDEXER_HPP
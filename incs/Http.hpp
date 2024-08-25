#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <map>

enum State {
    STATE_START,
    STATE_HEADERS,
    STATE_BODY,
	STATE_TOTAL_CHUNKS_SIZE,
    STATE_CHUNK_SIZE,
    STATE_CHUNK_DATA,
    STATE_CHUNK_END,
	STATE_INCOMPLETE,
    STATE_COMPLETE,
    STATE_INVALID
};

class Http {
private:
    State state;
    std::string method, path, version;
    std::map<std::string, std::string> headers;
    size_t contentLength;
    size_t chunkSize, totalChunkSize;
    std::string bodyBuffer;

    bool parseRequestLine(const std::string& line);
    void parseHeader(const std::string& line);
    bool processBody(const std::string& data, size_t& bytesRead);
	bool parseTotalChunkSize(const std::string& line);
    bool parseChunkSize(const std::string& line);
    bool processChunkData(const std::string& data, size_t& bytesRead);
    std::string trim(const std::string& str);

public:
    Http();

    State parse(const std::string& data);
    bool isComplete() const;
    bool isValid() const;
};
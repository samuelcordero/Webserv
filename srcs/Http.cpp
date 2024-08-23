#include "Http.hpp"
#include <sstream>
#include <cctype>

Http::Http() : state(STATE_START), contentLength(0), bytesRead(0), chunkSize(0) {}

State Http::parse(const std::string& data) {
    size_t pos = 0;
    std::string line;

    while (pos < data.size()) {
        size_t endPos = data.find("\r\n", pos);
        if (endPos == std::string::npos) {
            break; // No se encontró el delimitador, salir del bucle
        }
        
        line = data.substr(pos, endPos - pos);
		//std::cerr << "at line (" << line << ")\n";

        switch (state) {
            case STATE_START:
                if (parseRequestLine(line)) {
                    state = STATE_HEADERS;
                } else {
                    state = STATE_INVALID;
                }
                break;

            case STATE_HEADERS:
                if (line.empty()) {
                    // Fin de cabeceras
                    if (headers.find("Transfer-Encoding") != headers.end() &&
                        headers["Transfer-Encoding"] == "chunked") {
                        state = STATE_CHUNK_SIZE;
                    } else if (headers.find("Content-Length") != headers.end()) {
                        std::istringstream lengthStream(headers["Content-Length"]);
                        lengthStream >> contentLength;
                        state = STATE_BODY;
                    } else {
                        state = STATE_COMPLETE; // No hay cuerpo, la solicitud está completa
                    }
                } else {
                    parseHeader(line);
                }
                break;

            case STATE_BODY:
                if (processBody(data, pos)) {
                    state = STATE_COMPLETE;
                }
                break;

            case STATE_CHUNK_SIZE:
                if (parseChunkSize(line)) {
                    state = (chunkSize == 0) ? STATE_CHUNK_END : STATE_CHUNK_DATA;
                } else {
                    state = STATE_INVALID;
                }
                break;

            case STATE_CHUNK_DATA:
                if (processChunkData(data, pos)) {
                    state = STATE_CHUNK_SIZE; // Listo para el siguiente chunk
                }
                break;

            case STATE_CHUNK_END:
                // Manejar el final de la transferencia en chunks (especificado por "\r\n" después del chunk final)
                if (line.empty()) {
                    state = STATE_COMPLETE;
                }
                break;

            case STATE_COMPLETE:
            case STATE_INVALID:
                return state;
        }
		pos = endPos + 2; // Mover a la siguiente línea
    }

    return state;
}

bool Http::isComplete() const {
    return state == STATE_COMPLETE;
}

bool Http::isValid() const {
    return state != STATE_INVALID;
}

bool Http::parseRequestLine(const std::string& line) {
    std::istringstream lineStream(line);
    lineStream >> method >> path >> version;

    return (method == "GET" || method == "POST" || method == "DELETE") &&
           (version == "HTTP/1.1");
}

void Http::parseHeader(const std::string& line) {
    size_t colonPos = line.find(':');
    if (colonPos != std::string::npos) {
        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);
        headers[key] = trim(value);
    }
}

bool Http::processBody(const std::string& data, size_t& bytesRead) {
    // Cálculo de bytes en el cuerpo
    size_t bodyStart = data.find("\r\n\r\n");
    if (bodyStart == std::string::npos) {
        return false; // No se ha encontrado el cuerpo aún
    }

    size_t bodyLength = data.size() - (bodyStart + 4);
    bytesRead += bodyLength;
    return bytesRead >= contentLength;
}

bool Http::parseChunkSize(const std::string& line) {
    std::istringstream lineStream(line);
    lineStream >> std::hex >> chunkSize;
    return !lineStream.fail();
}

bool Http::processChunkData(const std::string& data, size_t& bytesRead) {
    size_t chunkDataStart = data.find("\r\n", bytesRead);
    if (chunkDataStart == std::string::npos) {
        return false; // No se ha encontrado el chunk aún
    }

    size_t chunkDataLength = chunkDataStart - bytesRead;
    bytesRead += chunkDataLength;
    return bytesRead >= chunkSize;
}

std::string Http::trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

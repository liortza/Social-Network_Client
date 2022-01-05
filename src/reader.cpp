#include "../include/reader.h"
#include <connectionHandler.h>
#include <iostream>

Reader::Reader(ConnectionHandler &handler, bool &shouldTerminate) : _handler(handler), _shouldTerminate(shouldTerminate) {}

void Reader::read() {
    while (!_shouldTerminate) {
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
        std::string line(buf);
        if (!_handler.sendLine(line)) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            break;
        }
        if (line == "LOGOUT") break;
    }
}
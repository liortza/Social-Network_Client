#include "../include/reader.h"
#include <connectionHandler.h>
#include <iostream>

Reader::Reader(ConnectionHandler &handler, bool &shouldTerminate, bool &logout) : _handler(handler),
                                                                                  _shouldTerminate(shouldTerminate),
                                                                                  _logout(logout) {}

void Reader::read() {
    while (1) {
        while (_logout) {
            if (_shouldTerminate) return;
        }
        std::cout << "continue" << std::endl;
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
        std::string line(buf);
        if (!_handler.sendLine(line)) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            break;
        }
        if (line == "LOGOUT") _logout = true;
    }
}
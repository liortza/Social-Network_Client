#include "../include/reader.h"
#include <connectionHandler.h>
#include <iostream>

Reader::Reader(ConnectionHandler &handler, bool &shouldTerminate) : _handler(handler), _shouldTerminate(shouldTerminate) {
    std::cout << "created reader \n" << std::endl;
}

void Reader::read() {
    while (!_shouldTerminate) {
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
        std::string line(buf);
        int len = line.length();
        if (!_handler.sendLine(line)) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            break;
        }
        // connectionHandler.sendLine(line) appends '\n' to the message. Therefore, we send len+1 bytes.
        std::cout << "Sent " << len + 1 << " bytes to server" << std::endl;
    }
}
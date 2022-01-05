#include "../include/writer.h"
#include <connectionHandler.h>
#include <iostream>

Writer::Writer(ConnectionHandler &handler, bool &shouldTerminate) : _handler(handler), _shouldTerminate(shouldTerminate) {}

void Writer::write() {
    while (1) {
        std::string answer;
        if (!_handler.getLine(answer)) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            break;
        }

        int len = answer.length();
        answer.resize(len - 1);
        std::cout << answer << std::endl;
        if (answer == "ACK 3") {
            std::cout << "Exiting...\n" << std::endl;
            _shouldTerminate = true;
            break;
        }
    }
}
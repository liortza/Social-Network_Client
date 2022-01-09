#ifndef CLIENT_READER_H
#define CLIENT_READER_H

#include "connectionHandler.h"

class Reader {
private:
    ConnectionHandler &_handler;
    bool &_shouldTerminate;
    bool &_logout;

public:
    Reader(ConnectionHandler &handler, bool &shouldTerminate, bool &logout);
    void read();
};


#endif //CLIENT_READER_H
